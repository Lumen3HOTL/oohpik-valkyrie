#include "HighScores.h"
#include "DisplayManager.h"
#include <cmath>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

namespace {
	const char* HIGH_SCORE_FILE = "highscores.csv";
	const char* HIGH_SCORE_HEADER = "seeds,levels,time";
	const int CENTER_X = 57; // middle of the 115-column window

	// A run that could actually happen: no negative counts, and a real, non-negative time
	bool isPossible(const ScoreEntry& entry) {
		return entry.seeds >= 0 && entry.levels >= 0 && std::isfinite(entry.time) && entry.time >= 0.0;
	}

	// Ranking: more seeds wins; ties go to more levels; then to the faster run.
	bool isBetter(const ScoreEntry& a, const ScoreEntry& b) {
		if (a.seeds != b.seeds) return a.seeds > b.seeds;
		if (a.levels != b.levels) return a.levels > b.levels;
		return a.time < b.time;
	}

	void saveHighScores(const std::vector<ScoreEntry>& scores) {
		std::ofstream file(HIGH_SCORE_FILE, std::ios::trunc);
		if (!file) {
			return; // can't write (e.g. read-only folder); the run just isn't recorded
		}
		file << HIGH_SCORE_HEADER << "\n";
		char time_text[32];
		for (const ScoreEntry& entry : scores) {
			std::snprintf(time_text, sizeof(time_text), "%.2f", entry.time);
			file << entry.seeds << "," << entry.levels << "," << time_text << "\n";
		}
	}
}

std::vector<ScoreEntry> loadHighScores() {
	std::vector<ScoreEntry> scores;
	std::ifstream file(HIGH_SCORE_FILE);
	std::string line;
	while (std::getline(file, line) && (int)scores.size() < MAX_HIGH_SCORES) {
		// Expect "seeds,levels,time"; skip the header, anything malformed and impossible runs
		std::stringstream fields(line);
		ScoreEntry entry;
		char comma1 = 0, comma2 = 0;
		if (fields >> entry.seeds >> comma1 >> entry.levels >> comma2 >> entry.time && comma1 == ',' && comma2 == ',' &&
			isPossible(entry)) {
			scores.push_back(entry);
		}
	}
	return scores;
}

int submitHighScore(const ScoreEntry& entry) {
	// Negative counts or a negative / non-number time can't come from a real run: not saved
	if (!isPossible(entry)) {
		return -1;
	}

	std::vector<ScoreEntry> scores = loadHighScores();

	// Find the first saved run this one beats; the new run goes just above it.
	// A run equal to an existing one goes below it, so earlier runs keep their place.
	int position = (int)scores.size();
	for (int i = 0; i < (int)scores.size(); i++) {
		if (isBetter(entry, scores[i])) {
			position = i;
			break;
		}
	}

	// Beats none of a full top 10: not saved
	if (position >= MAX_HIGH_SCORES) {
		return -1;
	}

	scores.insert(scores.begin() + position, entry);
	if ((int)scores.size() > MAX_HIGH_SCORES) {
		scores.pop_back(); // drop the lowest of the eleven
	}
	saveHighScores(scores);
	return position;
}

void drawHighScoreTable(int top_row, int highlight_index) {
	df::DisplayManager& dm = df::DisplayManager::getInstance(); // not DM: that macro points at the ResourceManager
	std::vector<ScoreEntry> scores = loadHighScores();

	const char* header = "rank   seeds   levels       time";
	int left = CENTER_X - (int)std::string(header).length() / 2;
	dm.drawString(df::Vector(left, top_row), header, df::LEFT_JUSTIFIED, df::WHITE);

	if (scores.empty()) {
		dm.drawString(df::Vector(CENTER_X, top_row + 2), "no high scores yet", df::CENTER_JUSTIFIED, df::WHITE);
		return;
	}

	char row[64];
	for (int i = 0; i < (int)scores.size(); i++) {
		std::snprintf(row, sizeof(row), "%3d.   %5d   %6d   %7.2fs", i + 1, scores[i].seeds, scores[i].levels, scores[i].time);
		df::Color color = (i == highlight_index) ? df::YELLOW : df::WHITE;
		dm.drawString(df::Vector(left, top_row + 1 + i), row, df::LEFT_JUSTIFIED, color);
	}
}
