#include "MapGenDebugObj.h"

namespace ookpik {
	void MapGenDebugObj::setDebugMode(bool new_debug_mode) {
		if (!m_delete_mode) {
			m_debug_gate.lock();
			m_debug_mode = new_debug_mode;
			m_debug_gate.unlock();
		}
		
	}
	bool MapGenDebugObj::getDebugMode() {
		bool temp = false;
		if (!m_delete_mode) {
			m_debug_gate.lock();
			temp = m_debug_mode;
			m_debug_gate.unlock();
		}
		
		
		return temp;
	}
	MapGenDebugObj::MapGenDebugObj() {

		m_debug_mode = false;
		m_debug_map1 = "";
		m_debug_map2 = "";
		m_current_debug_strip = std::vector<std::string>();
		m_debug_strips = std::vector<std::vector<std::string>>();
		m_debug_Coords = std::vector<df::Vector>();
		m_debug_map_positions = std::vector<df::Vector>();
		m_delete_mode = false;
		m_error_messages = std::vector<std::string>();
	}
	MapGenDebugObj::MapGenDebugObj(bool debug_mode) {

		m_debug_mode = debug_mode;
		m_debug_map1="";
		m_debug_map2="";
		m_current_debug_strip=std::vector<std::string>();
		m_debug_strips= std::vector<std::vector<std::string>>();
		m_debug_Coords = std::vector<df::Vector>();
		m_debug_map_positions= std::vector<df::Vector>();
		m_delete_mode = false;
		m_error_messages=std::vector<std::string>();
	}
	std::string MapGenDebugObj::getDebugMap1() {
		std::string temp = "";
		if (!m_delete_mode) {
			m_debug_gate.lock();
			temp = m_debug_map1;
			m_debug_gate.unlock();
		}
		
		
		return temp;
	}
	
	std::string MapGenDebugObj::getDebugMap2() {
		
		std::string temp = "";
		if (!m_delete_mode) {
			m_debug_gate.lock();
			temp = m_debug_map2;
			m_debug_gate.unlock();
		}
	
		return temp;
	}

	std::vector<std::string> MapGenDebugObj::getCurrentDebugStrip() {
		
		std::vector<std::string> temp;
		if (!m_delete_mode) {
			m_debug_gate.lock();
			temp = m_current_debug_strip;
			m_debug_gate.unlock();
		}
		
		return temp;
	}

	std::vector<std::vector<std::string>> MapGenDebugObj::getCurrentDebugStripList() {
		std::vector<std::vector<std::string>> temp;
		if (!m_delete_mode) {
			m_debug_gate.lock();
			temp = m_debug_strips;
			m_debug_gate.unlock();
		}
		
		
		return temp;
	}
	
	std::vector<df::Vector> MapGenDebugObj::getDebugCoords() {
		std::vector<df::Vector> temp;
		if (!m_delete_mode) {
			m_debug_gate.lock();
			temp = m_debug_Coords;
			m_debug_gate.unlock();
		}
		
		
		return temp;
	}

	std::vector<df::Vector> MapGenDebugObj::getDebugMapPositions() {
		std::vector<df::Vector> temp;
		if (!m_delete_mode) {
			m_debug_gate.lock();
			temp = m_debug_map_positions;
			m_debug_gate.unlock();
		}
		
		
		return temp;
	}

	std::vector<std::string> MapGenDebugObj::getErrorMessages() {
		std::vector<std::string> temp;
		if (!m_delete_mode) {
			m_debug_gate.lock();
			temp = m_error_messages;
			m_debug_gate.unlock();
		}
		

		return temp;
	}


