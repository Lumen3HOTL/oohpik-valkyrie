#pragma once
#include "Object.h"
#include "MapGenConfigObj.h"

// The map layout every level uses (fills the 115x30 window)
ookpik::MapGenConfig makeMapConfig();

// Enemies on a level (level 1 is the first map): none on level 1, 1 on levels 2-3,
// 2 on levels 4-5, then 3 from level 6 on
int enemyCountForLevel(int level);

// Enemies get faster every ENEMY_SPEEDUP_LEVELS levels (from level 2), stepping
// ENEMY_SPEEDUP_STEPS steps sooner each time, but never more often than every
// ENEMY_FASTEST_MOVE_STEPS steps. At 30 fps: 1.0 s on levels 2-4, 0.9 s on 5-7, ... 0.3 s from level 23.
const int ENEMY_SPEEDUP_LEVELS = 3;
const int ENEMY_SPEEDUP_STEPS = 3;
const int ENEMY_FASTEST_MOVE_STEPS = 9;

// Steps between enemy moves on a level
int enemyMoveStepsForLevel(int level);

// Enemies spawn at least this many hops from the owl (along open tiles), when the map has room
const int ENEMY_SPAWN_MIN_HOPS = 10;

// Put `count` enemies, stepping every `move_steps` steps, on random free tiles of the current
// map, as far as possible at least ENEMY_SPAWN_MIN_HOPS hops from the owl. Returns how many were placed.
int spawnEnemies(int count, int move_steps, df::Object* p_owl);

// Remove every map object (trees, ground, seeds, exit, enemies and the map builder) at the end of this frame.
void clearMap();

// Clear the current map (if there is one) and start generating a new one.
// The owl is kept; the map builder moves it to the new map's start position.
void startNewMap(df::Object* p_owl);
