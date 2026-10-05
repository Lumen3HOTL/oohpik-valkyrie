#include "MapBuilder.h"
#include <math.h>
namespace ookpik {
	

	std::vector<df::Vector> MapBuilder::generateBresenhamLine(int x1, int y1, int x2, int y2) {
		std::vector <df::Vector> pathPoints;

		if ((x1 == x2) && (y1 == y2)) {
			pathPoints.push_back(df::Vector(x1,y1));
			return pathPoints;
		}
		else if (x1 == x2) {
			int startY = y1;
			int endY = y2;
			if (startY > endY) {
				startY = y2;
				endY = y1;
			}

			for (int i = startY; i <= endY; i++) {
				pathPoints.push_back(df::Vector(x1, i));
			}
			return pathPoints;
		}

		else if (y1 == y2) {
			int startX = x1;
			int endX = x2;
			if (startX > endX) {
				startX = x2;
				endX = x1;
			}

			for (int i = startX; i <= endX; i++) {
				pathPoints.push_back(df::Vector(y1, i));
			}
			return pathPoints;
		}


		int xDistance = abs(x2 - x1);
		int yDistance = abs(y2 - y1);

		int stepX = 1;
		int stepY = 1;

		if (x1 > x2) {
			stepX = -1;
		}
		if (y1 > y2) {
			stepY = -1;
		}

		


		int currentX = x1;
		int currentY = y1;


		if (xDistance > yDistance) {

			float vectorError = (int)(xDistance / 2);

			while (currentX != x2) {
				pathPoints.push_back(df::Vector(currentX, currentY));

				vectorError -= yDistance;


				if (vectorError < 0) {
					currentY += stepY;
					vectorError += xDistance;
					pathPoints.push_back(df::Vector(currentX, currentY));
				}

				currentX += stepX;
			}
		}
		else {
			float vectorError = (int)(yDistance / 2);

			while (currentY != y2) {
				pathPoints.push_back(df::Vector(currentX, currentY));

				vectorError -= xDistance;

				if (vectorError < 0) {
					currentX += stepX;

					vectorError += yDistance;
					pathPoints.push_back(df::Vector(currentX, currentY));
				}
			}
		}
		pathPoints.push_back(df::Vector(x2, y2));

		return pathPoints;
	}

	std::vector<df::Vector> MapBuilder::generateXYLine(int x1, int y1, int x2, int y2, bool yFirst, bool point1First) {
		std::vector <df::Vector> pathPoints;
		int startX = x1;
		int endX = x2;
		int startY = y1;
		int endY = y2;

		if (startX > endX) {
			startX = x2;
			endX = x1;
		}
		if (startY > endY) {
			startY = y2;
			endY = y1;
		}

		if (x1 == x2) {
			

			for (int i = startY; i <= endY; i++) {
				pathPoints.push_back(df::Vector(x1, i));
			}
			return pathPoints;
		}

		else if (y1 == y2) {
			

			for (int i = startX; i <= endX; i++) {
				pathPoints.push_back(df::Vector(y1, i));
			}
			return pathPoints;
		}

		
		if (yFirst) {
			if (point1First) {
				for (int i = startY; i <= endY; i++) {
					pathPoints.push_back(df::Vector(x1, i));
				}
			}
			else {
				for (int i = startY; i <= endY; i++) {
					pathPoints.push_back(df::Vector(x2, i));
				}
			}
		}
		else {
			if (point1First) {
				for (int i = startX; i <= endX; i++) {
					pathPoints.push_back(df::Vector(i, y1));
				}
			}
			else {
				for (int i = startX; i <= endX; i++) {
					pathPoints.push_back(df::Vector(i, y2));
				}
			}
		}
		return pathPoints;
	}

	std::vector<std::vector<mapTileIds::mapTileId>> MapBuilder::createStartingMap(int width, int height, mapTileIds::mapTileId canvas) {
		std::vector<std::vector<mapTileIds::mapTileId>> map= std::vector<std::vector<mapTileIds::mapTileId>>();
		for (int x = 0; x < width; x++) {
			map.push_back(std::vector<mapTileIds::mapTileId>());
			for (int y = 0; y < height; y++) {
				map[x].push_back(canvas);
			}
		}

		return map;

	}

	std::vector<std::vector<mapTileIds::mapTileId>> MapBuilder::copyMap(std::vector<std::vector<mapTileIds::mapTileId>>& map) {
		std::vector<std::vector<mapTileIds::mapTileId>> mapCopy = std::vector<std::vector<mapTileIds::mapTileId>>();
		for (int x = 0; x < map.size(); x++) {
			mapCopy.push_back(std::vector<mapTileIds::mapTileId>());
			for (int y = 0; y < map[x].size(); y++) {
				mapCopy[x].push_back(map[x][y]);
			}
		}
		return mapCopy;
	}

	int MapBuilder::drawLine(std::vector<std::vector<mapTileIds::mapTileId>>& map, std::vector<df::Vector>& line, mapTileIds::mapTileId type) {
		if (line.empty()||map.empty()) {
			return -1;
		}

		df::Vector point;
		for (int pointIndex = 0; pointIndex < line.size(); pointIndex++) {

			point = line[pointIndex];
			if ((((int)point.getX()) > map.size()) || (((int)point.getX()) < 0) || (((int)point.getY()) > map[((int)point.getX())].size()) || (((int)point.getY()) < 0)){
				return -1;
			}
			
			map[(int)point.getX()][(int)point.getY()] = type;
		}
		return 0;
	}