	void MapGenDebugObj::reset() {
		if (!m_delete_mode) {
			m_debug_gate.lock();
	
			m_debug_map1 = "";
			m_debug_map2 = "";
			m_current_debug_strip.clear();
			m_debug_strips.clear();
			m_debug_Coords.clear();
			m_debug_map_positions.clear();

			m_error_messages.clear();
			m_debug_gate.unlock();
		}
		
		
	}
	int MapGenDebugObj::getErrorMessageQueueSize() {
		int temp = 0;
		if (!m_delete_mode) {
			m_debug_gate.lock();
			temp = m_error_messages.size();
			m_debug_gate.unlock();
		}
		
		
		return temp;
	}
	bool MapGenDebugObj::errorMessageQueueEmpty() {
		bool temp = false;
		if (!m_delete_mode) {
			m_debug_gate.lock();
			temp = m_error_messages.empty();
			m_debug_gate.unlock();
		}
		
		
		return temp;
	}
	int MapGenDebugObj::drainErrorMessageQueueToLog() {
		
		if (!m_delete_mode) {
			std::vector<std::string> temp;
			m_debug_gate.lock();
			temp = m_error_messages;
			m_error_messages.clear();
			m_debug_gate.unlock();
			df::LogManager& lm = df::LogManager::getInstance();
			int error = 0;
			for (int i = 0; i < temp.size(); i++) {
				error = lm.writeLog(temp[i].c_str());
				if (error == -1) {
					return error;
				}
			}
		}
		
		
		return 0;
	}
	void MapGenDebugObj::queueErrorMessage(std::string new_message) {
		if (!m_delete_mode) {
			m_debug_gate.lock();
			m_error_messages.push_back(new_message);
			m_debug_gate.unlock();
		}
		
	}
	void MapGenDebugObj::clearErrorMessages() {
		if (!m_delete_mode) {
			m_debug_gate.lock();
			m_error_messages.clear();
			m_debug_gate.unlock();
		}
		
	}
	void MapGenDebugObj::setErrorMessages(std::vector<std::string> new_error_messages) {
		if (!m_delete_mode) {
			m_debug_gate.lock();
			m_error_messages = new_error_messages;
			m_debug_gate.unlock();
		}
		
	}
	void MapGenDebugObj::setDebugMap1(std::string new_debug_map_1) {
		if (!m_delete_mode) {
			m_debug_gate.lock();
			m_debug_map1 = new_debug_map_1;
			m_debug_gate.unlock();
		}
		
	}
	void MapGenDebugObj::addToDebugMap1(std::string new_debug_map_1_addition) {
		if (!m_delete_mode) {
			m_debug_gate.lock();
			m_debug_map1.append(new_debug_map_1_addition);
			m_debug_gate.unlock();
		}
		
	}
	void MapGenDebugObj::logDebugMap1() {
		if (!m_delete_mode) {
			df::LogManager& lm = df::LogManager::getInstance();
			m_debug_gate.lock();

			lm.writeLog(std::string("debug map 1:\n").append(m_debug_map1).c_str());
			m_debug_map1.clear();
			m_debug_gate.unlock();
		}
		

	}
	void MapGenDebugObj::setDebugMap2(std::string new_debug_map_2) {
		if (!m_delete_mode) {
			m_debug_gate.lock();
			m_debug_map2 = new_debug_map_2;
			m_debug_gate.unlock();
		}
		
	}
	void MapGenDebugObj::addToDebugMap2(std::string new_debug_map_2_addition) {
		if (!m_delete_mode) {
			m_debug_gate.lock();
			m_debug_map2.append(new_debug_map_2_addition);
			m_debug_gate.unlock();
		}
		
	}
	void MapGenDebugObj::logDebugMap2() {
		if (!m_delete_mode) {
			df::LogManager& lm = df::LogManager::getInstance();
			m_debug_gate.lock();

			lm.writeLog(std::string("debug map 2:\n").append(m_debug_map2).c_str());
			m_debug_map2.clear();
			m_debug_gate.unlock();
		}
		
	}
	void MapGenDebugObj::setCurrentDebugStrip(std::vector<std::string> new_debug_strip) {
		if (!m_delete_mode) {
			m_debug_gate.lock();

			m_current_debug_strip = new_debug_strip;

			m_debug_gate.unlock();
		}
	
	}
	void MapGenDebugObj::addToCurrentDebugStrip(std::string new_debug_strip_addtion) {
		if (!m_delete_mode) {
			m_debug_gate.lock();

			m_current_debug_strip.push_back(new_debug_strip_addtion);

			m_debug_gate.unlock();
		}
		
	}
	void MapGenDebugObj::storeCurrentDebugStrip() {
		if (!m_delete_mode) {
			m_debug_gate.lock();
			m_debug_strips.push_back(m_current_debug_strip);
			m_current_debug_strip.clear();


			m_debug_gate.unlock();
		}
	
	}
	void MapGenDebugObj::clearCurrentDebugStrip() {
		if (!m_delete_mode) {
			m_debug_gate.lock();


			m_current_debug_strip.clear();
			m_debug_gate.unlock();
		}
		
	}
	void MapGenDebugObj::clearDebugStripList() {
		if (!m_delete_mode) {
			m_debug_gate.lock();

			m_debug_strips.clear();

			m_debug_gate.unlock();
		}
		
	}
	std::vector<std::string> MapGenDebugObj::getDebugStripAtIndex(int index) {
		std::vector<std::string> temp;
		if (!m_delete_mode) {
			m_debug_gate.lock();
			if ((index >= 0) && (index < m_debug_strips.size())) {
				temp = m_debug_strips[index];
			}


			m_debug_gate.unlock();
		}
	
		
		return temp;
		
	}
	int MapGenDebugObj::compileDebugStripsIntoMap2() {
		int error = 0;
		if (!m_delete_mode) {
			m_debug_gate.lock();
			if (!m_debug_strips.empty()) {
				m_debug_map2.clear();
				int uniformY = m_debug_strips[0].size();
				for (int i = 0; i < m_debug_strips.size();i++) {
					if (m_debug_strips[i].size() != uniformY) {
						error = -1;
						break;
					}
				}
				if (error != -1) {

					for (int y = 0; y < uniformY; y++) {
						for (int x = 0; x < m_debug_strips.size();x++) {
							m_debug_map2.append(m_debug_strips[x][y]);
						}
						m_debug_map2.append("\n");
					}

				}
			}


			m_debug_gate.unlock();
		}
		
		
		return error;
	}
	void MapGenDebugObj::setDebugCoords(std::vector<df::Vector> new_debug_coords) {
		if (!m_delete_mode) {
			m_debug_gate.lock();

			m_debug_Coords = new_debug_coords;

			m_debug_gate.unlock();
		}
		
	}
	void MapGenDebugObj::addDebugCoord(df::Vector new_coord) {
		if (!m_delete_mode) {
			m_debug_gate.lock();

			m_debug_Coords.push_back(new_coord);

			m_debug_gate.unlock();
		}
		
	}
	void MapGenDebugObj::clearDebugCoords() {
		if (!m_delete_mode) {
			m_debug_gate.lock();

			m_debug_Coords.clear();

			m_debug_gate.unlock();
		}
		
	}

