#include "Level.h"
#include "WorldManager.h"
#include "DisplayManager.h"
#include "MapBuilder.h"
#include "MapGenConfigObj.h"
#include "Enemy.h"
#include <algorithm>
#include <cmath>
#include <map>
#include <queue>
#include <random>
#include <set>
#include <vector>

// Map layout: fills the 115x30 window like the original ookpik.
// buildMap draws rows from y = 1, so 27 rows + a 1-tile border on each side = rows 1..29.
ookpik::MapGenConfig makeMapConfig() {
	ookpik::MapGenConfig config;
	config.setMapOrigin(df::Vector(2, 0));

	
	config.setMapWidth(53);
	config.setMapHeight(26);
	config.setMapBorderThickness(2);
	config.setObjectsConstructedPerFrame(5000); // build the whole map in one frame
	
	/*
	config.setMapWidth(500);
	config.setMapHeight(500);
	config.setMapBorderThickness(50);
	config.setObjectsConstructedPerFrame(1000); 
	*/
	
	
	config.setMapObjectWidth(2);
	config.setMapObjectHeight(1);
	config.setMapObjectAltitude(0);
	
	config.setTimeoutSeconds(60);
	config.setRandomSeed(0); // 0 = pick a new random seed each run

	// Open structures carved out of the forest
	config.setMinRooms(4);
	config.setMaxRooms(6);
	config.setMinRoomWidth(4);
	config.setMaxRoomWidth(12);
	config.setMinRoomHeight(3);
	config.setMaxRoomHeight(6);
	config.setMinRightAngleLines(15);
	config.setMaxRightAngleLine(20);
	config.setMinRightAngleLineWidth(10);
	config.setMaxRightAngleLineWidth(30);
	config.setMinRightAngleLineHeight(3);
	config.setMaxRightAngleLineHeight(12);
	config.setMinDiagLines(20);
	config.setMaxDiagLine(30);
	config.setMinDiagLineWidth(10);
	config.setMaxDiagLineWidth(30);
	config.setMinDiagLineHeight(3);
	config.setMaxDiagLineHeight(10);

	// Contents, based on the original game's small-map settings
	config.setMinRandTrees(50);
	config.setMaxRandTrees(150);
	config.setMinSeeds(15);
	config.setMaxSeeds(30);
	return config;
}

namespace {
	// Mark every object of a type for deletion (removed at the end of this frame)
	void removeObjectsOfType(const std::string& type) {
		df::ObjectList objects = WM.objectsOfType(type);
		for (int i = 0; i < objects.getCount(); i++) {
			WM.markForDelete(objects[i]);
		}
	}
}

int enemyCountForLevel(int level) {
	if (level < 2) {
		return 0;
	}
	return std::min(3, level / 2); // 2-3 -> 1, 4-5 -> 2, 6+ -> 3
}

int enemyMoveStepsForLevel(int level) {
	int speedups = std::max(0, (level - 2) / ENEMY_SPEEDUP_LEVELS); // levels 2-4: 0, 5-7: 1, ...
	return std::max(ENEMY_FASTEST_MOVE_STEPS, ookpik::ENEMY_MOVE_STEPS - speedups * ENEMY_SPEEDUP_STEPS);
}

int spawnEnemies(int count, int move_steps, df::Object* p_owl) {
	if (count <= 0 || p_owl == nullptr) {
		return 0;
	}
	df::DisplayManager& display = df::DisplayManager::getInstance();
	const int hop_x[] = { 0, 2, 0, -2 };
	const int hop_y[] = { -1, 0, 1, 0 };

	// Hops from the owl to every tile it could walk to (trees block the way)
	std::set<std::pair<int, int>> trees;
	df::ObjectList tree_list = WM.objectsOfType("Tree");
	for (int i = 0; i < tree_list.getCount(); i++) {
		trees.insert(std::make_pair((int)std::lround(tree_list[i]->getPosition().getX()), (int)std::lround(tree_list[i]->getPosition().getY())));
	}
	std::map<std::pair<int, int>, int> hops;
	std::queue<std::pair<int, int>> to_visit;
	std::pair<int, int> owl_tile((int)std::lround(p_owl->getPosition().getX()), (int)std::lround(p_owl->getPosition().getY()));
	hops[owl_tile] = 0;
	to_visit.push(owl_tile);
	int farthest = 0;
	while (!to_visit.empty()) {
		std::pair<int, int> tile = to_visit.front();
		to_visit.pop();
		for (int dir = 0; dir < 4; dir++) {
			std::pair<int, int> next(tile.first + hop_x[dir], tile.second + hop_y[dir]);
			if (next.first < 0 || next.second < 0 || next.first >= display.getHorizontal() || next.second >= display.getVertical() ||
				trees.count(next) > 0 || hops.count(next) > 0) {
				continue;
			}
			hops[next] = hops[tile] + 1;
			farthest = std::max(farthest, hops[next]);
			to_visit.push(next);
		}
	}

	// Far enough away if the map allows it, otherwise as far away as the map allows
	int min_hops = std::min(ENEMY_SPAWN_MIN_HOPS, farthest);
	static std::mt19937 engine(std::random_device{}());
	int placed = 0;
	for (int n = 0; n < count; n++) {
		// Free tiles only: nothing solid there (no seed, exit, owl or enemy already placed)
		std::vector<std::pair<int, int>> spots;
		for (const auto& entry : hops) {
			df::Vector position((float)entry.first.first, (float)entry.first.second);
			if (entry.second >= min_hops && entry.second > 0 && WM.getCollisions(p_owl, position).isEmpty()) {
				spots.push_back(entry.first);
			}
		}
		if (spots.empty()) {
			break;
		}
		std::uniform_int_distribution<int> pick(0, (int)spots.size() - 1);
		std::pair<int, int> spot = spots[pick(engine)];
		ookpik::Enemy* p_enemy = new ookpik::Enemy();
		p_enemy->setPosition(df::Vector((float)spot.first, (float)spot.second));
		p_enemy->setMoveInterval(move_steps);
		placed++;
	}
	return placed;
}

void clearMap() {
	// Remove by type. MapBuilder::destroyMap isn't used because its object list also contains the owl which must survive between maps.
	removeObjectsOfType("Tree");
	removeObjectsOfType("Ground");
	removeObjectsOfType("Seed");
	removeObjectsOfType("mapExit");
	removeObjectsOfType("Enemy");
	removeObjectsOfType("mapBuilder");
}

void startNewMap(df::Object* p_owl) {
	clearMap();

	ookpik::MapBuilder* p_map_builder = new ookpik::MapBuilder();
	p_map_builder->startGenerateMap(makeMapConfig(), p_owl);
}
