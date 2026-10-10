#pragma once

// Where runGameTests() saves its totals ("<tests> <failures>"), next to the exe,
// so a Debug build's title screen can show the most recent run
const char* const GAME_TEST_RESULTS_FILE = "gametests_results.txt";

// Run every game test batch (Normal, Errors, Edge Cases) against the real game.
// Results are written to dragonfly.log, and a combined total is printed to the console.
// Returns the total number of failed tests (0 = all passed).
int runGameTests();

int runFrameProfile(int frames); // TEMP-PROFILE

#ifdef _DEBUG
// Debug builds only: build maps back to back (no gameplay, no drawing) and count the ones
// that never finish building within 10 seconds. Returns that count.
int runMapStress(int maps);

// Debug builds only: build maps with fixed seeds first_seed..last_seed and list the seeds
// whose map has seeds or an exit the owl can't reach; draws the first such map as text.
// Returns how many bad maps it found.
int runMapScan(int first_seed, int last_seed);
#endif
