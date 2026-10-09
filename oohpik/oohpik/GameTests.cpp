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

#include <chrono>
#include <climits>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <limits>
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
const char* HIGH_SCORE_HEADER = "seeds,levels,time\n";

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
// would send it: after the step event and before the world updates.
void runFrame(df::Keyboard::Key key = df::Keyboard::UNDEFINED_KEY, df::EventKeyboardAction action = df::KEY_PRESSED) {
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
		GM.onEvent(&keyboard);
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

void pressKey(df::Keyboard::Key key) {
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
// Needs a world with no map in it, so map objects can be counted.
bool mapConfigIsRejected(const ookpik::MapGenConfig& config) {
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
		contents += std::to_string(seeds) + ",0,1.00\n";
	}
	return contents;
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
	batch.check(p_game_over->getRank() == 0, "the run is ranked first in an empty high score table",
		"rank: " + std::to_string(p_game_over->getRank()));
	std::vector<ScoreEntry> scores = loadHighScores();
	batch.check(scores.size() == 1, "dying saves the run to the high score table",
		"saved runs: " + std::to_string(scores.size()));
	if (scores.size() == 1) {
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

	writeHighScoreFile(std::string(HIGH_SCORE_HEADER) +
		"-3,1,2.00\n2,-1,2.00\n2,1,-2.00\n2,1,nan\n2,1,inf\n4,1,3.00\n");
	loaded = loadHighScores();
	batch.check(loaded.size() == 1 && loaded[0].seeds == 4,
		"saved rows with negative or non-number values are skipped", "rows loaded: " + std::to_string(loaded.size()));

	// --- Death screen: impossible totals ---
	clearHighScores();
	GameOver* p_bad_game_over = new GameOver(-5, -2, -1, -3.0);
	batch.check(p_bad_game_over->getRank() == -1 && loadHighScores().empty(),
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

	// --- A blocked hop still counts as a move ---
	// Documents current behaviour: Hero counts W as a move even when forward() refuses to move.
	placeOwl(p_hero, LEFT_EDGE_TILE, FACING_LEFT);
	int moves_before_blocked = p_hero->getMoves();
	pressKey(df::Keyboard::W);
	batch.check(samePosition(p_hero->getPosition(), LEFT_EDGE_TILE) && p_hero->getMoves() == moves_before_blocked + 1,
		"a hop blocked by the window edge still counts as a move (current behaviour)",
		"moves before: " + std::to_string(moves_before_blocked) + " after: " + std::to_string(p_hero->getMoves()));

	// --- Level Time restarts on a new map ---
	runFrames(30); // let the level clock run for about a second
	double time_before_exit = levelTimeShown(p_hero);
	df::Object* p_exit = findReachable(p_hero, "mapExit", stand, facing);
	batch.check(p_exit != nullptr, "found the exit and a free tile next to it");
	if (p_exit != nullptr) {
		placeOwl(p_hero, stand, facing);
		pressKey(df::Keyboard::W);
		bool next_loaded = waitForMap();
		batch.check(next_loaded, "the next map finishes building", describeMapBuilder());
		runFrame();
		double time_after_exit = levelTimeShown(p_hero);
		batch.check(next_loaded && time_after_exit < time_before_exit, "Level Time restarts when a new map loads",
			"level time before exit: " + std::to_string(time_before_exit) +
			" after new map: " + std::to_string(time_after_exit));
	}

	// --- The death screen's move count ---
	// Documents current behaviour: die() runs inside forward(), before eventHandler adds the
	// hop to m_moves, so the fatal hop isn't in the death screen's total.
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
	batch.check(p_game_over != nullptr && p_game_over->getMoves() == moves_before_fatal,
		"death screen move count leaves out the fatal hop (current behaviour)",
		"moves before the fatal hop: " + std::to_string(moves_before_fatal) + " death screen: " +
		std::to_string(p_game_over != nullptr ? p_game_over->getMoves() : -1));

	// --- A second run starts from zero ---
	runFrames(DEATH_INPUT_DELAY);
	pressKey(df::Keyboard::SPACE);
	runFrame();
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

// TEMP-MAPGEN-DEBUG: build maps back to back (no gameplay, no drawing) and count the ones
// that never finish building within 10 seconds. Returns that count.
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
