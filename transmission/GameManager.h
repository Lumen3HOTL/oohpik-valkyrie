#pragma once
#include "Manager.h"
#include "Clock.h"
namespace df {
	// Default frame time (game loop time) in milliseconds (33 ms == 30 f/s).
	const int FRAME_TIME_DEFAULT = 33;

	class GameManager : public Manager {

	private:
		GameManager(); // Private since a singleton.
		GameManager(GameManager const&); // Don't allow copy.
		void operator =(GameManager const&); // Don't allow assignment.
		bool m_game_over; // True, then game loop should stop.
		int m_frame_time; // Target time per game loop, in milliseconds.
		unsigned int m_last_frame_time;
		unsigned long long m_last_delta_time;
		Clock m_lastDeltaClock;

	public:
		// Get the singleton instance of the GameManager.
		static GameManager& getInstance();

		// Startup all GameManager services.
		int startUp(bool appendRun = false);

		// Shut down GameManager services.
		void shutDown();

		// Run game loop.
		void run();

		// Set game over status to indicated value.
		// If true (default), will stop game loop.
		void setGameOver(bool new_game_over = true);

		// Get game over status.
		bool getGameOver() const;

		// Return frame time.
		// Frame time is target time for game loop, in milliseconds.
		int getFrameTime() const;

		unsigned int getLastFrameTime()const;
		unsigned int getLastDeltaTime()const;
	};
}
 