	df::Vector MapGenDebugObj::getDebugCoordAtIndex(int index) {
		df::Vector temp;
		if (!m_delete_mode) {
			m_debug_gate.lock();
			if ((index >= 0) && (index < m_debug_Coords.size())) {
				temp = m_debug_Coords[index];
			}


			m_debug_gate.unlock();
		}
		
		
		return temp;
	}
	void MapGenDebugObj::setDebugMapPositions(std::vector<df::Vector> new_debug_map_positions) {
		if (!m_delete_mode) {
			m_debug_gate.lock();

			m_debug_map_positions = new_debug_map_positions;

			m_debug_gate.unlock();
		}
		
	}
	void MapGenDebugObj::addDebugMapPosition(df::Vector new_map_position) {
		if (!m_delete_mode) {
			m_debug_gate.lock();

			m_debug_map_positions.push_back(new_map_position);

			m_debug_gate.unlock();
		}
		
	}
	void MapGenDebugObj::clearDebugMapPositions() {
		if (!m_delete_mode) {
			m_debug_gate.lock();

			m_debug_map_positions.clear();

			m_debug_gate.unlock();
		}
		
	}
	df::Vector MapGenDebugObj::getDebugMapPositionAtIndex(int index) {
		df::Vector temp;
		if (!m_delete_mode) {
			m_debug_gate.lock();

			if ((index >= 0) && (index < m_debug_map_positions.size())) {
				temp = m_debug_map_positions[index];
			}

			m_debug_gate.unlock();
		}
		
		
		return temp;
	}

	void  MapGenDebugObj::setDeleteMode() {
		if (!m_delete_mode) {
			m_debug_gate.lock();
			m_delete_mode = true;
			m_debug_gate.unlock();
		}
		
	}
	
	void MapGenDebugObj::logDebugMapPositionsAndCoords() {
		if (!m_delete_mode) {
			m_debug_gate.lock();

			std::string coordsString = "visited coords: ";
			for (int i = 0; i < m_debug_Coords.size(); i++) {
				coordsString.append(std::string("< coord: ").append(std::to_string(i)).append(" map pos:  x:").append(std::to_string(m_debug_map_positions[i].getX())).append(" y: ").append(std::to_string(m_debug_map_positions[i].getY())).append(" object pos: x: ").append(std::to_string(m_debug_Coords[i].getX())).append(" y: ").append(std::to_string(m_debug_Coords[i].getY())).append(" >, "));
			}
			df::LogManager::getInstance().writeLog(coordsString.c_str());

			m_debug_gate.unlock();
		}
		
	}
}