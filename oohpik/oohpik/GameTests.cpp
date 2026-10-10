// Written with the aid of Claude Opus 5.5
// Game tests for oohpik. Run "oohpik.exe --test" from the build folder (bin\x64\Debug),
// so the Resources folder and df-font.ttf are next to it.
//
// Tests run in independent batches (Normal, Errors, Edge Cases). Each batch owns its own
// pass/fail counters and starts and shuts down the engine itself, so batches can be run
// individually or together without their counts colliding. Results are logged to
// dragonfly.log in the same format as the engine tests in transmission/tests.cpp.
//
// The tests play the real game: they run frames the same way GameManager::run() does,
// but send key presses directly instead of reading the keyboard.

#include "GameTests.h"

// Engine includes
#include "GameManager.h"
#include "WorldManager.h"
#include "DisplayManager.h"
#include "ResourceManager.h"
#include "LogManager.h"
#include "EventManager.h"
#include "TimerManager.h"
#include "EventKeyboard.h"
#include "EventCollision.h"
#include "EventStep.h"
#include "Clock.h"

// Game includes
#include "TitleScreen.h"
#include "Hero.h"
#include "GameOver.h"
#include "HighScores.h"
#include "Level.h"
#include "MapBuilder.h"
#include "Seed.h"
#include "MapExit.h"

#include <algorithm>
#include <chrono>
#include <climits>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <limits>
#include <set>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

namespace {

// Logs each result the same way as the engine tests: an optional detail line,
// then "test: <name> result: passed|failed".
struct TestBatch {
	const char* name;
	int pass_count = 0;
	int fail_count = 0;

	void check(bool condition, const char* test_name, const std::string& detail = "") {
		df::LogManager& logkeeper = df::LogManager::getInstance();
		const char* result = condition ? "passed" : "failed";

		if (condition) {
			pass_count++;
		}
		else {
			fail_count++;
		}

		// LogManager is down once a batch has shut the engine down,
		// so fall back to printf with the same format, like the LogManager suite does.
		if (!logkeeper.isStarted()) {
			if (!detail.empty()) {
				std::printf("%s\n", detail.c_str());
			}
			std::printf("test: %s result: %s\n", test_name, result);
			return;
		}

		if (!detail.empty()) {
			logkeeper.writeLog("%s", detail.c_str());
		}
		logkeeper.writeLog("test: %s result: %s", test_name, result);
	}

	int logSummary() const {
		df::LogManager& logkeeper = df::LogManager::getInstance();
		if (fail_count > 0) {
			logkeeper.writeLog("suite failed: %s failures: %d", name, fail_count);
		}
		else {
			logkeeper.writeLog("suite passed: %s", name);
		}
		return fail_count;
	}
};

// Menu option order in TitleScreen.cpp
const int MENU_PLAY = 0;
const int MENU_CONTROLS = 1;
const int MENU_HIGH_SCORES = 2;
const int MENU_QUIT = 3;

// Seed range set in Level.cpp's makeMapConfig()
const int MIN_SEEDS = 15;
const int MAX_SEEDS = 30;

// Owl hop offsets per facing, from Hero::forward(): 0 = up, 1 = right, 2 = down, 3 = left
const int HOP_X[] = { 0, 2, 0, -2 };
const int HOP_Y[] = { -1, 0, 1, 0 };
const int FACING_LEFT = 3;

// Frames a new map may take before a test gives up on it (about 10 seconds)
const int MAP_WAIT_FRAMES = 300;

// GameOver ignores keys for this many steps after the owl dies
const int DEATH_INPUT_DELAY = 30;

// A tile just left of the map (the map starts at x = 2), used to test the window edge
const df::Vector LEFT_EDGE_TILE(0, 15);

const char* HIGH_SCORE_FILE = "highscores.csv";
const char* HIGH_SCORE_BACKUP = "highscores.csv.testbackup";
const char* HIGH_SCORE_HEADER = "name,seeds,levels,time\n";
const char* OLD_HIGH_SCORE_HEADER = "seeds,levels,time\n"; // files saved before names existed

unsigned long long step_count = 0;
int probes_destroyed = 0;

// Counts its own deletions, to check that shutting down deletes every object
class ShutdownProbe : public df::Object {
public:
	ShutdownProbe() {
		setType("ShutdownProbe");
		setSolidness(df::SPECTRAL);
		setVisible(false);
	}
	~ShutdownProbe() {
		probes_destroyed++;
	}
};

// --- Running the game ---

// Start the engine and load the sprites the game uses. Returns false if either fails.
// Sounds and music are left out so the test run is silent; the game already treats them as optional.
bool startGame(bool append_log) {
	if (GM.startUp(append_log) != 0) {
		return false;
	}
	df::LogManager::getInstance().setFlush(true);

	bool loaded = true;
	loaded = RM.loadSprite("Resources/Sprites/player.sprite", "hero") == 0 && loaded;
	loaded = RM.loadSprite("Resources/Sprites/tree.sprite", "tree") == 0 && loaded;
	loaded = RM.loadSprite("Resources/Sprites/seed.sprite", "seed") == 0 && loaded;
	loaded = RM.loadSprite("Resources/Sprites/exit.sprite", "exit") == 0 && loaded;
	return loaded;
}

// One pass of GameManager::run()'s loop. A key, if given, is sent where the InputManager
// would send it: after the step event and before the world updates. key_repeats sends it
// that many times in the same frame, like several presses arriving between two frames.
void runFrame(df::Keyboard::Key key = df::Keyboard::UNDEFINED_KEY, df::EventKeyboardAction action = df::KEY_PRESSED,
	int key_repeats = 1) {
	df::Clock frame_clock;

	df::EventStep step(step_count++);
	GM.onEvent(&step);

	// Empty the window's event queue so Windows doesn't flag it as not responding.
	// Real key presses are dropped so they can't disturb the tests.
	sf::RenderWindow* p_window = df::DisplayManager::getInstance().getWindow();
	if (p_window != nullptr) {
		while (p_window->pollEvent()) {
		}
	}

	if (key != df::Keyboard::UNDEFINED_KEY) {
		df::EventKeyboard keyboard;
		keyboard.setKey(key);
		keyboard.setKeyboardAction(action);
		for (int press = 0; press < key_repeats; press++) {
			GM.onEvent(&keyboard);
		}
	}

	df::TimerManager::getInstance().update();
	WM.update();
	WM.draw();
	df::DisplayManager::getInstance().swapBuffers();

	// Keep the real frame rate, so the map generation thread gets the same time it would in game
	long long elapsed_ms = frame_clock.split() / 1000;
	if (elapsed_ms < GM.getFrameTime()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(GM.getFrameTime() - elapsed_ms));
	}
}

void runFrames(int count) {
	for (int i = 0; i < count; i++) {
		runFrame();
	}
}

// A quick tap: the key is pressed in this frame and let go before the next one
void pressKey(df::Keyboard::Key key) {
	runFrame(key, df::KEY_PRESSED);
	df::EventKeyboard release;
	release.setKey(key);
	release.setKeyboardAction(df::KEY_RELEASED);
	GM.onEvent(&release);
}

// Press a key and keep it held down (let go with releaseKey)
void holdKey(df::Keyboard::Key key) {
	runFrame(key, df::KEY_PRESSED);
}

void releaseKey(df::Keyboard::Key key) {
	runFrame(key, df::KEY_RELEASED);
}

// Send a keyboard event straight away, with any key and action, including ones the
// InputManager never sends (an undefined key or action, or a value outside the Key enum)
void sendKeyboardEvent(df::Keyboard::Key key, df::EventKeyboardAction action) {
	df::EventKeyboard keyboard;
	keyboard.setKey(key);
	keyboard.setKeyboardAction(action);
	GM.onEvent(&keyboard);
}

// --- Finding things in the world ---

// The most recently created object of a type (highest id), or nullptr if there is none
df::Object* findNewest(const std::string& type) {
	df::ObjectList objects = WM.objectsOfType(type);
	df::Object* p_newest = nullptr;
	for (int i = 0; i < objects.getCount(); i++) {
		if (p_newest == nullptr || objects[i]->getId() > p_newest->getId()) {
			p_newest = objects[i];
		}
	}
	return p_newest;
}

Hero* findHero() {
	return static_cast<Hero*>(findNewest("Hero"));
}

TitleScreen* findTitleScreen() {
	return static_cast<TitleScreen*>(findNewest("TitleScreen"));
}

GameOver* findGameOver() {
	return static_cast<GameOver*>(findNewest("GameOver"));
}

// Run frames until the newest map builder has finished building, then let it send its
// done event. Returns false if the map isn't built within MAP_WAIT_FRAMES frames.
bool waitForMap() {
	for (int frame = 0; frame < MAP_WAIT_FRAMES; frame++) {
		ookpik::MapBuilder* p_builder = static_cast<ookpik::MapBuilder*>(findNewest("mapBuilder"));
		if (p_builder != nullptr && p_builder->isMapBuildFinished()) {
			runFrames(2); // BUILD_DONE -> SENDING_EVENT -> DONE
			return true;
		}
		runFrame();
	}
	return false;
}

// How far the newest map builder got, for the detail line of a map that didn't load
std::string describeMapBuilder() {
	ookpik::MapBuilder* p_builder = static_cast<ookpik::MapBuilder*>(findNewest("mapBuilder"));
	if (p_builder == nullptr) {
		return "no map builder in the world";
	}
	return std::string("map builder: generation finished: ") + (p_builder->isMapGenFinished() ? "yes" : "no") +
		", build finished: " + (p_builder->isMapBuildFinished() ? "yes" : "no");
}

// Number of trees, ground tiles, seeds and exits in the world
int mapObjectCount() {
	return WM.objectsOfTypeCount("Tree") + WM.objectsOfTypeCount("Ground") +
		WM.objectsOfTypeCount("Seed") + WM.objectsOfTypeCount("mapExit");
}

// Frames a map builder with a bad config gets to try (and retry) before the test checks it
const int BAD_MAP_FRAMES = 30;

// Start a map builder with a config and let its errors and retries play out.
// Returns true if the bad config was caught: the builder never finished and built nothing.
// Clears any map already in the world first, so map objects can be counted.
bool mapConfigIsRejected(const ookpik::MapGenConfig& config) {
	clearMap();
	runFrame(); // any old map is deleted here
	df::Object* p_owl = new df::Object(); // stands in for the owl; the builder only moves it
	ookpik::MapBuilder* p_builder = new ookpik::MapBuilder();
	p_builder->startGenerateMap(config, p_owl);
	runFrames(BAD_MAP_FRAMES);
	bool rejected = !p_builder->isMapBuildFinished() && mapObjectCount() == 0;
	WM.markForDelete(p_builder);
	WM.markForDelete(p_owl);
	runFrame();
	return rejected;
}

// Build a map from a config straight away (no title screen or Hero), to check what the
// generator makes. Returns false if it doesn't finish. Remove it with clearMap() afterwards.
bool buildTestMap(const ookpik::MapGenConfig& config, df::Object* p_owl) {
	clearMap();
	runFrame(); // the old map is deleted here
	ookpik::MapBuilder* p_builder = new ookpik::MapBuilder();
	return p_builder->startGenerateMap(config, p_owl) == 0 && waitForMap();
}

// A tile position as whole numbers, for comparing and looking up positions
std::pair<int, int> tileOf(df::Vector position) {
	return std::make_pair((int)std::lround(position.getX()), (int)std::lround(position.getY()));
}

