#pragma once

// Run every game test batch (Normal, Errors, Edge Cases) against the real game.
// Results are written to dragonfly.log, and a combined total is printed to the console.
// Returns the total number of failed tests (0 = all passed).
int runGameTests();

int runMapStress(int maps); // TEMP-MAPGEN-DEBUG
