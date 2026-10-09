#pragma once
#include "Object.h"
#include "MapGenConfigObj.h"

// The map layout every level uses (fills the 115x30 window)
ookpik::MapGenConfig makeMapConfig();

// Remove every map object (trees, ground, seeds, exit and the map builder) at the end of this frame.
void clearMap();

// Clear the current map (if there is one) and start generating a new one.
// The owl is kept; the map builder moves it to the new map's start position.
void startNewMap(df::Object* p_owl);