// Every tree, seed and exit as "type x y", sorted, plus the owl's start: the whole layout of a map
std::vector<std::string> mapLayout(df::Object* p_owl) {
	std::vector<std::string> layout;
	const char* types[] = { "Tree", "Seed", "mapExit" };
	for (const char* type : types) {
		df::ObjectList objects = WM.objectsOfType(type);
		for (int i = 0; i < objects.getCount(); i++) {
			std::pair<int, int> tile = tileOf(objects[i]->getPosition());
			layout.push_back(std::string(type) + " " + std::to_string(tile.first) + " " + std::to_string(tile.second));
		}
	}
	std::sort(layout.begin(), layout.end());
	std::pair<int, int> owl_tile = tileOf(p_owl->getPosition());
	layout.push_back("owl " + std::to_string(owl_tile.first) + " " + std::to_string(owl_tile.second));
	return layout;
}

// Trees, seeds and exits that share a tile with another one (a correct map has none)
int sharedTileCount() {
	std::set<std::pair<int, int>> used;
	int shared = 0;
	const char* types[] = { "Tree", "Seed", "mapExit" };
	for (const char* type : types) {
		df::ObjectList objects = WM.objectsOfType(type);
		for (int i = 0; i < objects.getCount(); i++) {
			if (!used.insert(tileOf(objects[i]->getPosition())).second) {
				shared++;
			}
		}
	}
	return shared;
}

// Trees, seeds and exits outside the window
int outsideWindowCount() {
	df::DisplayManager& display = df::DisplayManager::getInstance();
	int outside = 0;
	const char* types[] = { "Tree", "Seed", "mapExit" };
	for (const char* type : types) {
		df::ObjectList objects = WM.objectsOfType(type);
		for (int i = 0; i < objects.getCount(); i++) {
			df::Vector position = objects[i]->getPosition();
			if (position.getX() < 0 || position.getY() < 0 ||
				position.getX() >= display.getHorizontal() || position.getY() >= display.getVertical()) {
				outside++;
			}
		}
	}
	return outside;
}

// Walk every tile the owl can hop to from start (trees block it; reaching the exit ends the
// level, so it isn't walked through). Returns how many seeds the owl can't reach, and sets
// exit_reached if the exit can be reached. If p_reached is given, it gets every tile reached.
int unreachableSeeds(df::Vector start, bool& exit_reached, std::set<std::pair<int, int>>* p_reached = nullptr) {
	df::DisplayManager& display = df::DisplayManager::getInstance();
	std::set<std::pair<int, int>> trees;
	df::ObjectList tree_list = WM.objectsOfType("Tree");
	for (int i = 0; i < tree_list.getCount(); i++) {
		trees.insert(tileOf(tree_list[i]->getPosition()));
	}
	std::set<std::pair<int, int>> exits;
	df::ObjectList exit_list = WM.objectsOfType("mapExit");
	for (int i = 0; i < exit_list.getCount(); i++) {
		exits.insert(tileOf(exit_list[i]->getPosition()));
	}

	exit_reached = false;
	std::set<std::pair<int, int>> visited;
	std::vector<std::pair<int, int>> to_visit;
	to_visit.push_back(tileOf(start));
	visited.insert(tileOf(start));
	while (!to_visit.empty()) {
		std::pair<int, int> tile = to_visit.back();
		to_visit.pop_back();
		for (int dir = 0; dir < 4; dir++) {
			std::pair<int, int> next = std::make_pair(tile.first + HOP_X[dir], tile.second + HOP_Y[dir]);
			if (next.first < 0 || next.second < 0 || next.first >= display.getHorizontal() || next.second >= display.getVertical() ||
				trees.count(next) > 0 || visited.count(next) > 0) {
				continue;
			}
			visited.insert(next);
			if (exits.count(next) > 0) {
				exit_reached = true;
			}
			else {
				to_visit.push_back(next);
			}
		}
	}

	int unreachable = 0;
	df::ObjectList seed_list = WM.objectsOfType("Seed");
	for (int i = 0; i < seed_list.getCount(); i++) {
		if (visited.count(tileOf(seed_list[i]->getPosition())) == 0) {
			unreachable++;
		}
	}
	if (p_reached != nullptr) {
		*p_reached = visited;
	}
	return unreachable;
}

// Time the work in the game loop with whatever is in the world now, averaged over some
// frames (in ms): the step event, the world update, drawing, and showing the frame
// (swapBuffers, which is where batched characters are actually drawn). No sleeping.
struct FrameTimes {
	double step = 0;
	double update = 0;
	double draw = 0;
	double show = 0;
	double total() const { return step + update + draw + show; }
};

FrameTimes timeFrames(int frames) {
	long long step_us = 0, update_us = 0, draw_us = 0, show_us = 0;
	df::Clock clock;
	for (int frame = 0; frame < frames; frame++) {
		clock.delta();
		df::EventStep step(step_count++);
		GM.onEvent(&step);
		step_us += clock.delta();
		WM.update();
		update_us += clock.delta();
		WM.draw();
		draw_us += clock.delta();
		df::DisplayManager::getInstance().swapBuffers();
		show_us += clock.delta();
	}
	FrameTimes times;
	times.step = step_us / 1000.0 / frames;
	times.update = update_us / 1000.0 / frames;
	times.draw = draw_us / 1000.0 / frames;
	times.show = show_us / 1000.0 / frames;
	return times;
}

// Lowest and highest ids among the map's trees, ground, seeds and exits.
// Ids only go up, so every object of a newer map has a higher id than any older one.
void mapObjectIdRange(unsigned long long& lowest, unsigned long long& highest) {
	const char* types[] = { "Tree", "Ground", "Seed", "mapExit" };
	lowest = ULLONG_MAX;
	highest = 0;
	for (const char* type : types) {
		df::ObjectList objects = WM.objectsOfType(type);
		for (int i = 0; i < objects.getCount(); i++) {
			if (objects[i]->getId() < lowest) {
				lowest = objects[i]->getId();
			}
			if (objects[i]->getId() > highest) {
				highest = objects[i]->getId();
			}
		}
	}
}

// True if the owl could stand on this tile: inside the window and below the status line
bool isInsideWindow(df::Vector position) {
	df::DisplayManager& display = df::DisplayManager::getInstance();
	return position.getX() >= 0 && position.getY() >= 1 &&
		position.getX() < display.getHorizontal() && position.getY() < display.getVertical();
}

// True if nothing solid (a tree, seed or exit) is on this tile
bool isFreeTile(Hero* p_hero, df::Vector position) {
	return isInsideWindow(position) && WM.getCollisions(p_hero, position).isEmpty();
}

// Find a free tile next to p_target that the owl can hop from to land on it.
// Sets stand and facing and returns true, or returns false if every side is blocked.
bool findApproach(Hero* p_hero, df::Object* p_target, df::Vector& stand, int& facing) {
	for (int dir = 0; dir < 4; dir++) {
		df::Vector from(p_target->getPosition().getX() - HOP_X[dir], p_target->getPosition().getY() - HOP_Y[dir]);
		if (isFreeTile(p_hero, from)) {
			stand = from;
			facing = dir;
			return true;
		}
	}
	return false;
}

// Find an object of a type the owl can reach in one hop, or nullptr if there is none
df::Object* findReachable(Hero* p_hero, const std::string& type, df::Vector& stand, int& facing) {
	df::ObjectList objects = WM.objectsOfType(type);
	for (int i = 0; i < objects.getCount(); i++) {
		if (findApproach(p_hero, objects[i], stand, facing)) {
			return objects[i];
		}
	}
	return nullptr;
}

// Find a seed with free tiles on both sides in a line, so the owl can hop onto it and then
// straight on past it. Sets stand (the tile before it) and facing, or returns nullptr.
df::Object* findSeedToHopOver(Hero* p_hero, df::Vector& stand, int& facing) {
	df::ObjectList seeds = WM.objectsOfType("Seed");
	for (int i = 0; i < seeds.getCount(); i++) {
		df::Vector seed_position = seeds[i]->getPosition();
		for (int dir = 0; dir < 4; dir++) {
			df::Vector from(seed_position.getX() - HOP_X[dir], seed_position.getY() - HOP_Y[dir]);
			df::Vector beyond(seed_position.getX() + HOP_X[dir], seed_position.getY() + HOP_Y[dir]);
			if (isFreeTile(p_hero, from) && isFreeTile(p_hero, beyond)) {
				stand = from;
				facing = dir;
				return seeds[i];
			}
		}
	}
	return nullptr;
}

// Find a tile and direction with `length` free tiles in a straight line ahead of it, for
// watching several hops in a row. Sets stand and facing, or returns false.
bool findStraightRun(Hero* p_hero, int length, df::Vector& stand, int& facing) {
	df::DisplayManager& display = df::DisplayManager::getInstance();
	for (int y = 1; y < display.getVertical(); y++) {
		for (int x = 0; x < display.getHorizontal(); x += 2) {
			df::Vector start((float)x, (float)y);
			if (!isFreeTile(p_hero, start)) {
				continue;
			}
			for (int dir = 0; dir < 4; dir++) {
				bool clear = true;
				for (int step = 1; step <= length && clear; step++) {
					df::Vector tile(start.getX() + HOP_X[dir] * step, start.getY() + HOP_Y[dir] * step);
					clear = isFreeTile(p_hero, tile);
				}
				if (clear) {
					stand = start;
					facing = dir;
					return true;
				}
			}
		}
	}
	return false;
}

// Turn the owl with real key presses until it faces a direction (each turn counts as a move)
void turnOwl(Hero* p_hero, int facing) {
	for (int turns = 0; turns < 4 && p_hero->getDirection() != facing; turns++) {
		pressKey(df::Keyboard::D);
	}
}

// Move the owl straight to a tile (no hop, so no collisions), then turn it to face a direction
void placeOwl(Hero* p_hero, df::Vector stand, int facing) {
	p_hero->setPosition(stand);
	turnOwl(p_hero, facing);
}

// --- Reading what's shown ---

// Compare positions by their coordinates, so these tests don't depend on Vector's == tolerance
bool samePosition(df::Vector a, df::Vector b) {
	return std::fabs(a.getX() - b.getX()) < 0.001f && std::fabs(a.getY() - b.getY()) < 0.001f;
}

std::string describe(df::Vector position) {
	return "(" + std::to_string((int)position.getX()) + ", " + std::to_string((int)position.getY()) + ")";
}

// The start of the owl's status line for these numbers, up to the level time
std::string expectedStatus(int seeds, int remaining, int moves, int maps) {
	return "seeds: " + std::to_string(seeds) + "    remaining: " + std::to_string(remaining) +
		"    moves: " + std::to_string(moves) + "    maps: " + std::to_string(maps) + "    Level Time: ";
}

bool statusShows(Hero* p_hero, int seeds, int remaining, int moves, int maps) {
	return p_hero->getStatusLine().rfind(expectedStatus(seeds, remaining, moves, maps), 0) == 0;
}

std::string statusDetail(Hero* p_hero, int seeds, int remaining, int moves, int maps) {
	return "expected status to start with: \"" + expectedStatus(seeds, remaining, moves, maps) +
		"\" actual: \"" + p_hero->getStatusLine() + "\"";
}

// The "Level Time" number on the owl's status line, or -1 if it isn't there
double levelTimeShown(Hero* p_hero) {
	std::string status = p_hero->getStatusLine();
	std::string label = "Level Time: ";
	size_t at = status.find(label);
	if (at == std::string::npos) {
		return -1;
	}
	return std::atof(status.c_str() + at + label.length());
}

// --- High score file ---

