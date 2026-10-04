#include "LogManager.h"
#include <stdarg.h>
namespace df{
	LogManager::LogManager() {
		//stop undefined unititialized memory behavoir
		this->setType("LogManager");
		m_do_flush = false;
		m_p_f = nullptr;
		m_line_Number = 0ul;
		m_start_time = std::chrono::high_resolution_clock::now();
		
	}

	LogManager::~LogManager() {
		//safety guard
		this->shutDown();
	}

	// Get the one and only instance of the LogManager.
	LogManager& LogManager::getInstance() {
		static LogManager logKeeper;
		return logKeeper;
	}

	

	int LogManager::startUp(bool appendMode) {
		if (Manager::startUp() < 0) {
			return -1;
		}
		m_p_f = nullptr;
		//open the log file with error check
		if (appendMode) {
			m_p_f = fopen(LOGFILE_NAME.c_str(), "a");
		}
		else {
			m_p_f = fopen(LOGFILE_NAME.c_str(), "w+");
		}
		
		if (m_p_f == nullptr) {
			this->shutDown();
			return -1;
		}
		//init the line number
		m_line_Number = 0ul;
		//write the first half of the log header with error check
		int startupWriteError = fprintf(m_p_f, "%s", "\n---dragonfly log start---\n---current time nanoseconds: ");
		if (startupWriteError == -1) {
			this->shutDown();
			return -1;
		}
		//init the start time
		m_start_time = (std::chrono::high_resolution_clock::now());
		//write the second half of the log header with error check
		startupWriteError = fprintf(m_p_f, "%s", std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(m_start_time.time_since_epoch()).count()).append("---\n").c_str());
		if (startupWriteError == -1) {
			this->shutDown();
			return -1;
		}
		//flush with error check
		startupWriteError = fflush(m_p_f);

		if (startupWriteError == -1) {
			this->shutDown();
			return -1;
		}
		return 0;
	}

	void LogManager::shutDown() {
		
		//safety check to prevent undefined behauvoir
		if (m_p_f != nullptr) {
			fprintf(m_p_f, "%s", "---current time nanoseconds: ");
			fprintf(m_p_f, "%s", std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count()).append("---\n").c_str());
			fprintf(m_p_f, "%s", "---dragonfly log end---\n\n");
			fflush(m_p_f);
			fclose(m_p_f);
			m_p_f = nullptr;
			m_line_Number = 0ul;
			m_start_time = std::chrono::high_resolution_clock::now();
		}
		Manager::shutDown();
	}

	void LogManager::setFlush(bool do_flush) {
		m_do_flush = do_flush;
	}

	int LogManager::writeLog(const char* fmt, ...)  {
		//don't try to write if we arent yet started
		if (isStarted()) {
			//write the header and error check it, yes i know its a really long one liner that does a ton of work every call and has many chances to fIl but i dont care, and it probably wont fail unless something deepr is very wrong anyway
			int error = fprintf(m_p_f, "%s", (std::string("<log number: ").append(std::to_string(m_line_Number)).append(" current time nanoseconds: ").append(std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count()))).append(" ns since start: ").append(std::to_string(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - m_start_time).count())).append("> ").c_str());
			if (error < 0) {
				return -1;
			}
			//update the line number
			m_line_Number += 1ul;
			//do the fancy varibable argument count write, and error check it
			va_list args;
			va_start(args, fmt);
			int bytesWritten = vfprintf(m_p_f, fmt, args);
			va_end(args);
			if (bytesWritten < 0) {
				return -1;
			}
			//write the footer and error check it
			error = fprintf(m_p_f, "%s", "\n");
			if (error < 0) {
				return - 1;
			}
			//if requested do the flush and error check it
			if(m_do_flush) {
				error= fflush(m_p_f);
				if (error < 0) {
					return -1;
				}
			}
			return bytesWritten;
		}
		//if we arent started yet error out
		return -1;
	}
}