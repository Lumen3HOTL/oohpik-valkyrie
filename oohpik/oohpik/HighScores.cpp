#include "HighScores.h"
#include "DisplayManager.h"
#include <cmath>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

namespace {
	const char* HIGH_SCORE_FILE = "highscores.csv";
	const char* HIGH_SCORE_HEADER = "name,seeds,levels,time";
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

	// Read a whole field as a number; false if it's empty or has anything else in it
	bool parseInt(const std::string& text, int& value) {
		std::stringstream stream(text);
		return (stream >> value) && (stream >> std::ws).eof();
	}

	bool parseDouble(const std::string& text, double& value) {
		std::stringstream stream(text);
		return (stream >> value) && (stream >> std::ws).eof();
	}

	// Read one row: "name,seeds,levels,time", or "seeds,levels,time" from before names existed.
	// False for the header, anything malformed and impossible runs.
	bool parseRow(std::string line, ScoreEntry& entry) {
		if (!line.empty() && line.back() == '\r') {
			line.pop_back(); // a file edited on Windows
		}
		std::vector<std::string> fields;
		std::stringstream row(line);
		std::string field;
		while (std::getline(row, field, ',')) {
			fields.push_back(field);
		}

		int first_number = 0;
		if (fields.size() == 4) {
			entry.name = isValidHighScoreName(fields[0]) ? fields[0] : NO_HIGH_SCORE_NAME;
			first_number = 1;
		}
		else if (fields.size() == 3) {
			entry.name = NO_HIGH_SCORE_NAME;
		}
		else {
			return false;
		}
		return parseInt(fields[first_number], entry.seeds) && parseInt(fields[first_number + 1], entry.levels) &&
			parseDouble(fields[first_number + 2], entry.time) && isPossible(entry);
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
			file << entry.name << "," << entry.seeds << "," << entry.levels << "," << time_text << "\n";
		}
	}

	// Where a run goes in the table: just above the first saved run it beats.
	// A run equal to an existing one goes below it, so earlier runs keep their place.
	int findPosition(const std::vector<ScoreEntry>& scores, const ScoreEntry& entry) {
		for (int i = 0; i < (int)scores.size(); i++) {
			if (isBetter(entry, scores[i])) {
				return i;
			}
		}
		return (int)scores.size();
	}
}

bool isValidHighScoreName(const std::string& name) {
	if ((int)name.length() != HIGH_SCORE_NAME_LENGTH) {
		return false;
	}
	for (char c : name) {
		if (!((c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))) {
			return false;
		}
	}
	return true;
}

std::vector<ScoreEntry> loadHighScores() {
	std::vector<ScoreEntry> scores;
	std::ifstream file(HIGH_SCORE_FILE);
	std::string line;
	while (std::getline(file, line) && (int)scores.size() < MAX_HIGH_SCORES) {
		ScoreEntry entry;
		if (parseRow(line, entry)) {
			scores.push_back(entry);
		}
	}
	return scores;
}

int rankHighScore(const ScoreEntry& entry) {
	// Negative counts or a negative / non-number time can't come from a real run
	if (!isPossible(entry)) {
		return -1;
	}
	int position = findPosition(loadHighScores(), entry);
	return position < MAX_HIGH_SCORES ? position : -1; // beats none of a full top 10
}

int submitHighScore(const ScoreEntry& entry) {
	int position = rankHighScore(entry);
	if (position < 0) {
		return -1;
	}

	ScoreEntry saved = entry;
	if (!isValidHighScoreName(saved.name)) {
		saved.name = NO_HIGH_SCORE_NAME; // also keeps commas out of the file
	}

	std::vector<ScoreEntry> scores = loadHighScores();
	scores.insert(scores.begin() + position, saved);
	if ((int)scores.size() > MAX_HIGH_SCORES) {
		scores.pop_back(); // drop the lowest of the eleven
	}
	saveHighScores(scores);
	return position;
}

void drawHighScoreTable(int top_row, int highlight_index) {
	df::DisplayManager& dm = df::DisplayManager::getInstance(); // not DM: that macro points at the ResourceManager
	std::vector<ScoreEntry> scores = loadHighScores();

	const char* header = "rank   name   seeds   levels       time";
	int left = CENTER_X - (int)std::string(header).length() / 2;
	dm.drawString(df::Vector(left, top_row), header, df::LEFT_JUSTIFIED, df::WHITE);

	if (scores.empty()) {
		dm.drawString(df::Vector(CENTER_X, top_row + 2), "no high scores yet", df::CENTER_JUSTIFIED, df::WHITE);
		return;
	}

	char row[64];
	for (int i = 0; i < (int)scores.size(); i++) {
		std::snprintf(row, sizeof(row), "%3d.    %3s   %5d   %6d   %7.1fs", i + 1, scores[i].name.c_str(),
			scores[i].seeds, scores[i].levels, scores[i].time);
		df::Color color = (i == highlight_index) ? df::YELLOW : df::WHITE;
		dm.drawString(df::Vector(left, top_row + 1 + i), row, df::LEFT_JUSTIFIED, color);
	}
}