// Delete the saved table so a test starts from an empty one
void clearHighScores() {
	std::remove(HIGH_SCORE_FILE);
}

void writeHighScoreFile(const std::string& contents) {
	std::ofstream file(HIGH_SCORE_FILE, std::ios::trunc);
	file << contents;
}

std::string readHighScoreFile() {
	std::ifstream file(HIGH_SCORE_FILE);
	std::stringstream contents;
	contents << file.rdbuf();
	return contents.str();
}

// Move the player's real high score table out of the way while the tests run
void backUpHighScores() {
	// A backup left behind means an earlier test run stopped early: put it back first
	std::ifstream old_backup(HIGH_SCORE_BACKUP);
	if (old_backup.good()) {
		old_backup.close();
		std::remove(HIGH_SCORE_FILE);
		std::rename(HIGH_SCORE_BACKUP, HIGH_SCORE_FILE);
	}
	std::rename(HIGH_SCORE_FILE, HIGH_SCORE_BACKUP); // does nothing if there is no table yet
}

void restoreHighScores() {
	std::remove(HIGH_SCORE_FILE);
	std::rename(HIGH_SCORE_BACKUP, HIGH_SCORE_FILE); // does nothing if there was no table
}

// A file holding a full top 10: seeds 20 (best) down to 11 (worst), no levels, 1 second each
std::string fullHighScoreFile() {
	std::string contents = HIGH_SCORE_HEADER;
	for (int seeds = 20; seeds > 20 - MAX_HIGH_SCORES; seeds--) {
		contents += "TST," + std::to_string(seeds) + ",0,1.00\n";
	}
	return contents;
}

// Type a string on the death screen's initials prompt, one key press per character
void typeInitials(const std::string& text) {
	for (char c : text) {
		if (c >= 'A' && c <= 'Z') {
			pressKey(static_cast<df::Keyboard::Key>(df::Keyboard::A + (c - 'A')));
		}
		else if (c >= '1' && c <= '9') {
			pressKey(static_cast<df::Keyboard::Key>(df::Keyboard::NUM1 + (c - '1')));
		}
		else if (c == '0') {
			pressKey(df::Keyboard::NUM0);
		}
	}
}

// Leave the death screen for the title screen the way a player would: wait out the input
// delay, enter initials if the run made the table, then press a key
void leaveDeathScreen() {
	runFrames(DEATH_INPUT_DELAY);
	GameOver* p_game_over = findGameOver();
	if (p_game_over != nullptr && p_game_over->isEnteringName()) {
		typeInitials("TST");
		pressKey(df::Keyboard::RETURN);
	}
	pressKey(df::Keyboard::SPACE);
	runFrame(); // the death screen returns to the title here
}

TestBatch runBatchNormal() {
	TestBatch batch{ "Normal" };

	bool started = startGame(false);
	batch.check(started, "engine starts and the game sprites load");
	if (!started) {
		GM.shutDown();
		return batch;
	}
	clearHighScores();

	// --- Title screen menu ---
	new TitleScreen();
	runFrame();
	TitleScreen* p_title = findTitleScreen();
	batch.check(p_title != nullptr, "title screen appears");
	if (p_title == nullptr) {
		GM.shutDown();
		return batch;
	}
	batch.check(p_title->getSelected() == MENU_PLAY, "title screen starts with Play highlighted");

	pressKey(df::Keyboard::S);
	batch.check(p_title->getSelected() == MENU_CONTROLS, "S moves the highlight down one option");
	pressKey(df::Keyboard::W);
	batch.check(p_title->getSelected() == MENU_PLAY, "W moves the highlight up one option");

	pressKey(df::Keyboard::C);
	runFrame();
	batch.check(p_title->isShowingControls(), "C opens the controls guide");
	pressKey(df::Keyboard::SPACE);
	batch.check(!p_title->isShowingControls() && findTitleScreen() != nullptr,
		"any key closes the controls guide and returns to the menu");

	pressKey(df::Keyboard::H);
	runFrame();
	batch.check(p_title->isShowingScores(), "H opens the high score table");
	batch.check(p_title->getSelected() == MENU_HIGH_SCORES, "a shortcut key also highlights its option");
	pressKey(df::Keyboard::SPACE);
	batch.check(!p_title->isShowingScores(), "any key closes the high score table");

	pressKey(df::Keyboard::W); // High scores -> Controls
	pressKey(df::Keyboard::RETURN);
	runFrame();
	batch.check(p_title->isShowingControls(), "enter selects the highlighted option (Controls)");
	pressKey(df::Keyboard::SPACE);

	pressKey(df::Keyboard::Q);
	runFrame();
	batch.check(GM.getGameOver(), "Q chooses Quit, which ends the game loop");
	GM.setGameOver(false); // keep testing

	pressKey(df::Keyboard::P);
	runFrame(); // Play is carried out here and the title screen is deleted
	batch.check(findTitleScreen() == nullptr, "P chooses Play and the title screen closes");
	batch.check(WM.objectsOfTypeCount("Hero") == 1, "Play creates the owl");
	batch.check(findNewest("mapBuilder") != nullptr, "Play starts building a map");

	// --- Loading into a map ---
	bool map_loaded = waitForMap();
	batch.check(map_loaded, "the first map finishes building", describeMapBuilder());
	Hero* p_hero = findHero();
	if (!map_loaded || p_hero == nullptr) {
		GM.shutDown();
		return batch;
	}

	int seeds_on_map = WM.objectsOfTypeCount("Seed");
	batch.check(seeds_on_map >= MIN_SEEDS && seeds_on_map <= MAX_SEEDS,
		"the map has between 15 and 30 seeds", "seeds on map: " + std::to_string(seeds_on_map));
	batch.check(WM.objectsOfTypeCount("mapExit") == 1,
		"the map has exactly one exit", "exits on map: " + std::to_string(WM.objectsOfTypeCount("mapExit")));
	batch.check(WM.objectsOfTypeCount("Tree") > 0,
		"the map has trees", "trees on map: " + std::to_string(WM.objectsOfTypeCount("Tree")));
	batch.check(isInsideWindow(p_hero->getPosition()),
		"the owl is placed inside the window, below the status line", "owl at " + describe(p_hero->getPosition()));
	batch.check(WM.getCollisions(p_hero, p_hero->getPosition()).isEmpty(),
		"the owl doesn't start on a tree, seed or the exit", "owl at " + describe(p_hero->getPosition()));
	batch.check(statusShows(p_hero, 0, seeds_on_map, 0, 0),
		"status line starts at 0 seeds, every seed remaining, 0 moves, 0 maps",
		statusDetail(p_hero, 0, seeds_on_map, 0, 0));

	// Level Time is shown as seconds with one decimal place, e.g. "Level Time: 1.4 second(s)"
	std::string status = p_hero->getStatusLine();
	std::string time_label = "Level Time: ";
	size_t time_start = status.find(time_label);
	size_t time_end = status.find(" second(s)");
	std::string shown_time = (time_start == std::string::npos || time_end == std::string::npos) ? "" :
		status.substr(time_start + time_label.length(), time_end - (time_start + time_label.length()));
	size_t dot = shown_time.find('.');
	batch.check(dot != std::string::npos && dot > 0 && dot + 2 == shown_time.length(),
		"Level Time is shown to one decimal place", "shown: \"" + shown_time + "\"");

	// --- Moving and turning ---
	pressKey(df::Keyboard::D);
	batch.check(p_hero->getDirection() == 2, "D turns the owl right (from facing right to facing down)");
	pressKey(df::Keyboard::A);
	batch.check(p_hero->getDirection() == 1, "A turns the owl left (back to facing right)");
	batch.check(p_hero->getMoves() == 2,
		"each turn counts as a move", "moves: " + std::to_string(p_hero->getMoves()));

	int free_dir = -1;
	df::Vector hop_to;
	for (int dir = 0; dir < 4 && free_dir == -1; dir++) {
		df::Vector to(p_hero->getPosition().getX() + HOP_X[dir], p_hero->getPosition().getY() + HOP_Y[dir]);
		if (isFreeTile(p_hero, to)) {
			free_dir = dir;
			hop_to = to;
		}
	}
	batch.check(free_dir != -1, "the owl has a free tile next to it");
	if (free_dir != -1) {
		turnOwl(p_hero, free_dir);
		int moves_before_hop = p_hero->getMoves();
		pressKey(df::Keyboard::W);
		batch.check(samePosition(p_hero->getPosition(), hop_to), "W hops the owl one tile forward",
			"expected " + describe(hop_to) + " actual " + describe(p_hero->getPosition()));
		batch.check(p_hero->getMoves() == moves_before_hop + 1, "a hop counts as a move",
			"moves before: " + std::to_string(moves_before_hop) + " after: " + std::to_string(p_hero->getMoves()));
	}

	// --- Holding W hops again every HOLD_REPEAT_STEPS frames (0.2 s), and stops when let go ---
	df::Vector run_start;
	int run_facing = 0;
	bool found_run = findStraightRun(p_hero, 3, run_start, run_facing);
	batch.check(found_run, "found three free tiles in a row to hold W along");
	if (found_run) {
		placeOwl(p_hero, run_start, run_facing);
		int moves_before_hold = p_hero->getMoves();
		holdKey(df::Keyboard::W);
		int hops_on_press = p_hero->getMoves() - moves_before_hold;
		runFrames(HOLD_REPEAT_STEPS - 1);
		int hops_before_delay = p_hero->getMoves() - moves_before_hold;
		runFrame();
		int hops_after_delay = p_hero->getMoves() - moves_before_hold;
		runFrames(HOLD_REPEAT_STEPS);
		int hops_after_second_delay = p_hero->getMoves() - moves_before_hold;
		releaseKey(df::Keyboard::W);
		runFrames(HOLD_REPEAT_STEPS * 2);
		int hops_after_release = p_hero->getMoves() - moves_before_hold;
		df::Vector run_end(run_start.getX() + HOP_X[run_facing] * 3, run_start.getY() + HOP_Y[run_facing] * 3);

		batch.check(hops_on_press == 1, "pressing W hops straight away",
			"hops: " + std::to_string(hops_on_press));
		batch.check(hops_before_delay == 1 && hops_after_delay == 2,
			"holding W hops again after 6 frames (0.2 s), not sooner",
			"hops after 5 frames: " + std::to_string(hops_before_delay) + ", after 6: " + std::to_string(hops_after_delay));
		batch.check(hops_after_second_delay == 3, "holding W keeps hopping every 6 frames",
			"hops: " + std::to_string(hops_after_second_delay));
		batch.check(hops_after_release == 3 && samePosition(p_hero->getPosition(), run_end) && !p_hero->isForwardHeld(),
			"letting go of W stops the hopping",
			"hops: " + std::to_string(hops_after_release) + ", owl at " + describe(p_hero->getPosition()) +
			" expected " + describe(run_end));
	}

	// --- Collecting seeds ---
	df::Vector stand;
	int facing = 0;
	df::Object* p_seed = findReachable(p_hero, "Seed", stand, facing);
	batch.check(p_seed != nullptr, "found a seed the owl can hop onto");
	if (p_seed != nullptr) {
		df::Vector seed_position = p_seed->getPosition();
		int seeds_left = WM.objectsOfTypeCount("Seed");
		placeOwl(p_hero, stand, facing);
		pressKey(df::Keyboard::W); // the seed is deleted at the end of this frame
		batch.check(p_hero->getSeeds() == 1,
			"hopping onto a seed collects it", "seeds collected: " + std::to_string(p_hero->getSeeds()));
		batch.check(WM.objectsOfTypeCount("Seed") == seeds_left - 1,
			"the collected seed is removed from the map",
			"seeds before: " + std::to_string(seeds_left) + " after: " + std::to_string(WM.objectsOfTypeCount("Seed")));
		batch.check(samePosition(p_hero->getPosition(), seed_position), "the owl lands on the seed's tile",
			"expected " + describe(seed_position) + " actual " + describe(p_hero->getPosition()));
		batch.check(statusShows(p_hero, 1, seeds_left - 1, p_hero->getMoves(), 0),
			"status line shows 1 seed collected and one fewer remaining",
			statusDetail(p_hero, 1, seeds_left - 1, p_hero->getMoves(), 0));
	}

	// --- Going to the next level ---
	df::Object* p_exit = findReachable(p_hero, "mapExit", stand, facing);
	batch.check(p_exit != nullptr, "found the exit and a free tile next to it");
	if (p_exit != nullptr) {
		unsigned long long old_lowest = 0;
		unsigned long long old_highest = 0;
		mapObjectIdRange(old_lowest, old_highest);
		int seeds_before = p_hero->getSeeds();

		placeOwl(p_hero, stand, facing);
		pressKey(df::Keyboard::W);
		batch.check(p_hero->getMaps() == 1,
			"reaching the exit counts a completed map", "maps: " + std::to_string(p_hero->getMaps()));

		bool next_loaded = waitForMap();
		batch.check(next_loaded, "the next map finishes building", describeMapBuilder());

		unsigned long long new_lowest = 0;
		unsigned long long new_highest = 0;
		mapObjectIdRange(new_lowest, new_highest);
		batch.check(new_lowest > old_highest, "every object from the old map is removed",
			"newest old map id: " + std::to_string(old_highest) + " oldest map id now: " + std::to_string(new_lowest));

		int new_seeds = WM.objectsOfTypeCount("Seed");
		batch.check(new_seeds >= MIN_SEEDS && new_seeds <= MAX_SEEDS,
			"the next map has between 15 and 30 seeds", "seeds on map: " + std::to_string(new_seeds));
		batch.check(WM.objectsOfTypeCount("mapExit") == 1, "the next map has exactly one exit",
			"exits on map: " + std::to_string(WM.objectsOfTypeCount("mapExit")));
		batch.check(WM.objectsOfTypeCount("mapBuilder") == 1, "only the new map builder is left",
			"map builders: " + std::to_string(WM.objectsOfTypeCount("mapBuilder")));
		batch.check(p_hero->getSeeds() == seeds_before, "collected seeds carry over to the next map",
			"seeds before: " + std::to_string(seeds_before) + " after: " + std::to_string(p_hero->getSeeds()));
		batch.check(WM.getCollisions(p_hero, p_hero->getPosition()).isEmpty(),
			"the owl is placed on a free tile of the new map", "owl at " + describe(p_hero->getPosition()));
		batch.check(statusShows(p_hero, seeds_before, new_seeds, p_hero->getMoves(), 1),
			"status line shows 1 map completed and the new map's seeds",
			statusDetail(p_hero, seeds_before, new_seeds, p_hero->getMoves(), 1));
	}

	// --- Dying ---
	df::Object* p_tree = findReachable(p_hero, "Tree", stand, facing);
	batch.check(p_tree != nullptr, "found a tree the owl can fly into");
	if (p_tree == nullptr) {
		GM.shutDown();
		return batch;
	}
	placeOwl(p_hero, stand, facing);
	int seeds_at_death = p_hero->getSeeds();
	int maps_at_death = p_hero->getMaps();
	pressKey(df::Keyboard::W); // the owl is deleted at the end of this frame
	p_hero = nullptr;
	batch.check(WM.objectsOfTypeCount("Hero") == 0, "flying into a tree kills the owl");

	GameOver* p_game_over = findGameOver();
	batch.check(p_game_over != nullptr, "the death screen appears");
	if (p_game_over == nullptr) {
		GM.shutDown();
		return batch;
	}
	batch.check(p_game_over->getSeeds() == seeds_at_death, "death screen shows the seeds collected",
		"expected " + std::to_string(seeds_at_death) + " actual " + std::to_string(p_game_over->getSeeds()));
	batch.check(p_game_over->getMaps() == maps_at_death, "death screen shows the maps completed",
		"expected " + std::to_string(maps_at_death) + " actual " + std::to_string(p_game_over->getMaps()));
	batch.check(p_game_over->getTime() > 0.0, "death screen shows how long the run lasted",
		"time: " + std::to_string(p_game_over->getTime()));

	// --- Logging high scores ---
	batch.check(p_game_over->getRank() == 0, "the run would rank first in an empty high score table",
		"rank: " + std::to_string(p_game_over->getRank()));
	batch.check(p_game_over->isEnteringName(), "a top-10 run asks for the player's initials");
	batch.check(loadHighScores().empty(), "the run isn't saved before the initials are entered",
		"saved runs: " + std::to_string(loadHighScores().size()));

	runFrames(DEATH_INPUT_DELAY);
	typeInitials("ABX");
	pressKey(df::Keyboard::BACKSPACE);
	typeInitials("C");
	batch.check(p_game_over->getName() == "ABC", "letters type the initials and backspace removes the last one",
		"initials: " + p_game_over->getName());
	pressKey(df::Keyboard::RETURN);
	batch.check(!p_game_over->isEnteringName() && p_game_over->getRank() == 0, "enter saves the run with its initials",
		"rank: " + std::to_string(p_game_over->getRank()));

	std::vector<ScoreEntry> scores = loadHighScores();
	batch.check(scores.size() == 1, "the run is saved to the high score table",
		"saved runs: " + std::to_string(scores.size()));
	if (scores.size() == 1) {
		batch.check(scores[0].name == "ABC", "the saved run has the player's initials", "saved name: " + scores[0].name);
		batch.check(scores[0].seeds == seeds_at_death && scores[0].levels == maps_at_death,
			"the saved run has the run's seeds and levels",
			"saved seeds: " + std::to_string(scores[0].seeds) + " levels: " + std::to_string(scores[0].levels));
		batch.check(std::fabs(scores[0].time - p_game_over->getTime()) < 0.01, "the saved run has the run's time",
			"saved: " + std::to_string(scores[0].time) + " run: " + std::to_string(p_game_over->getTime()));
	}
	batch.check(readHighScoreFile().rfind(HIGH_SCORE_HEADER, 0) == 0, "the high score file starts with its header row");

	int better_rank = submitHighScore(ScoreEntry{ seeds_at_death + 5, 0, 99.0 });
	scores = loadHighScores();
	batch.check(better_rank == 0, "a run with more seeds is ranked above the saved one",
		"rank: " + std::to_string(better_rank));
	batch.check(scores.size() == 2 && scores[0].seeds == seeds_at_death + 5 && scores[1].seeds == seeds_at_death,
		"the high score table is saved best first");

	// --- Back to the title screen ---
	runFrames(DEATH_INPUT_DELAY);
	pressKey(df::Keyboard::SPACE);
	runFrame(); // the death screen returns to the title here
	batch.check(findTitleScreen() != nullptr && findGameOver() == nullptr,
		"a key on the death screen returns to the title screen");
	batch.check(mapObjectCount() == 0 && WM.objectsOfTypeCount("mapBuilder") == 0,
		"the old map is cleared when returning to the title screen",
		"map objects left: " + std::to_string(mapObjectCount()));

	// --- Exiting cleanly ---
	pressKey(df::Keyboard::P);
	runFrame();
	bool replay_loaded = waitForMap();
	batch.check(replay_loaded && findHero() != nullptr, "Play works again after a death", describeMapBuilder());
	if (replay_loaded && findHero() != nullptr) {
		pressKey(df::Keyboard::ESCAPE);
		batch.check(GM.getGameOver(), "escape during play ends the game loop");
	}

	GM.shutDown();
	batch.check(!GM.isStarted() && !WM.isStarted() && !df::DisplayManager::getInstance().isStarted() &&
		!df::EventManager::getInstance().isStarted() && !RM.isStarted() && !df::LogManager::getInstance().isStarted(),
		"shutting down stops every engine manager");

	return batch;
}

