#pragma once
#include "Object.h"

// Clear the current map (if there is one) and start generating a new one.
// The owl is kept; the map builder moves it to the new map's start position.
void startNewMap(df::Object* p_owl);
