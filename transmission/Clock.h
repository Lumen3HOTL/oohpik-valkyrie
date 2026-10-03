#pragma once

#include <chrono>

namespace df {
	class Clock {
	private:
		//the previous time delta
		std::chrono::time_point<std::chrono::high_resolution_clock> m_previous_time;
	public:
		// S e t s p r e v i o u s t i m e t o c u r r e n t ti m e .
		Clock();

		// Return ti m e e l a p s e d s i n c e d e l t a ( ) was l a s t c a l l e d , −1 i f e r r o r .
		// R e s e t s p r e v i o u s ti m e .
		// U n i t s a r e m i c r o s e c o n d s .
		long long delta();

		// Does n o t r e s e t p r e v i o u s ti m e .
		// U n i t s a r e m i c r o s e c o n d s .
		long long split();


	};
	
}