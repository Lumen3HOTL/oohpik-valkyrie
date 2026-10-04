#pragma once
// The log file manager.

 #ifndef __LOG_MANAGER_H__
 #define __LOG_MANAGER_H__
#define _CRT_SECURE_NO_DEPRECATE
 // System includes.
 #include <stdio.h>
#include <chrono>

 // Engine includes.
 #include "Manager.h"

 namespace df {
	
		const std::string LOGFILE_NAME = "dragonfly.log";
	
		class LogManager : public Manager {
		
		 private:
			LogManager(); // Private since a singleton.
			LogManager(LogManager const&); // Don't allow copy.
			void operator =(LogManager const&); // Don't allow assignment.
			bool m_do_flush; // True if flush to disk after each write.
			bool appendMode;
			FILE * m_p_f; // Pointer to log file struct.
			unsigned long m_line_Number;
			std::chrono::time_point<std::chrono::high_resolution_clock> m_start_time;
			void updateLineNumber();
		
		public:
			// If log file is open, close it.
				~LogManager();
			
				// Get the one and only instance of the LogManager.
				static LogManager& getInstance();
				
				// Startup the LogManager (open log file "dragonfly.log").
				int startUp(bool appendMode=false);
				
				// Shut down the LogManager (close log file).
				void shutDown();
				
				// Set flush of log file after each write.
				void setFlush(bool do_flush = true);
				
				// Write to log file. Support sprintf() formatting of strings.
				// Return number of bytes written, -1 if error.
				int writeLog(const char* fmt, ...) ;
				
		};
		
			
 } // end o f namespace d f
 #endif// LOG MANAGER H