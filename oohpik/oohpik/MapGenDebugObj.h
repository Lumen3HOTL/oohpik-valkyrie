#pragma once
#include <string>
#include <vector>
#include <Vector.h>
#include <LogManager.h>
#include <mutex>
namespace ookpik {

	class MapGenDebugObj {
	private:
		bool m_delete_mode;
		bool m_debug_mode;
		std::string m_debug_map1;
		std::string m_debug_map2;
		std::vector<std::string> m_current_debug_strip;
		std::vector<std::vector<std::string>> m_debug_strips;
		std::vector<df::Vector> m_debug_Coords;
		std::vector<df::Vector> m_debug_map_positions;
		std::mutex m_debug_gate = std::mutex();
		std::vector<std::string> m_error_messages;
	public:
		void setDebugMode(bool new_debug_mode);
		bool getDebugMode();
		MapGenDebugObj(bool debug_mode);
		MapGenDebugObj();
		std::string getDebugMap1();
		
		std::string getDebugMap2();
		
		std::vector<std::string> getCurrentDebugStrip();
		
		std::vector<std::vector<std::string>> getCurrentDebugStripList();
		
		std::vector<df::Vector> getDebugCoords();

		std::vector<df::Vector> getDebugMapPositions();
		
		std::vector<std::string> getErrorMessages();
	

		void setDeleteMode();


		void reset();
		int getErrorMessageQueueSize();
		bool errorMessageQueueEmpty();
		int drainErrorMessageQueueToLog();
		void queueErrorMessage(std::string new_message);
		void clearErrorMessages();
		void setErrorMessages(std::vector<std::string> new_error_messages);
		void setDebugMap1(std::string new_debug_map_1);
		void addToDebugMap1(std::string new_debug_map_1_addition);
		void logDebugMap1();
		void setDebugMap2(std::string new_debug_map_2);
		void addToDebugMap2(std::string new_debug_map_2_addition);
		void logDebugMap2();
		void setCurrentDebugStrip(std::vector<std::string> new_debug_strip);
		void addToCurrentDebugStrip(std::string new_debug_strip_addtion);
		void storeCurrentDebugStrip();
		void clearCurrentDebugStrip();
		void clearDebugStripList();
		std::vector<std::string> getDebugStripAtIndex(int index);
		int compileDebugStripsIntoMap2();
		void setDebugCoords(std::vector<df::Vector> new_debug_coords);
		void addDebugCoord(df::Vector new_coord);
		void clearDebugCoords();

		df::Vector getDebugCoordAtIndex(int index);
		
		void setDebugMapPositions(std::vector<df::Vector> new_debug_map_positions);
		void addDebugMapPosition(df::Vector new_map_position);
		void clearDebugMapPositions();
		df::Vector getDebugMapPositionAtIndex(int index);
		void logDebugMapPositionsAndCoords();
		

	};
}