TestBatch runBatchError() {
	TestBatch batch{ "Errors" };

	bool started = startGame(true);
	batch.check(started, "engine starts and the game sprites load");
	if (!started) {
		GM.shutDown();
		return batch;
	}
	clearHighScores();

	// --- Map loading: bad configs are caught instead of building a broken map or crashing ---
	// Each case changes one value of the real level config (makeMapConfig) to something invalid.
	ookpik::MapGenConfig bad_config = makeMapConfig();
	bad_config.setMapWidth(0);
	batch.check(mapConfigIsRejected(bad_config), "a map width of 0 is caught");

	bad_config = makeMapConfig();
	bad_config.setMapHeight(-5);
	batch.check(mapConfigIsRejected(bad_config), "a negative map height is caught");

	bad_config = makeMapConfig();
	bad_config.setTimeoutSeconds(0);
	batch.check(mapConfigIsRejected(bad_config), "a generation timeout of 0 seconds is caught");

	bad_config = makeMapConfig();
	bad_config.setObjectsConstructedPerFrame(0);
	batch.check(mapConfigIsRejected(bad_config), "building 0 objects per frame is caught");

	bad_config = makeMapConfig();
	bad_config.setMinSeeds(-3);
	batch.check(mapConfigIsRejected(bad_config), "a negative seed count is caught");

	bad_config = makeMapConfig();
	bad_config.setMinSeeds(40);
	bad_config.setMaxSeeds(10);
	batch.check(mapConfigIsRejected(bad_config), "more min seeds than max seeds is caught");

	bad_config = makeMapConfig();
	bad_config.setMaxSeeds(100000);
	batch.check(mapConfigIsRejected(bad_config), "more seeds than the map has room for is caught");

	bad_config = makeMapConfig();
	bad_config.setMinRooms(9);
	bad_config.setMaxRooms(2);
	batch.check(mapConfigIsRejected(bad_config), "more min rooms than max rooms is caught");

	bad_config = makeMapConfig();
	bad_config.setMinRoomWidth(20);
	bad_config.setMaxRoomWidth(5);
	batch.check(mapConfigIsRejected(bad_config), "a min room width above the max is caught");

	bad_config = makeMapConfig();
	bad_config.setMinRandTrees(200);
	bad_config.setMaxRandTrees(10);
	batch.check(mapConfigIsRejected(bad_config), "more min random trees than max is caught");

	ookpik::MapBuilder* p_no_owl_builder = new ookpik::MapBuilder();
	int no_owl_result = p_no_owl_builder->startGenerateMap(makeMapConfig(), nullptr);
	runFrames(5);
	batch.check(no_owl_result == -1 && !p_no_owl_builder->isMapGenFinished() && mapObjectCount() == 0,
		"starting a map with no owl is refused", "startGenerateMap returned " + std::to_string(no_owl_result));
	WM.markForDelete(p_no_owl_builder);
	runFrame();

	// --- Title screen: keys that should do nothing ---
	new TitleScreen();
	runFrame();
	TitleScreen* p_title = findTitleScreen();
	batch.check(p_title != nullptr, "title screen appears");
	if (p_title == nullptr) {
		GM.shutDown();
		return batch;
	}

	pressKey(df::Keyboard::Z);
	runFrame();
	batch.check(p_title->getSelected() == MENU_PLAY && !p_title->isShowingControls() &&
		!p_title->isShowingScores() && !GM.getGameOver() && findTitleScreen() != nullptr,
		"title screen ignores keys that aren't menu keys");

	releaseKey(df::Keyboard::Q);
	runFrame();
	batch.check(!GM.getGameOver(), "title screen ignores key releases (releasing Q doesn't quit)");

	pressKey(df::Keyboard::C);
	runFrame();
	pressKey(df::Keyboard::Q);
	runFrame();
	batch.check(!GM.getGameOver() && !p_title->isShowingControls(),
		"Q on the controls guide only closes the guide, it doesn't quit");
	GM.setGameOver(false); // in case it did

	int selected_before = p_title->getSelected();
	sendKeyboardEvent(df::Keyboard::UNDEFINED_KEY, df::KEY_PRESSED);
	sendKeyboardEvent(df::Keyboard::Q, df::UNDEFINED_KEYBOARD_ACTION);
	sendKeyboardEvent(static_cast<df::Keyboard::Key>(999), df::KEY_PRESSED);
	runFrame();
	batch.check(p_title->getSelected() == selected_before && !p_title->isShowingControls() &&
		!p_title->isShowingScores() && !GM.getGameOver() && findTitleScreen() != nullptr,
		"title screen ignores an undefined key, an undefined action and an out-of-range key");
	GM.setGameOver(false); // in case it didn't

	// --- In game: keys and moves that should do nothing ---
	pressKey(df::Keyboard::P);
	runFrame();
	bool map_loaded = waitForMap();
	Hero* p_hero = findHero();
	batch.check(map_loaded && p_hero != nullptr, "the map finishes building", describeMapBuilder());
	if (!map_loaded || p_hero == nullptr) {
		GM.shutDown();
		return batch;
	}

	int moves = p_hero->getMoves();
	df::Vector position = p_hero->getPosition();
	int direction = p_hero->getDirection();
	pressKey(df::Keyboard::Z);
	batch.check(p_hero->getMoves() == moves && samePosition(p_hero->getPosition(), position) && p_hero->getDirection() == direction,
		"owl ignores keys that aren't controls");

	releaseKey(df::Keyboard::W);
	releaseKey(df::Keyboard::D);
	batch.check(p_hero->getMoves() == moves && samePosition(p_hero->getPosition(), position) && p_hero->getDirection() == direction,
		"owl ignores key releases (releasing W or D does nothing)");

	sendKeyboardEvent(df::Keyboard::UNDEFINED_KEY, df::KEY_PRESSED);
	sendKeyboardEvent(df::Keyboard::W, df::UNDEFINED_KEYBOARD_ACTION);
	sendKeyboardEvent(static_cast<df::Keyboard::Key>(999), df::KEY_PRESSED);
	runFrame();
	batch.check(p_hero->getMoves() == moves && samePosition(p_hero->getPosition(), position) && p_hero->getDirection() == direction,
		"owl ignores an undefined key, an undefined action and an out-of-range key");

	df::EventCollision empty_collision; // no objects involved
	int empty_collision_result = p_hero->eventHandler(&empty_collision);
	batch.check(empty_collision_result == 0 && p_hero->getSeeds() == 0 && p_hero->getMaps() == 0 &&
		WM.objectsOfTypeCount("Hero") == 1, "owl ignores a collision event with nothing in it");

	df::Vector off_screen(200, 200);
	placeOwl(p_hero, off_screen, FACING_LEFT);
	pressKey(df::Keyboard::W);
	batch.check(samePosition(p_hero->getPosition(), off_screen) && WM.objectsOfTypeCount("Hero") == 1,
		"an owl placed far outside the window can't hop", "owl at " + describe(p_hero->getPosition()));

	placeOwl(p_hero, LEFT_EDGE_TILE, FACING_LEFT);
	pressKey(df::Keyboard::W);
	batch.check(samePosition(p_hero->getPosition(), LEFT_EDGE_TILE), "owl can't hop off the left edge of the window",
		"owl at " + describe(p_hero->getPosition()));
	batch.check(WM.objectsOfTypeCount("Hero") == 1, "hopping into the window edge doesn't kill the owl");

	ookpik::Seed* p_seed = new ookpik::Seed();
	bool first_collect = p_seed->collect();
	bool second_collect = p_seed->collect();
	batch.check(first_collect && !second_collect, "a seed can only be collected once");
	runFrame(); // collect() marked the seed for deletion

	ookpik::MapExit* p_exit = new ookpik::MapExit();
	bool first_use = p_exit->use();
	bool second_use = p_exit->use();
	batch.check(first_use && !second_use, "an exit can only start one new map");
	WM.markForDelete(p_exit);
	runFrame();

	// --- Death screen: keys pressed too soon are ignored ---
	df::Vector stand;
	int facing = 0;
	df::Object* p_tree = findReachable(p_hero, "Tree", stand, facing);
	batch.check(p_tree != nullptr, "found a tree the owl can fly into");
	if (p_tree != nullptr) {
		placeOwl(p_hero, stand, facing);
		pressKey(df::Keyboard::W); // the owl is deleted at the end of this frame
		p_hero = nullptr;
		pressKey(df::Keyboard::SPACE);
		runFrames(2);
		batch.check(findGameOver() != nullptr && findTitleScreen() == nullptr,
			"death screen ignores keys pressed during its input delay");

		// --- Initials prompt: keys that can't be part of a name ---
		GameOver* p_name_game_over = findGameOver();
		batch.check(p_name_game_over != nullptr && p_name_game_over->isEnteringName(),
			"the run asks for initials (empty table)");
		if (p_name_game_over != nullptr && p_name_game_over->isEnteringName()) {
			runFrames(DEATH_INPUT_DELAY);
			pressKey(df::Keyboard::SPACE);
			pressKey(df::Keyboard::ESCAPE);
			pressKey(df::Keyboard::PERIOD);
			runFrame();
			batch.check(p_name_game_over->isEnteringName() && p_name_game_over->getName().empty() &&
				findGameOver() != nullptr && findTitleScreen() == nullptr,
				"keys that aren't letters or numbers don't type initials or leave the prompt",
				"initials: \"" + p_name_game_over->getName() + "\"");

			typeInitials("AB");
			pressKey(df::Keyboard::RETURN);
			batch.check(p_name_game_over->isEnteringName() && loadHighScores().empty(),
				"enter with fewer than 3 initials doesn't save the run");
		}
	}

	// --- High score table: bad or missing data ---
	clearHighScores();
	batch.check(loadHighScores().empty(), "a missing high score file loads as an empty table");

	writeHighScoreFile(std::string(HIGH_SCORE_HEADER) + "abc\n1;2;3\n4,5\n,,\n\n7,1,12.50\n");
	std::vector<ScoreEntry> loaded = loadHighScores();
	batch.check(loaded.size() == 1 && loaded[0].seeds == 7 && loaded[0].levels == 1,
		"malformed high score rows are skipped", "rows loaded: " + std::to_string(loaded.size()));

	std::string full = fullHighScoreFile();
	writeHighScoreFile(full);
	int rank = submitHighScore(ScoreEntry{ 1, 0, 99.0 });
	batch.check(rank == -1, "a run worse than a full top 10 isn't ranked", "rank: " + std::to_string(rank));
	batch.check(readHighScoreFile() == full, "a run that isn't ranked leaves the saved table unchanged");

	// --- High score table: impossible runs are refused ---
	const double NOT_A_NUMBER = std::numeric_limits<double>::quiet_NaN();
	const double INFINITE_TIME = std::numeric_limits<double>::infinity();
	clearHighScores();
	int negative_seeds_rank = submitHighScore(ScoreEntry{ -4, 1, 10.0 });
	int negative_levels_rank = submitHighScore(ScoreEntry{ 4, -1, 10.0 });
	int negative_time_rank = submitHighScore(ScoreEntry{ 4, 1, -10.0 });
	int nan_time_rank = submitHighScore(ScoreEntry{ 4, 1, NOT_A_NUMBER });
	int infinite_time_rank = submitHighScore(ScoreEntry{ 4, 1, INFINITE_TIME });
	batch.check(negative_seeds_rank == -1, "a run with negative seeds is refused", "rank: " + std::to_string(negative_seeds_rank));
	batch.check(negative_levels_rank == -1, "a run with negative levels is refused", "rank: " + std::to_string(negative_levels_rank));
	batch.check(negative_time_rank == -1, "a run with a negative time is refused", "rank: " + std::to_string(negative_time_rank));
	batch.check(nan_time_rank == -1, "a run with a time that isn't a number is refused", "rank: " + std::to_string(nan_time_rank));
	batch.check(infinite_time_rank == -1, "a run with an infinite time is refused", "rank: " + std::to_string(infinite_time_rank));
	batch.check(loadHighScores().empty(), "refused runs aren't saved",
		"rows saved: " + std::to_string(loadHighScores().size()));

	// Names that aren't 3 capital letters or digits are saved as "---" (a comma would also break the file)
	clearHighScores();
	submitHighScore(ScoreEntry{ 3, 0, 5.0, "A,B" });
	submitHighScore(ScoreEntry{ 2, 0, 5.0, "ABCD" });
	submitHighScore(ScoreEntry{ 1, 0, 5.0, "ab1" });
	std::vector<ScoreEntry> named = loadHighScores();
	batch.check(named.size() == 3 && named[0].name == NO_HIGH_SCORE_NAME && named[1].name == NO_HIGH_SCORE_NAME &&
		named[2].name == NO_HIGH_SCORE_NAME,
		"names with a comma, the wrong length or lower case are saved as ---",
		"rows: " + std::to_string(named.size()) + (named.empty() ? std::string() : " first name: " + named[0].name));

	writeHighScoreFile(std::string(HIGH_SCORE_HEADER) +
		"-3,1,2.00\n2,-1,2.00\n2,1,-2.00\n2,1,nan\n2,1,inf\n4,1,3.00\n");
	loaded = loadHighScores();
	batch.check(loaded.size() == 1 && loaded[0].seeds == 4,
		"saved rows with negative or non-number values are skipped", "rows loaded: " + std::to_string(loaded.size()));

	// --- Death screen: impossible totals ---
	clearHighScores();
	GameOver* p_bad_game_over = new GameOver(-5, -2, -1, -3.0);
	batch.check(p_bad_game_over->getRank() == -1 && !p_bad_game_over->isEnteringName() && loadHighScores().empty(),
		"a death screen given negative totals doesn't record a high score",
		"rank: " + std::to_string(p_bad_game_over->getRank()) + " rows saved: " + std::to_string(loadHighScores().size()));
	WM.markForDelete(p_bad_game_over);
	runFrame();

	// --- High score table drawing: rows and highlights out of range ---
	writeHighScoreFile(fullHighScoreFile());
	drawHighScoreTable(8, 50);
	drawHighScoreTable(8, -7);
	drawHighScoreTable(-20, -1);
	drawHighScoreTable(500, -1);
	batch.check(true, "the high score table draws with an out-of-range highlight or start row without crashing");

	// --- Exiting: bad calls during and after shutdown ---
	// Regression: ResourceManager::shutDown() used to loop over sounds with the music count,
	// so more music than sounds read past the end of the sound list. Loaded only, never played.
	bool audio_loaded = RM.loadSound("Resources/Sounds/move.wav", "shutdown test sound") == 0 &&
		RM.loadMusic("Resources/Sounds/outside.wav", "shutdown test music 1") == 0 &&
		RM.loadMusic("Resources/Sounds/outside.wav", "shutdown test music 2") == 0;
	batch.check(audio_loaded, "one sound and two music tracks load for the shutdown test");

	batch.check(WM.markForDelete(nullptr) == -1, "deleting a null object is refused");
	GM.shutDown();
	GM.shutDown();
	batch.check(!GM.isStarted() && !WM.isStarted() && !df::DisplayManager::getInstance().isStarted() &&
		!df::EventManager::getInstance().isStarted() && !RM.isStarted() && !df::LogManager::getInstance().isStarted(),
		"shutting down twice (with more music than sounds loaded) is harmless");
	return batch;
}