	int MapBuilder::drawSquare(std::vector<std::vector<mapTileIds::mapTileId>>& map, df::Box square, mapTileIds::mapTileId value) {
		if (map.empty()) {
			return -1;
		}
		int width = (int)square.getHorizontal();
		int height = (int)square.getVertical();
		int x = (int)square.getCorner().getX();
		int y = ((int)square.getCorner().getY());
		if ((width < 1) || (height < 1) || (x < 0) || (y < 0)) {
			return -1;
		}

		if (x + width - 1 > map.size()) {
			return -1;
		}

		for (int i = x; i < width + x; i++) {
			for (int j = y; j < y + height; j++) {
				if (map[i].size() < j) {
					return -1;
				}
				map[i][j] = value;
			}
		}
		
		return 0;

	}

	int MapBuilder::floodFill(std::vector<std::vector<mapTileIds::mapTileId>>& map, df::Vector location, mapTileIds::mapTileId value) {
		std::unordered_set<df::Vector> visited;



		return 0;
	}

	std::vector<df::Vector> MapBuilder::floodFillReturnCoords(std::vector<std::vector<int>>& map, df::Vector location, mapTileIds::mapTileId value);

	std::vector<df::Vector> MapBuilder::findOpenCoords(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId open);

	int MapBuilder::findOpenCoordsCount(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId open);

	std::vector<std::vector<df::Vector>> MapBuilder::findZones(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId open);

	int MapBuilder::findLargestZone(std::vector<std::vector<df::Vector>> zones);

	df::Vector MapBuilder::findClosestPointInOtherZone(df::Vector targetPoint, std::vector<df::Vector>& otherPoints);

	CoordinatePair MapBuilder::findSmallestConnectionLine(std::vector<df::Vector>& startZone, std::vector<std::vector<df::Vector>>& otherZones);

	int MapBuilder::sprinkleTrees(std::vector<std::vector<mapTileIds::mapTileId>>& map, int trees);

	int MapBuilder::sprinkleSeeds(std::vector<std::vector<mapTileIds::mapTileId>>& map, int seeds);

	int MapBuilder::placeOwl(std::vector<std::vector<mapTileIds::mapTileId>>& map);

	int MapBuilder::placeExit(std::vector<std::vector<int>>& map);

	df::ObjectList MapBuilder::buildMap(std::vector<std::vector<mapTileIds::mapTileId>>& map, df::Object* owl);

	df::Vector MapBuilder::findRandomOpenCoord(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId open);

	int MapBuilder::squarePlot(std::vector<std::vector<int>>& map, df::Vector location, mapTileIds::mapTileId value);
	
	int MapBuilder::calculateNeededOpenSpaces(int seeds);

	void MapBuilder::generateMap();




	int MapBuilder::startGenerateMap(MapGenConfig config, df::Object* owl){
		if (!m_generating) {
			m_genDone = false;
			m_generating = true;
			m_configObj = config;
			m_player = owl;
			m_genThread = &std::thread(generateMap, this);
			return 0;
			m_player = nullptr;
		}
		return -1;
		
	}

	bool MapBuilder::isMapGenFinished()const {
		return m_genDone;
	}




	MapBuilder::MapBuilder() {
		m_configObj = MapGenConfig();
		m_genDone = false;
		m_generating = false;
		m_needed_empty_spaces=0;
		m_mapReturn=df::ObjectList();
		m_timer = df::Clock();
	}
	MapBuilder::~MapBuilder() {
		m_genThread->join();
	}


	int MapBuilder::destroyMap(df::ObjectList map) {
		df::WorldManager& wm = df::WorldManager::getInstance();
		for (int index = 0; index < map.getCount(); index++) {
			wm.markForDelete(map[index]);
		}
	}

	int MapBuilder::eventHandler(df::Event* m_p) {

		if (m_p->getType().compare(df::STEP_EVENT)) {
			if (!m_done_sent) {
				if (!m_generating) {
					if (m_genDone) {
						m_genThread->join();
						m_genThread = nullptr;
						df::GameManager& gm = df::GameManager::getInstance();
						EventMapGenDone done = EventMapGenDone(m_mapReturn, m_genTime);
						gm.onEvent(&done);
						m_done_sent = true;
					}
				}
			}
		}
		return 0;
	}

	int MapBuilder::draw() {
		if (this->getVisible()) {
			df::DisplayManager& dm = df::DisplayManager::getInstance();
			if (m_genDone) {
				return dm.drawString(this->getPosition(), "map generation done!", df::CENTER_JUSTIFIED, df::WHITE);
			}
			else if (m_generating) {
				return dm.drawString(this->getPosition(), "generating map!", df::CENTER_JUSTIFIED, df::WHITE);
			}
			else {
				return dm.drawString(this->getPosition(), "waiting to generate!", df::CENTER_JUSTIFIED, df::WHITE);
			}
		}
	}
}