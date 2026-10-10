#pragma once
#include <string>
#include <vector>

// Initials are exactly this many characters, each A-Z or 0-9
const int HIGH_SCORE_NAME_LENGTH = 3;

// Shown for runs saved without initials (including tables saved before names existed)
const std::string NO_HIGH_SCORE_NAME = "---";

// One finished run
struct ScoreEntry {
	int seeds;    // seeds collected over the whole run
	int levels;   // maps completed (exits reached)
	double time;  // length of the run, in seconds
	std::string name = NO_HIGH_SCORE_NAME; // the player's initials
};

// The table is saved as highscores.csv next to the game (columns: name,seeds,levels,time),
// best run first, so it can be displayed without sorting. Older files without the name
// column still load, with NO_HIGH_SCORE_NAME as the name.
const int MAX_HIGH_SCORES = 10;

// True if a name can be saved: exactly HIGH_SCORE_NAME_LENGTH characters, each A-Z or 0-9
bool isValidHighScoreName(const std::string& name);

// Read the saved table (best first). Returns an empty list if there is no file yet
std::vector<ScoreEntry> loadHighScores();

// The position a run would take in the table (0 = best) without saving it, or -1 if it
// wouldn't make the top 10 or is impossible (negative seeds or levels, or a negative,
// NaN or infinite time)
int rankHighScore(const ScoreEntry& entry);

// Insert a finished run into the table in ranked order and save it, keeping only the top 10.
// A name that isn't valid is saved as NO_HIGH_SCORE_NAME.
// Returns the run's position in the table (0 = best), or -1 if it wasn't saved (see rankHighScore)
int submitHighScore(const ScoreEntry& entry);

// Draw the table centred on the screen, starting at top_row.
// highlight_index is the row to show in yellow (-1 for none)
void drawHighScoreTable(int top_row, int highlight_index);