TestBatch runBatchEdgeCases() {
	TestBatch batch{ "Edge Cases" };

	bool started = startGame(true);
	batch.check(started, "engine starts and the game sprites load");
	if (!started) {
		GM.shutDown();
		return batch;
	}
	clearHighScores();

	// Regression: Vector(x, y) used to leave its comparison tolerance unset
	batch.check(df::Vector(28, 6) == df::Vector(28, 6) && !(df::Vector(28, 6) != df::Vector(28, 6)),
		"positions made from the same coordinates compare equal");

	// --- Map generation: what the generator actually builds ---
	df::Object* p_test_owl = new df::Object(); // stands in for the owl; the builder only moves it

	// The same fixed seed must build the same map
	ookpik::MapGenConfig seeded_config = makeMapConfig();
	seeded_config.setRandomSeed(424242);
	bool first_seeded = buildTestMap(seeded_config, p_test_owl);
	std::vector<std::string> first_layout = mapLayout(p_test_owl);
	bool second_seeded = buildTestMap(seeded_config, p_test_owl);
	std::vector<std::string> second_layout = mapLayout(p_test_owl);
	batch.check(first_seeded && second_seeded && first_layout == second_layout,
		"the same fixed random seed builds the same map",
		"objects in first map: " + std::to_string(first_layout.size()) + " second: " + std::to_string(second_layout.size()));

	// Random maps: everything must be inside the window, on its own tile, and reachable
	const int RANDOM_MAPS = 5;
	int maps_built = 0;
	int maps_with_one_exit = 0;
	int maps_with_seeds_in_range = 0;
	int maps_inside_window = 0;
	int maps_without_shared_tiles = 0;
	int maps_fully_reachable = 0;
	std::string reach_detail;
	for (int map = 0; map < RANDOM_MAPS; map++) {
		if (!buildTestMap(makeMapConfig(), p_test_owl)) {
			continue;
		}
		maps_built++;
		int seeds = WM.objectsOfTypeCount("Seed");
		maps_with_one_exit += WM.objectsOfTypeCount("mapExit") == 1 ? 1 : 0;
		maps_with_seeds_in_range += (seeds >= MIN_SEEDS && seeds <= MAX_SEEDS) ? 1 : 0;
		maps_inside_window += outsideWindowCount() == 0 ? 1 : 0;
		maps_without_shared_tiles += sharedTileCount() == 0 ? 1 : 0;
		bool exit_reached = false;
		int unreachable = unreachableSeeds(p_test_owl->getPosition(), exit_reached);
		if (unreachable == 0 && exit_reached) {
			maps_fully_reachable++;
		}
		else {
			reach_detail += " map " + std::to_string(map) + ": " + std::to_string(unreachable) + " of " +
				std::to_string(seeds) + " seeds unreachable, exit " + (exit_reached ? "reachable" : "unreachable") + ";";
		}
	}
	std::string out_of = " (" + std::to_string(RANDOM_MAPS) + " random maps)";
	batch.check(maps_built == RANDOM_MAPS, "every random map finishes building",
		std::to_string(maps_built) + " of " + std::to_string(RANDOM_MAPS) + " built");
	batch.check(maps_with_one_exit == maps_built, "every map has exactly one exit",
		std::to_string(maps_with_one_exit) + " of " + std::to_string(maps_built) + out_of);
	batch.check(maps_with_seeds_in_range == maps_built, "every map has between 15 and 30 seeds",
		std::to_string(maps_with_seeds_in_range) + " of " + std::to_string(maps_built) + out_of);
	batch.check(maps_inside_window == maps_built, "every tree, seed and exit is inside the window",
		std::to_string(maps_inside_window) + " of " + std::to_string(maps_built) + out_of);
	batch.check(maps_without_shared_tiles == maps_built, "no two trees, seeds or exits share a tile",
		std::to_string(maps_without_shared_tiles) + " of " + std::to_string(maps_built) + out_of);
	batch.check(maps_fully_reachable == maps_built, "the owl can reach every seed and the exit from its start",
		std::to_string(maps_fully_reachable) + " of " + std::to_string(maps_built) + out_of + reach_detail);

	// Speed: with a full map built, one game loop pass must fit in the frame time (33 ms).
	// Regression: drawing each character with its own draw calls used to take ~50 ms (Release)
	// to ~250 ms (Debug) per frame. --frameprofile shows where the time goes.
	FrameTimes frame_times = timeFrames(30);
	char frame_detail[160];
	std::snprintf(frame_detail, sizeof(frame_detail),
		"average ms per frame: step %.2f, update %.2f, draw %.2f, show %.2f, total %.2f (budget %d), objects: %d",
		frame_times.step, frame_times.update, frame_times.draw, frame_times.show, frame_times.total(),
		GM.getFrameTime(), WM.AllObjectsCount());
	batch.check(frame_times.total() < GM.getFrameTime(), "a frame with a full map fits in the frame time", frame_detail);

	// Range ends: min == max must give exactly that many seeds
	ookpik::MapGenConfig exact_config = makeMapConfig();
	exact_config.setMinSeeds(20);
	exact_config.setMaxSeeds(20);
	bool exact_built = buildTestMap(exact_config, p_test_owl);
	batch.check(exact_built && WM.objectsOfTypeCount("Seed") == 20, "min seeds equal to max seeds gives exactly that many",
		"seeds: " + std::to_string(WM.objectsOfTypeCount("Seed")));

	// The smallest map the generator allows: a 6x4 play area, 1-tile border, one 2x2 room
	// (it needs at least one room or line to carve open space), 1 seed, no extra trees
	ookpik::MapGenConfig tiny_config = makeMapConfig();
	tiny_config.setMapWidth(6);
	tiny_config.setMapHeight(4);
	tiny_config.setMapBorderThickness(1);
	tiny_config.setTimeoutSeconds(5);
	tiny_config.setMinRooms(0);
	tiny_config.setMaxRooms(0);
	tiny_config.setMinRoomWidth(1);
	tiny_config.setMaxRoomWidth(1);
	tiny_config.setMinRoomHeight(1);
	tiny_config.setMaxRoomHeight(1);
	tiny_config.setMinRightAngleLines(0);
	tiny_config.setMaxRightAngleLine(0);
	tiny_config.setMinRightAngleLineWidth(1);
	tiny_config.setMaxRightAngleLineWidth(1);
	tiny_config.setMinRightAngleLineHeight(1);
	tiny_config.setMaxRightAngleLineHeight(1);
	tiny_config.setMinDiagLines(0);
	tiny_config.setMaxDiagLine(0);
	tiny_config.setMinDiagLineWidth(1);
	tiny_config.setMaxDiagLineWidth(1);
	tiny_config.setMinDiagLineHeight(1);
	tiny_config.setMaxDiagLineHeight(1);
	tiny_config.setMinRandTrees(0);
	tiny_config.setMaxRandTrees(0);
	tiny_config.setMinSeeds(1);
	tiny_config.setMaxSeeds(1);

	// With no rooms or lines at all there's no open space to put anything in, so it's refused
	batch.check(mapConfigIsRejected(tiny_config), "a map with no rooms or lines (no open space) is refused");

	tiny_config.setMinRooms(1);
	tiny_config.setMaxRooms(1);
	tiny_config.setMinRoomWidth(2);
	tiny_config.setMaxRoomWidth(2);
	tiny_config.setMinRoomHeight(2);
	tiny_config.setMaxRoomHeight(2);
	bool tiny_built = buildTestMap(tiny_config, p_test_owl);
	bool tiny_exit_reached = false;
	int tiny_unreachable = tiny_built ? unreachableSeeds(p_test_owl->getPosition(), tiny_exit_reached) : -1;
	batch.check(tiny_built && WM.objectsOfTypeCount("Seed") == 1 && WM.objectsOfTypeCount("mapExit") == 1 &&
		tiny_unreachable == 0 && tiny_exit_reached,
		"the smallest allowed map builds with its seed and exit reachable", describeMapBuilder() +
		", seeds: " + std::to_string(WM.objectsOfTypeCount("Seed")) + ", exits: " + std::to_string(WM.objectsOfTypeCount("mapExit")));

	// Regression: these fixed seeds used to put the exit on the only way into part of the map,
	// leaving seeds behind it that could never be collected (found with --mapscan)
	const int EXIT_REGRESSION_SEEDS[] = { 6, 15, 54 };
	for (int regression_seed : EXIT_REGRESSION_SEEDS) {
		ookpik::MapGenConfig regression_config = makeMapConfig();
		regression_config.setRandomSeed(regression_seed);
		bool regression_built = buildTestMap(regression_config, p_test_owl);
		bool regression_exit_reached = false;
		int regression_unreachable = regression_built ?
			unreachableSeeds(p_test_owl->getPosition(), regression_exit_reached) : -1;
		std::string name = "map seed " + std::to_string(regression_seed) + " leaves every seed and the exit reachable";
		batch.check(regression_built && regression_unreachable == 0 && regression_exit_reached, name.c_str(),
			std::to_string(regression_unreachable) + " seeds unreachable, exit " + (regression_exit_reached ? "reachable" : "unreachable"));
	}

	// Starting a second map in the same frame as the first: only the second one is built
	clearMap();
	runFrame();
	startNewMap(p_test_owl);
	startNewMap(p_test_owl); // clears the first builder while its thread is still generating
	bool double_start_built = waitForMap();
	batch.check(double_start_built && WM.objectsOfTypeCount("mapBuilder") == 1 && WM.objectsOfTypeCount("mapExit") == 1,
		"starting a new map while one is generating leaves exactly one map",
		"builders: " + std::to_string(WM.objectsOfTypeCount("mapBuilder")) + " exits: " + std::to_string(WM.objectsOfTypeCount("mapExit")));

	clearMap();
	WM.markForDelete(p_test_owl);
	runFrame();

	// --- Title screen ---
	new TitleScreen();
	runFrame();
	TitleScreen* p_title = findTitleScreen();
	batch.check(p_title != nullptr, "title screen appears");
	if (p_title == nullptr) {
		GM.shutDown();
		return batch;
	}

	pressKey(df::Keyboard::W);
	batch.check(p_title->getSelected() == MENU_QUIT, "W on the top option wraps round to Quit");
	pressKey(df::Keyboard::S);
	batch.check(p_title->getSelected() == MENU_PLAY, "S on the bottom option wraps round to Play");
	pressKey(df::Keyboard::DOWNARROW);
	pressKey(df::Keyboard::UPARROW);
	batch.check(p_title->getSelected() == MENU_PLAY, "arrow keys move the highlight like W and S");

	// Two menu keys arriving in the same frame: only the last one is carried out
	sendKeyboardEvent(df::Keyboard::P, df::KEY_PRESSED);
	sendKeyboardEvent(df::Keyboard::C, df::KEY_PRESSED);
	runFrame();
	batch.check(p_title->isShowingControls() && WM.objectsOfTypeCount("Hero") == 0,
		"two menu keys in one frame: only the last one is carried out");
	pressKey(df::Keyboard::SPACE); // close the controls guide

	pressKey(df::Keyboard::Q);
	bool quit_same_frame = GM.getGameOver();
	runFrame();
	bool quit_next_frame = GM.getGameOver();
	batch.check(!quit_same_frame && quit_next_frame,
		"menu choices are carried out on the frame after the key press");
	GM.setGameOver(false);

	pressKey(df::Keyboard::S); // Quit wraps round to Play
	pressKey(df::Keyboard::SPACE);
	runFrame();
	batch.check(findTitleScreen() == nullptr && WM.objectsOfTypeCount("Hero") == 1,
		"space selects the highlighted option (Play)");

	bool map_loaded = waitForMap();
	Hero* p_hero = findHero();
	batch.check(map_loaded && p_hero != nullptr, "the map finishes building", describeMapBuilder());
	if (!map_loaded || p_hero == nullptr) {
		GM.shutDown();
		return batch;
	}

	// --- Turning wraps round ---
	pressKey(df::Keyboard::A); // right -> up
	pressKey(df::Keyboard::A); // up -> left
	batch.check(p_hero->getDirection() == 3, "turning left from up wraps round to facing left");
	pressKey(df::Keyboard::D);
	batch.check(p_hero->getDirection() == 0, "turning right from left wraps round to facing up");

	// --- A hop over a seed sends two collision events, but the seed counts once ---
	df::Vector stand;
	int facing = 0;
	df::Object* p_seed = findReachable(p_hero, "Seed", stand, facing);
	batch.check(p_seed != nullptr, "found a seed the owl can hop onto");
	if (p_seed != nullptr) {
		int seeds_left = WM.objectsOfTypeCount("Seed");
		placeOwl(p_hero, stand, facing);
		pressKey(df::Keyboard::W);
		batch.check(p_hero->getSeeds() == 1, "one hop onto a seed counts it once",
			"seeds collected: " + std::to_string(p_hero->getSeeds()));
		batch.check(statusShows(p_hero, 1, seeds_left - 1, p_hero->getMoves(), 0),
			"status line shows the new seed totals on the same frame",
			statusDetail(p_hero, 1, seeds_left - 1, p_hero->getMoves(), 0));
	}

	// --- Two hops in one frame straight over a seed: it's collected once and the owl lands past it ---
	df::Object* p_hop_seed = findSeedToHopOver(p_hero, stand, facing);
	batch.check(p_hop_seed != nullptr, "found a seed with free tiles before and after it");
	if (p_hop_seed != nullptr) {
		df::Vector seed_position = p_hop_seed->getPosition();
		df::Vector beyond(seed_position.getX() + HOP_X[facing], seed_position.getY() + HOP_Y[facing]);
		int seeds_before = p_hero->getSeeds();
		placeOwl(p_hero, stand, facing);
		int moves_before = p_hero->getMoves();
		runFrame(df::Keyboard::W, df::KEY_PRESSED, 2);
		sendKeyboardEvent(df::Keyboard::W, df::KEY_RELEASED); // let go, so W isn't left held
		batch.check(p_hero->getSeeds() == seeds_before + 1 && samePosition(p_hero->getPosition(), beyond) &&
			p_hero->getMoves() == moves_before + 2,
			"two hops in one frame over a seed collect it once and land past it",
			"seeds: " + std::to_string(seeds_before) + " -> " + std::to_string(p_hero->getSeeds()) +
			", owl at " + describe(p_hero->getPosition()) + " expected " + describe(beyond));
	}

	// --- The other window edges block hops too (the left edge is in the Errors batch) ---
	const df::Vector EDGE_TILES[] = { df::Vector(57, 0), df::Vector(114, 15), df::Vector(57, 29) };
	const int EDGE_FACINGS[] = { 0, 1, 2 }; // up, right, down: straight out of the window
	const char* EDGE_NAMES[] = { "top", "right", "bottom" };
	for (int edge = 0; edge < 3; edge++) {
		placeOwl(p_hero, EDGE_TILES[edge], EDGE_FACINGS[edge]);
		pressKey(df::Keyboard::W);
		std::string name = std::string("owl can't hop off the ") + EDGE_NAMES[edge] + " edge of the window";
		batch.check(samePosition(p_hero->getPosition(), EDGE_TILES[edge]) && WM.objectsOfTypeCount("Hero") == 1,
			name.c_str(), "owl at " + describe(p_hero->getPosition()));
	}

	// --- Collecting the last seed on the map ---
	df::Object* p_last_seed = findReachable(p_hero, "Seed", stand, facing);
	batch.check(p_last_seed != nullptr, "found a seed to leave as the last one");
	if (p_last_seed != nullptr) {
		df::ObjectList all_seeds = WM.objectsOfType("Seed");
		for (int i = 0; i < all_seeds.getCount(); i++) {
			if (all_seeds[i] != p_last_seed) {
				WM.markForDelete(all_seeds[i]);
			}
		}
		runFrame(); // every other seed is removed here
		int seeds_before = p_hero->getSeeds();
		placeOwl(p_hero, stand, facing);
		pressKey(df::Keyboard::W);
		batch.check(WM.objectsOfTypeCount("Seed") == 0 && statusShows(p_hero, seeds_before + 1, 0, p_hero->getMoves(), 0),
			"collecting the last seed shows 0 remaining", statusDetail(p_hero, seeds_before + 1, 0, p_hero->getMoves(), 0));
	}

	// --- Only real moves count: not a hop the window edge blocks, and not quitting ---
	placeOwl(p_hero, LEFT_EDGE_TILE, FACING_LEFT);
	int moves_before_blocked = p_hero->getMoves();
	pressKey(df::Keyboard::W);
	batch.check(samePosition(p_hero->getPosition(), LEFT_EDGE_TILE) && p_hero->getMoves() == moves_before_blocked,
		"a hop blocked by the window edge doesn't count as a move",
		"moves before: " + std::to_string(moves_before_blocked) + " after: " + std::to_string(p_hero->getMoves()));

	holdKey(df::Keyboard::W);
	runFrames(HOLD_REPEAT_STEPS * 2);
	batch.check(samePosition(p_hero->getPosition(), LEFT_EDGE_TILE) && p_hero->getMoves() == moves_before_blocked,
		"holding W against the window edge doesn't move or count moves",
		"moves before: " + std::to_string(moves_before_blocked) + " after: " + std::to_string(p_hero->getMoves()));
	releaseKey(df::Keyboard::W);

	int moves_before_escape = p_hero->getMoves();
	pressKey(df::Keyboard::ESCAPE);
	batch.check(GM.getGameOver() && p_hero->getMoves() == moves_before_escape, "escape doesn't count as a move",
		"moves before: " + std::to_string(moves_before_escape) + " after: " + std::to_string(p_hero->getMoves()));
	GM.setGameOver(false); // keep testing

	// --- Level Time restarts on a new map ---
	runFrames(30); // let the level clock run for about a second
	double time_before_exit = levelTimeShown(p_hero);
	df::Object* p_exit = findReachable(p_hero, "mapExit", stand, facing);
	batch.check(p_exit != nullptr, "found the exit and a free tile next to it");
	if (p_exit != nullptr) {
		placeOwl(p_hero, stand, facing);
		holdKey(df::Keyboard::W); // still held when the owl reaches the exit
		bool next_loaded = waitForMap();
		batch.check(next_loaded, "the next map finishes building", describeMapBuilder());
		runFrame();
		double time_after_exit = levelTimeShown(p_hero);
		batch.check(next_loaded && time_after_exit < time_before_exit, "Level Time restarts when a new map loads",
			"level time before exit: " + std::to_string(time_before_exit) +
			" after new map: " + std::to_string(time_after_exit));

		// Holding W ends at the exit: the owl waits on the new map until W is pressed again
		df::Vector new_map_start = p_hero->getPosition();
		int moves_on_new_map = p_hero->getMoves();
		runFrames(HOLD_REPEAT_STEPS * 2);
		batch.check(!p_hero->isForwardHeld() && samePosition(p_hero->getPosition(), new_map_start) &&
			p_hero->getMoves() == moves_on_new_map,
			"holding W into the exit stops on the new map until W is pressed again",
			"owl at " + describe(p_hero->getPosition()) + " started at " + describe(new_map_start));
		releaseKey(df::Keyboard::W);
	}

	// --- The death screen's move count includes the fatal hop ---
	df::Object* p_tree = findReachable(p_hero, "Tree", stand, facing);
	batch.check(p_tree != nullptr, "found a tree the owl can fly into");
	if (p_tree == nullptr) {
		GM.shutDown();
		return batch;
	}
	placeOwl(p_hero, stand, facing);
	int moves_before_fatal = p_hero->getMoves();
	pressKey(df::Keyboard::W); // the owl is deleted at the end of this frame
	p_hero = nullptr;
	GameOver* p_game_over = findGameOver();
	batch.check(p_game_over != nullptr && p_game_over->getMoves() == moves_before_fatal + 1,
		"death screen move count includes the fatal hop",
		"moves before the fatal hop: " + std::to_string(moves_before_fatal) + " death screen: " +
		std::to_string(p_game_over != nullptr ? p_game_over->getMoves() : -1));

	// --- Initials: digits from both number rows, and limits on what can be typed ---
	if (p_game_over != nullptr && p_game_over->isEnteringName()) {
		runFrames(DEATH_INPUT_DELAY);
		pressKey(df::Keyboard::BACKSPACE); // nothing to remove yet
		pressKey(df::Keyboard::NUM7);
		pressKey(df::Keyboard::NUMPAD4);
		typeInitials("Z");
		typeInitials("Q"); // a 4th character
		batch.check(p_game_over->getName() == "74Z",
			"initials take digits from either number row, and a 4th character is ignored",
			"initials: " + p_game_over->getName());
		pressKey(df::Keyboard::RETURN);
		std::vector<ScoreEntry> digit_scores = loadHighScores();
		batch.check(!digit_scores.empty() && digit_scores[0].name == "74Z", "initials made of digits are saved",
			"saved name: " + (digit_scores.empty() ? std::string("none") : digit_scores[0].name));
	}

	// --- A second run starts from zero ---
	leaveDeathScreen();
	pressKey(df::Keyboard::P);
	runFrame();
	bool replay_loaded = waitForMap();
	p_hero = findHero();
	batch.check(replay_loaded, "the new run's map finishes building", describeMapBuilder());
	batch.check(WM.objectsOfTypeCount("Hero") == 1, "a new run after dying has exactly one owl",
		"owls: " + std::to_string(WM.objectsOfTypeCount("Hero")));
	if (p_hero != nullptr) {
		batch.check(p_hero->getMoves() == 0 && p_hero->getSeeds() == 0 && p_hero->getMaps() == 0,
			"a new run after dying starts with 0 moves, seeds and maps",
			"moves: " + std::to_string(p_hero->getMoves()) + " seeds: " + std::to_string(p_hero->getSeeds()) +
			" maps: " + std::to_string(p_hero->getMaps()));
	}

	// --- Two hops into a tree in the same frame: the owl only dies once ---
	// Also a run with nothing collected: it's still a valid run, so it's recorded
	clearHighScores();
	if (p_hero != nullptr) {
		p_tree = findReachable(p_hero, "Tree", stand, facing);
		batch.check(p_tree != nullptr, "found a tree the owl can fly into");
		if (p_tree != nullptr) {
			placeOwl(p_hero, stand, facing);
			runFrame(df::Keyboard::W, df::KEY_PRESSED, 2); // the owl is deleted at the end of this frame
			p_hero = nullptr;
			batch.check(WM.objectsOfTypeCount("GameOver") == 1, "two hops into a tree in one frame give one death screen",
				"death screens: " + std::to_string(WM.objectsOfTypeCount("GameOver")));
			// Every death screen gets the same initials, so a second one would save a second row
			runFrames(DEATH_INPUT_DELAY);
			typeInitials("ZZZ");
			pressKey(df::Keyboard::RETURN);
			std::vector<ScoreEntry> death_scores = loadHighScores();
			batch.check(death_scores.size() == 1, "two hops into a tree in one frame record one high score",
				"rows saved: " + std::to_string(death_scores.size()));
			batch.check(!death_scores.empty() && death_scores[0].seeds == 0 && death_scores[0].levels == 0,
				"a run with nothing collected is still recorded");
		}
	}

	// --- High score ranking ---
	clearHighScores();
	submitHighScore(ScoreEntry{ 5, 1, 20.0 });
	int equal_rank = submitHighScore(ScoreEntry{ 5, 1, 20.0 });
	batch.check(equal_rank == 1, "a run equal to a saved run is placed below it", "rank: " + std::to_string(equal_rank));
	int levels_rank = submitHighScore(ScoreEntry{ 5, 2, 50.0 });
	batch.check(levels_rank == 0, "with equal seeds, more levels ranks higher", "rank: " + std::to_string(levels_rank));
	int faster_rank = submitHighScore(ScoreEntry{ 5, 1, 10.0 });
	batch.check(faster_rank == 1, "with equal seeds and levels, the faster run ranks higher",
		"rank: " + std::to_string(faster_rank));

	clearHighScores();
	submitHighScore(ScoreEntry{ 1, 0, 12.3456 });
	std::vector<ScoreEntry> scores = loadHighScores();
	batch.check(scores.size() == 1 && std::fabs(scores[0].time - 12.35) < 0.0001,
		"run times are saved rounded to two decimal places",
		"saved time: " + (scores.empty() ? std::string("none") : std::to_string(scores[0].time)));

	writeHighScoreFile(fullHighScoreFile());
	int tied_last_rank = submitHighScore(ScoreEntry{ 11, 0, 1.0 });
	batch.check(tied_last_rank == -1, "a run tied with 10th place doesn't make a full table",
		"rank: " + std::to_string(tied_last_rank));

	int best_rank = submitHighScore(ScoreEntry{ 25, 0, 1.0 });
	scores = loadHighScores();
	batch.check(best_rank == 0 && (int)scores.size() == MAX_HIGH_SCORES && scores.back().seeds == 12,
		"a new best run on a full table drops the 10th place run",
		"rank: " + std::to_string(best_rank) + " rows: " + std::to_string(scores.size()) +
		" last seeds: " + (scores.empty() ? std::string("none") : std::to_string(scores.back().seeds)));

	int last_place_rank = submitHighScore(ScoreEntry{ 12, 0, 0.5 });
	batch.check(last_place_rank == MAX_HIGH_SCORES - 1, "a run that only beats 10th place takes 10th place",
		"rank: " + std::to_string(last_place_rank));

	writeHighScoreFile(fullHighScoreFile() + "5,0,1.00\n4,0,1.00\n");
	batch.check((int)loadHighScores().size() == MAX_HIGH_SCORES, "only the top 10 rows of a longer file are loaded",
		"rows loaded: " + std::to_string(loadHighScores().size()));

	// --- High score names and older files ---
	writeHighScoreFile(std::string(OLD_HIGH_SCORE_HEADER) + "9,2,30.00\r\n8,1,20.00\n");
	std::vector<ScoreEntry> old_rows = loadHighScores();
	batch.check(old_rows.size() == 2 && old_rows[0].name == NO_HIGH_SCORE_NAME && old_rows[1].seeds == 8,
		"a table saved before names existed loads with --- names, including Windows line endings",
		"rows loaded: " + std::to_string(old_rows.size()));

	writeHighScoreFile(std::string(HIGH_SCORE_HEADER) + "ABC,9,2,30.00\n7,1,20.00\nab,5,0,1.00\n");
	std::vector<ScoreEntry> mixed_rows = loadHighScores();
	batch.check(mixed_rows.size() == 3 && mixed_rows[0].name == "ABC" && mixed_rows[1].name == NO_HIGH_SCORE_NAME &&
		mixed_rows[2].name == NO_HIGH_SCORE_NAME,
		"a table mixing named and unnamed rows loads every row, and a bad saved name shows as ---",
		"rows loaded: " + std::to_string(mixed_rows.size()));

	writeHighScoreFile(fullHighScoreFile());
	GameOver* p_low_game_over = new GameOver(0, 0, 0, 99.0);
	batch.check(p_low_game_over->getRank() == -1 && !p_low_game_over->isEnteringName(),
		"a run that doesn't make the top 10 isn't asked for initials");
	WM.markForDelete(p_low_game_over);
	runFrame();

	// --- Exiting: every object is deleted ---
	// Empty the world first, so the probes are the only objects left when the engine shuts down
	df::ObjectList everything = WM.getAllObjects();
	for (int i = 0; i < everything.getCount(); i++) {
		WM.markForDelete(everything[i]);
	}
	runFrame();
	const int PROBES = 4;
	probes_destroyed = 0;
	for (int i = 0; i < PROBES; i++) {
		new ShutdownProbe();
	}
	GM.shutDown();
	batch.check(probes_destroyed == PROBES, "shutting down deletes every object left in the world",
		"deleted " + std::to_string(probes_destroyed) + " of " + std::to_string(PROBES));

	return batch;
}

}  // namespace

