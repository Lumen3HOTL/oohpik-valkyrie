#pragma once
// The l o g f i l e manager .

 #ifndef __LOG_MANAGER_H__
 #define __LOG_MANAGER_H__
#define _CRT_SECURE_NO_DEPRECATE
 // System i n c l u d e s .
 #include <stdio.h>
#include <chrono>

 // Engine i n c l u d e s .
 #include "Manager.h"

 namespace df {
	
		const std::string LOGFILE_NAME = "dragonfly.log";
	
		class LogManager : public Manager {
		
		 private:
			LogManager(); // P r i v a t e s i n c e a s i n g l e t o n .
			LogManager(LogManager const&); // Don ’ t a l l o w copy .
			void operator =(LogManager const&); // Don ’ t a l l o w a s s i g n m e n t .
			bool m_do_flush; // True i f f l u s h t o d i s k a f t e r e a c h w r i t e .
			bool appendMode;
			FILE * m_p_f; // P o i n t e r t o l o g f i l e s t r u c t .
			unsigned long m_line_Number;
			std::chrono::time_point<std::chrono::high_resolution_clock> m_start_time;
			void updateLineNumber();
		
		public:
			// I f l o g f i l e i s open , c l o s e i t .
				~LogManager();
			
				// Get t h e one and o n l y i n s t a n c e o f t h e LogManager .
				static LogManager& getInstance();
				
				// S t a r t up t h e LogManager ( open l o g f i l e ” d r a g o n f l y . l o g ”) .
				int startUp(bool appendMode=false);
				
				// S h u t down t h e LogManager ( c l o s e l o g f i l e ) .
				void shutDown();
				
				// S e t f l u s h o f l o g f i l e a f t e r e a c h w r i t e .
				void setFlush(bool do_flush = true);
				
				// Write t o l o g f i l e . S u p p o r t s p r i n t f ( ) f o r m a t t i n g o f s t r i n g s .
				// Return number o f b y t e s w r i t t e n , −1 i f e r r o r .
				int writeLog(const char* fmt, ...) ;
				
		};
		
			
 } // end o f namespace d f
 #endif// LOG MANAGER H