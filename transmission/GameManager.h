#pragma once
#include "Manager.h"
#include "Clock.h"
namespace df {
	// D e f a u l t frame ti m e ( game l o o p ti m e ) i n m i l l i s e c o n d s ( 3 3 ms == 30 f / s ) .
	const int FRAME_TIME_DEFAULT = 33;

	class GameManager : public Manager {

	private:
		GameManager(); // P r i v a t e s i n c e a s i n g l e t o n .
		GameManager(GameManager const&); // Don ’ t a l l o w copy .
		void operator =(GameManager const&); // Don ’ t a l l o w a s s i g n m e n t .
		bool m_game_over; // True , t h e n game l o o p s h o u l d s t o p .
		int m_frame_time; // T a r g e t ti m e p e r game l o o p , i n m i l l i s e c o n d s .
		unsigned int m_last_frame_time;
		unsigned long long m_last_delta_time;
		Clock m_lastDeltaClock;

	public:
		// Get t h e s i n g l e t o n i n s t a n c e o f t h e GameManager .
		static GameManager& getInstance();

		// S t a r t u p a l l GameManager s e r v i c e s .
		int startUp(bool appendRun = false);

		// S h u t down GameManager s e r v i c e s .
		void shutDown();

		// Run game l o o p .
		void run();

		// S e t game o v e r s t a t u s t o i n d i c a t e d v a l u e .
		// I f t r u e ( d e f a u l t ) , w i l l s t o p game l o o p .
		void setGameOver(bool new_game_over = true);

		// Get game o v e r s t a t u s .
		bool getGameOver() const;

		// Return frame ti m e .
		// Frame ti m e i s t a r g e t ti m e f o r game l o o p , i n m i l l i s e c o n d s .
		int getFrameTime() const;

		unsigned int getLastFrameTime()const;
		unsigned int getLastDeltaTime()const;
	};
}
 