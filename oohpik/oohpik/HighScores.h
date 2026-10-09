#pragma once
#include <vector>

// One finished run
struct ScoreEntry {
	int seeds;    // seeds collected over the whole run
	int levels;   // maps completed (exits reached)
	double time;  // length of the run, in seconds
};

// The table is saved as highscores.csv next to the game (columns: seeds,levels,time),
// best run first, so it can be displayed without sorting
const int MAX_HIGH_SCORES = 10;

// Read the saved table (best first). Returns an empty list if there is no file yet
std::vector<ScoreEntry> loadHighScores();

// Insert a finished run into the table in ranked order and save it, keeping only the top 10.
// Returns the run's position in the table (0 = best), or -1 if it didn't make the top 10
// or is impossible (negative seeds or levels, or a negative, NaN or infinite time)
int submitHighScore(const ScoreEntry& entry);

// Draw the table centred on the screen, starting at top_row.
// highlight_index is the row to show in yellow (-1 for none)
void drawHighScoreTable(int top_row, int highlight_index);