#ifdef _DEBUG
int runMapScan(int first_seed, int last_seed) {
	if (!startGame(false)) {
		GM.shutDown();
		return -1;
	}
	df::Object* p_owl = new df::Object(); // stands in for the owl; the builder only moves it
	int bad_maps = 0;
	for (int seed = first_seed; seed <= last_seed; seed++) {
		clearMap();
		WM.update();
		ookpik::MapGenConfig config = makeMapConfig();
		config.setRandomSeed(seed);
		ookpik::MapBuilder* p_builder = new ookpik::MapBuilder();
		p_builder->startGenerateMap(config, p_owl);
		df::Clock wait_clock;
		bool finished = false;
		while (!finished && wait_clock.split() < 10000000LL) {
			df::EventStep step(step_count++);
			GM.onEvent(&step);
			WM.update();
			finished = p_builder->isMapBuildFinished();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		if (!finished) {
			std::printf("seed %d: map never finished\n", seed);
			continue;
		}

		bool exit_reached = false;
		std::set<std::pair<int, int>> reached;
		int unreachable = unreachableSeeds(p_owl->getPosition(), exit_reached, &reached);
		if (unreachable == 0 && exit_reached) {
			continue;
		}
		bad_maps++;
		std::printf("seed %d: %d of %d seeds unreachable, exit %s\n", seed, unreachable,
			WM.objectsOfTypeCount("Seed"), exit_reached ? "reachable" : "unreachable");
		if (bad_maps > 1) {
			continue; // only the first bad map is drawn
		}

		// One character per tile: # tree, @ reachable seed, ! unreachable seed, E exit, O owl,
		// space for open tiles the owl can reach, - for open tiles it can't
		std::set<std::pair<int, int>> trees, seeds, exits;
		const char* types[] = { "Tree", "Seed", "mapExit" };
		std::set<std::pair<int, int>>* sets[] = { &trees, &seeds, &exits };
		for (int t = 0; t < 3; t++) {
			df::ObjectList objects = WM.objectsOfType(types[t]);
			for (int i = 0; i < objects.getCount(); i++) {
				sets[t]->insert(tileOf(objects[i]->getPosition()));
			}
		}
		std::pair<int, int> owl = tileOf(p_owl->getPosition());
		for (int y = 0; y < 30; y++) {
			std::string line;
			for (int x = 0; x < 115; x += 2) {
				std::pair<int, int> tile = std::make_pair(x, y);
				if (tile == owl) line += 'O';
				else if (trees.count(tile)) line += '#';
				else if (exits.count(tile)) line += 'E';
				else if (seeds.count(tile)) line += reached.count(tile) ? '@' : '!';
				else line += reached.count(tile) ? ' ' : '-';
			}
			std::printf("%2d |%s|\n", y, line.c_str());
		}
	}
	GM.shutDown();
	std::printf("map scan: %d of %d seeds built a map with something unreachable\n", bad_maps, last_seed - first_seed + 1);
	return bad_maps;
}

int runMapStress(int maps) {
	if (!startGame(false)) {
		GM.shutDown();
		return -1;
	}
	df::Object* p_owl = new df::Object(); // stands in for the owl; the builder only moves it
	int stalled = 0;
	for (int map = 0; map < maps; map++) {
		startNewMap(p_owl);
		bool finished = false;
		df::Clock wait_clock;
		while (!finished && wait_clock.split() < 10000000LL) {
			df::EventStep step(step_count++);
			GM.onEvent(&step);
			WM.update();
			ookpik::MapBuilder* p_builder = static_cast<ookpik::MapBuilder*>(findNewest("mapBuilder"));
			finished = p_builder != nullptr && p_builder->isMapBuildFinished();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		if (!finished) {
			stalled++;
			std::printf("map %d never finished: %s\n", map, describeMapBuilder().c_str());
		}
	}
	GM.shutDown();
	std::printf("map stress: %d of %d maps never finished\n", stalled, maps);
	return stalled;
}

int runFrameProfile(int frames) {
	if (!startGame(false)) {
		GM.shutDown();
		return -1;
	}
	df::Object* p_owl = new df::Object(); // stands in for the owl; the builder only moves it
	ookpik::MapGenConfig config = makeMapConfig();
	config.setRandomSeed(6); // the same map every run, so runs can be compared
	if (!buildTestMap(config, p_owl)) {
		std::printf("frame profile: the map didn't build\n");
		GM.shutDown();
		return -1;
	}
	std::printf("objects in world: %d (trees %d, seeds %d)\n", WM.AllObjectsCount(),
		WM.objectsOfTypeCount("Tree"), WM.objectsOfTypeCount("Seed"));
	FrameTimes times = timeFrames(frames);
	std::printf("average per frame over %d frames (ms): step %.2f, update %.2f, draw %.2f, show %.2f, total %.2f (budget %d)\n",
		frames, times.step, times.update, times.draw, times.show, times.total(), GM.getFrameTime());
	GM.shutDown();
	return 0;
}
#endif

int runGameTests() {
	// Remove the last run's totals first, so a run that crashes leaves none instead of stale ones
	std::remove(GAME_TEST_RESULTS_FILE);
	backUpHighScores();

	TestBatch batches[] = { runBatchNormal(), runBatchError(), runBatchEdgeCases() };

	restoreHighScores();

	// Every batch shuts the engine down, so the log is opened again for the summary
	df::LogManager& logkeeper = df::LogManager::getInstance();
	logkeeper.startUp(true);
	logkeeper.setFlush(true);

	int total_pass = 0;
	int total_fail = 0;
	for (const TestBatch& batch : batches) {
		total_pass += batch.pass_count;
		total_fail += batch.logSummary();
	}
	logkeeper.writeLog("game tests: %d passed, %d failed", total_pass, total_fail);
	logkeeper.shutDown();

	std::ofstream results(GAME_TEST_RESULTS_FILE, std::ios::trunc);
	results << (total_pass + total_fail) << " " << total_fail << "\n";

	std::printf("testing complete! %d passed, %d failed\ntesting log saved to dragonfly.log\n", total_pass, total_fail);
	return total_fail;
}
