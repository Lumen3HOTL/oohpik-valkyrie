  #include "MapBuilder.h"
#include <algorithm>
#include <cmath>
#include <fstream> // TEMP-MAPGEN-DEBUG

//behold madness
namespace ookpik {
	// Ground objects are invisible and do nothing, but there is one per open tile,
	// and every object costs time each frame. Set to true to create them again.
	const bool CREATE_GROUND_OBJECTS = false;

	unsigned long long MapBuilder::getBuildTime() {
		unsigned long long temp = 0;
		if (!this->getDeleteMode()) {
			m_state_gate.lock();
			temp = m_buildTime;
			m_state_gate.unlock();
		}
		
		return temp;
	}
	void MapBuilder::setBuildTime(unsigned long long new_build_time) {
		if (!this->getDeleteMode()) {
			m_state_gate.lock();
			m_buildTime = new_build_time;
			m_state_gate.unlock();
		}
		
	}

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
				pathPoints.push_back(df::Vector(i,y1));
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

			int vectorError = xDistance / 2;

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
			int vectorError = yDistance / 2;

			while (currentY != y2) {
				pathPoints.push_back(df::Vector(currentX, currentY));

				vectorError -= xDistance;

				if (vectorError < 0) {
					currentX += stepX;

					vectorError += yDistance;
					pathPoints.push_back(df::Vector(currentX, currentY));
				}

				currentY += stepY;
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
				pathPoints.push_back(df::Vector( i,y1));
			}
			return pathPoints;
		}

		
		if (yFirst) {
			if (point1First) {
				for (int i = startY; i <= endY; i++) {
					pathPoints.push_back(df::Vector(x1, i));
				}
				for (int i = startX; i <= endX; i++) {
					pathPoints.push_back(df::Vector(i, y2));
				}
			}
			else {
				for (int i = startY; i <= endY; i++) {
					pathPoints.push_back(df::Vector(x2, i));
				}
				for (int i = startX; i <= endX; i++) {
					pathPoints.push_back(df::Vector(i, y1));
				}
			}
		}
		else {
			if (point1First) {
				for (int i = startX; i <= endX; i++) {
					pathPoints.push_back(df::Vector(i, y1));
				}
				for (int i = startY; i <= endY; i++) {
					pathPoints.push_back(df::Vector(x2, i));
				}
			}
			else {
				for (int i = startX; i <= endX; i++) {
					pathPoints.push_back(df::Vector(i, y2));
				}
				for (int i = startY; i <= endY; i++) {
					pathPoints.push_back(df::Vector(x1, i));
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
				map.at(x).push_back(canvas);
			}
		}

		return map;

	}

	std::vector<std::vector<mapTileIds::mapTileId>> MapBuilder::copyMap(std::vector<std::vector<mapTileIds::mapTileId>> map) {
		
		
		return map;
	}

	int MapBuilder::drawLine(std::vector<std::vector<mapTileIds::mapTileId>>& map, std::vector<df::Vector>& line, mapTileIds::mapTileId type) {
		if (map.empty()) {
			this->m_debug_harness->queueErrorMessage("drawLineError: error 0 map empty!");
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			
			
			return -1;
		}
		else if (line.empty()) {
			this->m_debug_harness->queueErrorMessage("drawLineError: error 1 line empty!");
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);


			return -1;
		}

		df::Vector point;
		for (int pointIndex = 0; pointIndex < line.size(); pointIndex++) {

			point = line.at(pointIndex);
			if ((((int)point.getX()) >= map.size()) || (((int)point.getX()) < 0) ){
				this->m_debug_harness->queueErrorMessage(std::string("drawLineError: error 2 line extends outside map!").append(" invalid point is x: ").append(std::to_string(point.getX())).append(" map size is: Width: ").append(std::to_string(map.size())).append("!"));
				this->setCurrentMode(GenerationStages::GENERATION_ERROR);
				return -1;
			}
			if (((((int)point.getY()) >= map.at(((int)point.getX())).size())) || (((int)point.getY()) < 0)) {
				this->m_debug_harness->queueErrorMessage(std::string("drawLineError: error 2 line extends outside map!").append(" invalid point is ").append(" y: ").append(std::to_string(point.getY())).append(" map size is: ").append(" height: ").append(std::to_string(map.at(((int)point.getX())).size())).append("!"));
				this->setCurrentMode(GenerationStages::GENERATION_ERROR);
				return -1;
			}
			
			map.at((int)point.getX()).at(((int)point.getY()))= type;
		}
		return 0;
	}

	int MapBuilder::drawRectangle(std::vector<std::vector<mapTileIds::mapTileId>>& map, df::Box square, mapTileIds::mapTileId value) {
		if (map.empty()) {
			this->m_debug_harness->queueErrorMessage("drawSquare: error 0 map empty!");
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return -1;
		}
		int width = (int)square.getHorizontal();
		int height = (int)square.getVertical();
		int x = (int)square.getCorner().getX();
		int y = ((int)square.getCorner().getY());
		if ((width < 1) || (height < 1) || (x < 0) || (y < 0)) {
			this->m_debug_harness->queueErrorMessage(std::string("drawSquare: error 1 invalid square dimensions or coordinates!").append(" values are: width:").append(std::to_string(width)).append(" height: ").append(std::to_string(height)).append(" x: ").append(std::to_string(x)).append(" y: ").append(std::to_string(y)).append("!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return -1;
		}

		if (x + width - 1 >= map.size()) {
			this->m_debug_harness->queueErrorMessage(std::string("drawSquare: error 2 invalid square size or coordinates!").append(" values are: max square x: ").append(std::to_string(x+width -1)).append(" map width: ").append(std::to_string(map.size())).append("!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return -1;
		}

		for (int i = x; i < width + x; i++) {
			for (int j = y; j < y + height; j++) {
				if (map.at(i).size() <= j) {
					this->m_debug_harness->queueErrorMessage(std::string("drawSquare: error 3 invalid square size or coordinates!").append(" values are: y:").append(std::to_string(j)).append(" map x: ").append(std::to_string(i)).append(" map height at map x: ").append(std::to_string(map.at(i).size())).append("!"));
					this->setCurrentMode(GenerationStages::GENERATION_ERROR);
					return -1;
				}
				map.at(i).at(j) = value;
			}
		}
		
		return 0;

	}

	int MapBuilder::floodFill(std::vector<std::vector<mapTileIds::mapTileId>>& map, df::Vector location, mapTileIds::mapTileId fillValue, mapTileIds::mapTileId emptyValue) {
		if (map.empty()) {
			this->m_debug_harness->queueErrorMessage("floodfill: error 0 map empty!");
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return -1;
		}
		
		std::unordered_set<unsigned long long> visited;

		std::queue<df::Vector> toVisit;

		toVisit.push(location);
		
		df::Vector currentCoord;
		df::Vector newCoord;

		while (!toVisit.empty()) {
			currentCoord = toVisit.front();
			toVisit.pop();
			if (!visited.contains(((((unsigned long long)(currentCoord.getX())) << 32) | ((unsigned long long)currentCoord.getY())))) {
				if ((((int)currentCoord.getX()) < 0) || (((int)currentCoord.getX()) >= map.size()) || (((int)currentCoord.getY()) < 0) || (((int)currentCoord.getY()) >= map.at(((int)currentCoord.getX())).size())) {
					this->m_debug_harness->queueErrorMessage("floodfill: error 1 point outside map!");
					this->setCurrentMode(GenerationStages::GENERATION_ERROR);
					return -1;
				}

				if (map.at(((int)currentCoord.getX())).at(((int)currentCoord.getY())) == emptyValue) {
					map.at(((int)currentCoord.getX())).at(((int)currentCoord.getY())) = fillValue;
					newCoord = currentCoord;
					newCoord.setX(((int)newCoord.getX()) - 1);
					if ((((int)newCoord.getX()) >= 0)&&(map.at(((int)newCoord.getX())).size()>((int)newCoord.getY()))&&map.at(((int)newCoord.getX())).at(((int)newCoord.getY()))==emptyValue) {
						toVisit.push(newCoord);
					}
					newCoord = currentCoord;
					newCoord.setX(((int)newCoord.getX()) + 1);
					if ((((int)newCoord.getX()) < map.size()) && (map.at(((int)newCoord.getX())).size() > ((int)newCoord.getY())) && map.at(((int)newCoord.getX())).at(((int)newCoord.getY())) == emptyValue) {
						toVisit.push(newCoord);
					}
					newCoord = currentCoord;
					newCoord.setY(((int)newCoord.getY()) - 1);
					if ((((int)newCoord.getY()) >= 0) && (map.at(((int)newCoord.getX())).size() > ((int)newCoord.getY())) && map.at(((int)newCoord.getX())).at(((int)newCoord.getY())) == emptyValue) {
						toVisit.push(newCoord);
					}
					newCoord = currentCoord;
					newCoord.setY(((int)newCoord.getY()) + 1);
					if ((((int)newCoord.getY()) < map.at(((int)newCoord.getX())).size()) && (map.at(((int)newCoord.getX())).size() > ((int)newCoord.getY())) && map.at(((int)newCoord.getX())).at(((int)newCoord.getY())) == emptyValue) {
						toVisit.push(newCoord);
					}
				}

				visited.insert(((((unsigned long long)(currentCoord.getX())) << 32) | ((unsigned long long)currentCoord.getY())));
			}
			
		}



		return 0;
	}

	std::vector<df::Vector> MapBuilder::floodFillReturnCoords(std::vector<std::vector<mapTileIds::mapTileId>>& map, df::Vector location, mapTileIds::mapTileId fillValue, mapTileIds::mapTileId emptyValue) {
		if (map.empty()) {
			this->m_debug_harness->queueErrorMessage("floodfillReturnCoords: error 0 map empty!");
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return std::vector<df::Vector>();
		}
		else if (fillValue == emptyValue) {
			this->m_debug_harness->queueErrorMessage(std::string("floodfillReturnCoords: error 1 fill value is equal to emtpy value! fill and empty value: ").append(std::to_string((int)fillValue)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return std::vector<df::Vector>();
		}

		std::unordered_set<unsigned long long> visited;

		std::queue<df::Vector> toVisit;

		std::vector<df::Vector> foundCoords;

		toVisit.push(location);

		df::Vector currentCoord;
		df::Vector newCoord;

		while (!toVisit.empty()) {
			currentCoord = toVisit.front();
			toVisit.pop();
			if (!visited.contains(((((unsigned long long)(currentCoord.getX())) << 32) | ((unsigned long long)currentCoord.getY())))) {
				if ((((int)currentCoord.getX()) < 0) || (((int)currentCoord.getX()) >= map.size()) || (((int)currentCoord.getY()) < 0) || (((int)currentCoord.getY()) >= map.at(((int)currentCoord.getX())).size())) {
					this->m_debug_harness->queueErrorMessage("floodfillreturnCoords: error 1 point outside map!");
					this->setCurrentMode(GenerationStages::GENERATION_ERROR);
					
					return std::vector<df::Vector>();
				}

				if (map.at(((int)currentCoord.getX())).at(((int)currentCoord.getY())) == emptyValue) {
					map.at(((int)currentCoord.getX())).at(((int)currentCoord.getY())) = fillValue;
					foundCoords.push_back(currentCoord);
					newCoord = currentCoord;
					newCoord.setX(((int)newCoord.getX()) - 1);
					if ((((int)newCoord.getX()) >= 0) && (map.at(((int)newCoord.getX())).size() > ((int)newCoord.getY())) && map.at(((int)newCoord.getX())).at(((int)newCoord.getY())) == emptyValue) {
						toVisit.push(newCoord);
					}
					newCoord = currentCoord;
					newCoord.setX(((int)newCoord.getX()) + 1);
					if ((((int)newCoord.getX()) < map.size()) && (map.at(((int)newCoord.getX())).size() > ((int)newCoord.getY())) && map.at(((int)newCoord.getX())).at(((int)newCoord.getY())) == emptyValue) {
						toVisit.push(newCoord);
					}
					newCoord = currentCoord;
					newCoord.setY(((int)newCoord.getY()) - 1);
					if ((((int)newCoord.getY()) >= 0) && (map.at(((int)newCoord.getX())).size() >((int)newCoord.getY())) && map.at(((int)newCoord.getX())).at(((int)newCoord.getY())) == emptyValue) {
						toVisit.push(newCoord);
					}
					newCoord = currentCoord;
					newCoord.setY(((int)newCoord.getY()) + 1);
					if ((((int)newCoord.getY()) < map.at(((int)newCoord.getX())).size()) && (map.at(((int)newCoord.getX())).size() > ((int)newCoord.getY())) && map.at(((int)newCoord.getX())).at(((int)newCoord.getY())) == emptyValue) {
						toVisit.push(newCoord);
					}
				}

				visited.insert(((((unsigned long long)(currentCoord.getX())) << 32) | ((unsigned long long)currentCoord.getY())));
			}

		}



		return foundCoords;
	}

	std::vector<df::Vector> MapBuilder::findCoordsOfValue(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId type) {
		if (map.empty()) {
			this->m_debug_harness->queueErrorMessage("findOpenCoords: error 0 map empty!");
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return std::vector<df::Vector>();
		}

		std::vector<df::Vector> openCoords;

		for (int x = 0; x < map.size();x++) {
			if (map.at(x).empty()) {
				this->m_debug_harness->queueErrorMessage("FindOpenCoords: error 1 map collumn empty!");
				this->setCurrentMode(GenerationStages::GENERATION_ERROR);
				return std::vector<df::Vector>();
			}
			for (int y = 0; y < map.at(x).size(); y++) {
				if (map.at(x).at(y) == type) {
					openCoords.push_back(df::Vector(x, y));
				}
			
			}
		}
		return openCoords;
	}

	int MapBuilder::findCoordsOfValueCount(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId type) {
		if (map.empty()) {
			this->m_debug_harness->queueErrorMessage("FindOpencoordsCount: error 0 map empty!");
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return -1;
		}
		int openCount = 0;
		for (int x = 0; x < map.size();x++) {
			if (map.at(x).empty()) {
				this->m_debug_harness->queueErrorMessage(std::string("FindOpenCoordsCount: error 1 map collumn empty!").append(" empty y at x: ").append(std::to_string(x)).append("!"));
				this->setCurrentMode(GenerationStages::GENERATION_ERROR);

				return -1;
			}
			for (int y = 0; y < map.at(x).size(); y++) {
				if (map.at(x).at(y) == type) {
					openCount++;
				}

			}
		}
		return openCount;
	}

	std::vector<std::vector<df::Vector>> MapBuilder::findZones(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId flood, mapTileIds::mapTileId open) {
		if (map.empty()) {
			this->m_debug_harness->queueErrorMessage("FindZones: error 0 map empty!");
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return std::vector<std::vector<df::Vector>>();
		}

		std::vector<df::Vector> openCoords;

		std::vector<std::vector<mapTileIds::mapTileId>> tempMap=this->copyMap(map);

		std::vector<std::vector<df::Vector>> zones;

		std::vector<df::Vector> currentZone;

		df::Vector floodSeed;

		

		openCoords = this->findCoordsOfValue(tempMap,open);

		if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
			this->m_debug_harness->queueErrorMessage("FindZones: error 1 find open coords error!");
			return std::vector<std::vector<df::Vector>>();
		}


		while (!openCoords.empty()) {
			floodSeed = openCoords.at(0);
			currentZone = this->floodFillReturnCoords(tempMap, floodSeed, mapTileIds::FLOOD,open);
			if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
				this->m_debug_harness->queueErrorMessage("FindZones: error 2 flood fill failure!");
				return std::vector<std::vector<df::Vector>>();
			}
			zones.push_back(currentZone);
			currentZone.clear();
			openCoords = this->findCoordsOfValue(tempMap, open);

			if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
				this->m_debug_harness->queueErrorMessage("FindZones: error 3 find open coords error!");
				return std::vector<std::vector<df::Vector>>();
			}
		}

		return zones;
	}

	int MapBuilder::findLargestZone(std::vector<std::vector<df::Vector>>& zones) {
		if (zones.empty()) {
			this->m_debug_harness->queueErrorMessage("findLargestZone: error 0 empty zones list!");
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			
			return -1;
		}
		int biggest = 0;
		int biggestZone = -1;
		for (int i = 0; i < zones.size(); i++) {
			if (zones.at(i).size() > biggest) {
				biggest = zones.at(i).size();
				biggestZone = i;
			}
		}

		if ((biggest <= 0 ) || (biggestZone == -1)) {
			this->m_debug_harness->queueErrorMessage("findLargestZone: error 1 all zones empty!");
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			
			return -1;
		}

		return biggestZone;
	}

	float MapBuilder::findDistance(df::Vector p0, df::Vector p1) {
		float precursor = ((p1.getX()-p0.getX()) * (p1.getX() - p0.getX()))+ ((p1.getY()-p0.getY()) * (p1.getY() - p0.getY()));
		
			
		
		return sqrtf(precursor);
	}


	df::Vector MapBuilder::findClosestPointInOtherZone(df::Vector targetPoint, std::vector<df::Vector>& otherPoints) {
		if (otherPoints.empty()) {
			this->m_debug_harness->queueErrorMessage(std::string("find closest point in other zone: error 0 empty other points vector!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return df::Vector();
		}

		df::Vector closestPoint = otherPoints.at(0);
		float shortestDistance = this->findDistance(targetPoint,closestPoint);
		float testDistance = 0;
		for (int i = 0; i < otherPoints.size(); i++) {
			testDistance = this->findDistance(targetPoint, otherPoints.at(i));
			if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
				this->m_debug_harness->queueErrorMessage(std::string("find closest point in other zone: error 1 failed distance calculation!"));
				return df::Vector();
			}
			if (testDistance < shortestDistance) {
				closestPoint = otherPoints.at(i);
				shortestDistance = testDistance;
			}
		}

		return closestPoint;
	}

	CoordinatePair MapBuilder::findSmallestConnectionLine(std::vector<df::Vector>& startZone, std::vector<std::vector<df::Vector>>& otherZones) {
		if (startZone.empty()) {
			this->m_debug_harness->queueErrorMessage(std::string("findSmallestConnectionLine: error 0 empty start zone!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return CoordinatePair();
		}
		else if (otherZones.empty()) {
			this->m_debug_harness->queueErrorMessage(std::string("findSmallestConnectionLine: error 1 empty other zones vector!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return CoordinatePair();
		}
		else if (otherZones.at(0).empty()) {
			this->m_debug_harness->queueErrorMessage(std::string("findSmallestConnectionLine: error 2 empty other zone vector 0!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return CoordinatePair();
		}

		
		df::Vector overallShortestStartZonePoint=startZone.at(0);
		df::Vector overallShortestOtherZonePoint=otherZones.at(0).at(0);
		float overallShortestDistance = this->findDistance(overallShortestStartZonePoint,overallShortestOtherZonePoint);

		df::Vector startCheckPoint;
		df::Vector endCheckPoint;
		float checkDistance=0;
		int currentZone=0;
		for (int start = 0; start < startZone.size(); start++) {
			startCheckPoint = startZone.at(start);
			for (int zone = 0; zone < otherZones.size(); zone++) {
				currentZone = zone;
				if (otherZones.at(zone).empty()) {
					this->m_debug_harness->queueErrorMessage(std::string("findSmallestConnectionLine: error 2 empty other zone!").append(" other zones index: ").append(std::to_string(zone)).append("!"));
					this->setCurrentMode(GenerationStages::GENERATION_ERROR);
					return CoordinatePair();
				}
				
				endCheckPoint = this->findClosestPointInOtherZone(startCheckPoint,otherZones.at(zone));
				if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
					this->m_debug_harness->queueErrorMessage(std::string("findSmallestConnectionLine: error 3 closest point search failed!"));
					return CoordinatePair();
				}

				checkDistance = this->findDistance(startCheckPoint, endCheckPoint);
				if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
					this->m_debug_harness->queueErrorMessage(std::string("findSmallestConnectionLine: error 4 failed distance calculation!"));
					return CoordinatePair();
				}
				if (checkDistance < overallShortestDistance) {
					overallShortestStartZonePoint = startCheckPoint;
					overallShortestOtherZonePoint = endCheckPoint;
					overallShortestDistance = checkDistance;
				}
				
			}
		}

		return CoordinatePair(overallShortestStartZonePoint, overallShortestOtherZonePoint);

	}

	int MapBuilder::eliminateDisperateZones(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId flood, mapTileIds::mapTileId empty, MapGenConfig configObj) {
		if (map.empty()) {
			this->m_debug_harness->queueErrorMessage(std::string("eliminateDisperateZones: error 0 empty map!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return -1;
		}
		df::Clock timer;
		timer.delta();

		std::vector<std::vector<df::Vector>> zones=this->findZones(map,flood,empty);
		// No zones means nothing open was found; report it rather than indexing an empty list
		if (zones.empty()) {
			this->m_debug_harness->queueErrorMessage(std::string("eliminateDisperateZones: error 5 no open zones found!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return -1;
		}
		std::vector<df::Vector> startZone = zones.at(zones.size() - 1);
		zones.pop_back();
		std::vector<df::Vector> fixLine;
		
		
		if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
			this->m_debug_harness->queueErrorMessage(std::string("eliminateDisperateZones: error 1 zone search failed!"));
			return -1;
		}
		int timeout = configObj.getTimeoutSeconds();

		int selection = 0;

		bool yFirst = false;
		bool point1first = false;


		int topLeftx = 0;
		int topLefty = 0;

		int repairWidth = 0;
		int repairHeight = 0;
		df::Box repairBox;
		df::Vector testPoint;

		CoordinatePair fixPoints;
		while ((zones.size() > 0)) {
			if ((timer.split() / 1000) >= (33 * timeout * 30)) {
				this->m_debug_harness->queueErrorMessage(std::string("eliminateDisperateZones: error 3 process timeout!"));
				this->setCurrentMode(GenerationStages::GENERATION_ERROR);
				return -1;
			}

			fixPoints = this->findSmallestConnectionLine(startZone, zones);
			if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
				this->m_debug_harness->queueErrorMessage(std::string("eliminateDisperateZones: error 4 connection line search failed!"));
				return -1;
			}
			testPoint = this->getXYDistanceBetweenTwoPoint(fixPoints.getPoint0(), fixPoints.getPoint1());
			if (this->getCurrentMode() == GenerationStages::GENERATION_ERROR) {
				this->m_debug_harness->queueErrorMessage(std::string("eliminateDisperateZones: error 5 zone search failed!"));
				return -1;
			}
			if (testPoint.getX()+testPoint.getY() > 7) {
				selection = this->getRandom(0, 1);
			}
			else {
				selection = this->getRandom(0, 2);
			}
			
			switch (selection) {
				case 0:
					fixLine = this->generateBresenhamLine(fixPoints.getPoint0().getX(), fixPoints.getPoint0().getY(), fixPoints.getPoint1().getX(), fixPoints.getPoint1().getY());
					this->drawLine(map, fixLine, empty);
					if (this->getCurrentMode() == GenerationStages::GENERATION_ERROR) {
						this->m_debug_harness->queueErrorMessage(std::string("eliminateDisperateZones: error 6 connection line draw failed!"));
						return -1;
					}
					break;
			case 1:
				yFirst = false;
				point1first = false;
				if (this->getRandom(0, 1) == 1) {
					yFirst = true;
				}
				if (this->getRandom(0, 1) == 1) {
					point1first = true;
				}
				fixLine = this->generateXYLine(fixPoints.getPoint0().getX(), fixPoints.getPoint0().getY(), fixPoints.getPoint1().getX(), fixPoints.getPoint1().getY(), yFirst, point1first);
				this->drawLine(map, fixLine, empty);
				if (this->getCurrentMode() == GenerationStages::GENERATION_ERROR) {
					this->m_debug_harness->queueErrorMessage(std::string("eliminateDisperateZones: error 6 connection line draw failed!"));
					return -1;
				}
				break;
			case 2:
				// The box starts at the smaller x and y of the two points, so it never extends past either point
				topLeftx = std::min(fixPoints.getPoint0().getX(), fixPoints.getPoint1().getX());
				repairWidth = std::abs(fixPoints.getPoint1().getX() - fixPoints.getPoint0().getX());
				topLefty = std::min(fixPoints.getPoint0().getY(), fixPoints.getPoint1().getY());
				repairHeight = std::abs(fixPoints.getPoint1().getY() - fixPoints.getPoint0().getY());
				if (repairWidth <= 0) {
					repairWidth = 1;
				}
				if (repairHeight <= 0) {
					repairHeight = 1;
				}
				repairBox = df::Box(df::Vector(topLeftx, topLefty), repairWidth, repairHeight);
				this->drawRectangle(map, repairBox, empty);
				
				if (this->getCurrentMode() == GenerationStages::GENERATION_ERROR) {
					this->m_debug_harness->queueErrorMessage(std::string("eliminateDisperateZones: error 6 connection box draw failed!"));
					return -1;
				}
				break;
			}
			
			
			
			zones = this->findZones(map, flood, empty);
			if (zones.empty()) {
				this->m_debug_harness->queueErrorMessage(std::string("eliminateDisperateZones: error 7 no open zones found after joining!"));
				this->setCurrentMode(GenerationStages::GENERATION_ERROR);
				return -1;
			}
			
			
			startZone = zones.at(zones.size() - 1);
			zones.pop_back();
			
		}
		return 0;
	}

	std::vector<df::Vector> MapBuilder::getRandomCoordListOfValue(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId open) {
		if (map.empty()) {
			this->m_debug_harness->queueErrorMessage(std::string("generateRandomOpenCoordList: error 0 empty map!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return std::vector<df::Vector>();
		}
		std::vector<df::Vector> openSpace = this->findCoordsOfValue(map, open);
		
		if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
			this->m_debug_harness->queueErrorMessage(std::string("generateRandomOpenCoordList: error 1 retreive empty space failed!"));
			return std::vector<df::Vector>();
		}
		if (openSpace.empty()) {
			this->m_debug_harness->queueErrorMessage(std::string("generateRandomOpenCoordList: error 2 no open space!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return std::vector<df::Vector>();
		}
		std::shuffle(openSpace.begin(), openSpace.end(), m_RandomEngine);

		return openSpace;
	}

	df::Vector MapBuilder::getRandomCoordOfValue(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId open) {
		if (map.empty()) {
			this->m_debug_harness->queueErrorMessage(std::string("generateRandomOpenCoord: error 0 empty map!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return df::Vector();
		}
		std::vector<df::Vector> openSpace = this->findCoordsOfValue(map, open);

		if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
			this->m_debug_harness->queueErrorMessage(std::string("generateRandomOpenCoord: error 1 retreive empty space failed!"));
			return df::Vector();
		}
		if (openSpace.empty()) {
			this->m_debug_harness->queueErrorMessage(std::string("generateRandomOpenCoord: error 2 no open space!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return df::Vector();
		}
		return openSpace.at(this->getRandom(0, openSpace.size()-1));
	}
	

	int MapBuilder::sprinkleTrees(std::vector<std::vector<mapTileIds::mapTileId>>& map, int trees, mapTileIds::mapTileId open, mapTileIds::mapTileId tree) {
		if (map.empty()) {
			this->m_debug_harness->queueErrorMessage(std::string("sprinkleTrees: error 0 empty map!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return -1;
		}
		
		std::vector<df::Vector> openSpace = this->getRandomCoordListOfValue(map, open);
		if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
			this->m_debug_harness->queueErrorMessage(std::string("sprinkleTrees: error 1 random open coord list genration failed!"));
			return -1;
		}

		if (trees > openSpace.size()) {
			this->m_debug_harness->queueErrorMessage(std::string("sprinkleTrees: error 2 too little open space for specified trees! values: open space: ").append(std::to_string(openSpace.size())).append(" trees: ").append(std::to_string(trees)).append("!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return -1;
		}


		for (int treeIndex = 0; treeIndex < trees; treeIndex++) {
			if ((((int)openSpace.at(treeIndex).getX()) >= map.size()) || (((int)openSpace.at(treeIndex).getX()) < 0)) {
				this->m_debug_harness->queueErrorMessage(std::string("sprinkleTrees: error 3 map x smaller than random open coord x or random open coord x less than zero! values: map x: ").append(std::to_string(map.size())).append(" coord x: ").append(std::to_string((int)openSpace.at(treeIndex).getX())).append("!"));

				this->setCurrentMode(GenerationStages::GENERATION_ERROR);
				return -1;
			}
			if (map.at((int)openSpace.at(treeIndex).getX()).size() <= (int)openSpace.at(treeIndex).getY()) {
				this->m_debug_harness->queueErrorMessage(std::string("sprinkleTrees: error 4 map y smaller than random open coord y! values: small y x:").append(std::to_string((int)openSpace.at(treeIndex).getX())).append(" map y size: ").append(std::to_string(map.at((int)openSpace.at(treeIndex).getX()).size())).append(" coord y: ").append(std::to_string((int)openSpace.at(treeIndex).getY())).append("!"));

				this->setCurrentMode(GenerationStages::GENERATION_ERROR);
				return -1;
			}
			map.at((int)openSpace.at(treeIndex).getX()).at((int)openSpace.at(treeIndex).getY()) = tree;
		}
		return 0;
	}

	int MapBuilder::sprinkleSeeds(std::vector<std::vector<mapTileIds::mapTileId>>& map, int seeds, mapTileIds::mapTileId open, mapTileIds::mapTileId seed){
		if (map.empty()) {
			this->m_debug_harness->queueErrorMessage(std::string("sprinkeSeeds: error 0 empty map!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return -1;
		}

		std::vector<df::Vector> openSpace = this->getRandomCoordListOfValue(map, open);
		if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
			this->m_debug_harness->queueErrorMessage(std::string("sprinkleSeeds: error 1 random open coord list genration failed!"));
			return -1;
		}

		if (seeds > openSpace.size()) {
			this->m_debug_harness->queueErrorMessage(std::string("sprinkleSeeds: error 2 too little open space for specified trees! values: open space: ").append(std::to_string(openSpace.size())).append(" seeds: ").append(std::to_string(seeds)).append("!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return -1;
		}


		for (int seedIndex = 0; seedIndex < seeds; seedIndex++) {
			if ((((int)openSpace.at(seedIndex).getX()) >= map.size())|| (((int)openSpace.at(seedIndex).getX()) < 0)) {
				this->m_debug_harness->queueErrorMessage(std::string("sprinkleSeeds: error 3 map x smaller than random open coord x or random open cooord x less than zero! values: map x: ").append(std::to_string(map.size())).append(" coord x: ").append(std::to_string((int)openSpace.at(seedIndex).getX())).append("!"));

				this->setCurrentMode(GenerationStages::GENERATION_ERROR);
				return -1;
			}
			if (map.at((int)openSpace.at(seedIndex).getX()).size() <= (int)openSpace.at(seedIndex).getY()) {
				this->m_debug_harness->queueErrorMessage(std::string("sprinkleSeeds: error 4 map y smaller than random open coord y! values: small y x:").append(std::to_string((int)openSpace.at(seedIndex).getX())).append(" map y size: ").append(std::to_string(map.at((int)openSpace.at(seedIndex).getX()).size())).append(" coord y: ").append(std::to_string((int)openSpace.at(seedIndex).getY())).append("!"));

				this->setCurrentMode(GenerationStages::GENERATION_ERROR);
				return -1;
			}
			map.at((int)openSpace.at(seedIndex).getX()).at((int)openSpace.at(seedIndex).getY()) = seed;
		}
		return 0;
	}

	int MapBuilder::placeOwl(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId open, mapTileIds::mapTileId owl) {

		if (map.empty()) {
			this->m_debug_harness->queueErrorMessage(std::string("placeOwl: error 0 empty map!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return -1;
		}

		df::Vector openPos = this->getRandomCoordOfValue(map,open);
		if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
			this->m_debug_harness->queueErrorMessage(std::string("placeOwl: error 1 open coord generation failed!"));
			return -1;
		}

		if ((((int)openPos.getX()) < 0) || (((int)openPos.getX()) >= map.size()) ) {
			this->m_debug_harness->queueErrorMessage(std::string("placeOwl: error 2 random open pos x invalid! values: pos x: ").append(std::to_string(((int)openPos.getX()))).append(" map x: ").append(std::to_string(map.size())).append("!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return -1;
		}
		if ((((int)openPos.getY()) < 0) || (((int)openPos.getY()) >= map.at(((int)openPos.getX())).size())) {
			this->m_debug_harness->queueErrorMessage(std::string("placeOwl: error 3 random open pos Y invalid! values: pos y: ").append(std::to_string(((int)openPos.getY()))).append(" map Y: ").append(std::to_string(map.at((int)openPos.getX()).size())).append("!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return -1;
		}


		map.at(((int)openPos.getX())).at((int)openPos.getY()) = owl;
		return 0;
	}

	int MapBuilder::placeExit(std::vector < std::vector < mapTileIds::mapTileId >> &map, mapTileIds::mapTileId open, mapTileIds::mapTileId exit) {
		if (map.empty()) {
			this->m_debug_harness->queueErrorMessage(std::string("placeExit: error 0 empty map!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return -1;
		}

		df::Vector openPos = this->getRandomCoordOfValue(map, open);
		if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
			this->m_debug_harness->queueErrorMessage(std::string("placeExit: error 1 open coord generation failed!"));
			return -1;
		}

		if ((((int)openPos.getX()) < 0) || (((int)openPos.getX()) >= map.size())) {
			this->m_debug_harness->queueErrorMessage(std::string("placeExit: error 2 random open pos x invalid! values: pos x: ").append(std::to_string(((int)openPos.getX()))).append(" map x: ").append(std::to_string(map.size())).append("!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return -1;
		}
		if ((((int)openPos.getY()) < 0) || (((int)openPos.getY()) >= map.at(((int)openPos.getX())).size())) {
			this->m_debug_harness->queueErrorMessage(std::string("placeExit: error 3 random open pos Y invalid! values: pos y: ").append(std::to_string(((int)openPos.getY()))).append(" map Y: ").append(std::to_string(map.at((int)openPos.getX()).size())).append("!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return -1;
		}


		map.at(((int)openPos.getX())).at((int)openPos.getY()) = exit;
		return 0;
	}

	

	

	int MapBuilder::squarePlot(std::vector<std::vector<mapTileIds::mapTileId>>& map, df::Vector location, mapTileIds::mapTileId value) {
		if (map.empty()) {
			this->m_debug_harness->queueErrorMessage(std::string("squarePlot: error 0 empty map!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return -1;
		}
		if ((((int)location.getX()) < 0) || (((int)location.getX()) >= map.size())) {
			this->m_debug_harness->queueErrorMessage(std::string("squarePlot: error 1 plot corner 0 x invalid! values: pos x: ").append(std::to_string(((int)location.getX()))).append(" map x: ").append(std::to_string(map.size())).append("!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return -1;
		}
		if ((((int)location.getY()) < 0) || (((int)location.getY()) >= map.at(((int)location.getX())).size())) {
			this->m_debug_harness->queueErrorMessage(std::string("squarePlot: error 2 corner 0 Y invalid! values: pos y: ").append(std::to_string(((int)location.getY()))).append(" map Y: ").append(std::to_string(map.at((int)location.getX()).size())).append("!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return -1;
		}
		if ((((int)location.getX())+1 >= map.size())) {
			this->m_debug_harness->queueErrorMessage(std::string("squarePlot: error 3 corner 1 x invalid! values: pos x: ").append(std::to_string(((int)location.getX()))).append(" map x: ").append(std::to_string(map.size())).append("!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return -1;
		}
		if ((((int)location.getY())+1 >= map.at(((int)location.getX())).size())) {
			this->m_debug_harness->queueErrorMessage(std::string("squarePlot: error 4 corner 1 Y invalid! values: pos y: ").append(std::to_string(((int)location.getY()))).append(" map Y: ").append(std::to_string(map.at((int)location.getX()).size())).append("!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return -1;
		}

		map.at(((int)location.getX())).at(((int)location.getY())) = value;
		map.at(((int)location.getX())+1).at(((int)location.getY())) = value;
		map.at(((int)location.getX())).at(((int)location.getY())+1) = value;
		map.at(((int)location.getX())+1).at(((int)location.getY())+1) = value;
		return 0;
	}
	
	int MapBuilder::calculateNeededOpenSpaces(int seeds) {
		return seeds + 2;
	}

	df::Vector MapBuilder::getXYDistanceBetweenTwoPoint(df::Vector p0, df::Vector p1) {
		return df::Vector(abs(p0.getX() - p1.getX()), abs(p0.getY() - p1.getY()));
	}


	CoordinatePair  MapBuilder::findAngleLineClosestToDistance(std::vector<std::vector<mapTileIds::mapTileId>>& map, int dist, mapTileIds::mapTileId pointType) {
		if (map.empty()) {

			this->m_debug_harness->queueErrorMessage(std::string("findAngleLineClosestToDistance: error 0 empty map!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return CoordinatePair();

		}

		if (dist < 1) {
			this->m_debug_harness->queueErrorMessage(std::string("findAngleLineClosestToDistance: error 1 invalid distance of: ").append(std::to_string(dist)).append("!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return CoordinatePair();
		}

		std::vector < df::Vector> avalablePoint = this->getRandomCoordListOfValue(map, pointType);
		if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
			this->m_debug_harness->queueErrorMessage(std::string("findAngleLineClosestToDistance: error 2 avalable point retreival failed!"));
			return CoordinatePair();
		}

		if (avalablePoint.size() < 2) {
			this->m_debug_harness->queueErrorMessage(std::string("findAngleLineClosestToDistance: error 4 too few avalable points! avalable points: ").append(std::to_string(avalablePoint.size())).append("!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return CoordinatePair();
		}

		df::Vector closestStartPoint0= avalablePoint.at(0);
		df::Vector closestStartPoint1= avalablePoint.at(1);

		df::Vector testPoint = avalablePoint.at(0);
		df::Vector testPoint2 = avalablePoint.at(1);

		df::Vector resultPoint= this->getXYDistanceBetweenTwoPoint(testPoint, testPoint2);

		float closestDistance = resultPoint.getX()+resultPoint.getY();

		float testDistance=0;

		for (int point0 = 0; point0 < avalablePoint.size(); point0++) {
			for (int point1 = 0; point1 < avalablePoint.size(); point1++) {
				testPoint = avalablePoint.at(point0);
				testPoint2 = avalablePoint.at(point1);
				if (testPoint != testPoint2) {
					resultPoint=this->getXYDistanceBetweenTwoPoint(testPoint, testPoint2);
					testDistance = resultPoint.getX() + resultPoint.getY();
					if (abs(testDistance - dist) < abs(closestDistance-dist)) {
						closestStartPoint0 = testPoint;
						closestStartPoint1 = testPoint2;
						closestDistance = testDistance;
					}

				}
			}
		}
		return CoordinatePair(closestStartPoint0, closestStartPoint1);
	}
	CoordinatePair  MapBuilder::findDiagLineClosestToDistance(std::vector<std::vector<mapTileIds::mapTileId>>& map, int dist, mapTileIds::mapTileId pointType) {
		if (map.empty()) {

			this->m_debug_harness->queueErrorMessage(std::string("findAngleLineClosestToDistance: error 0 empty map!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return CoordinatePair();

		}

		if (dist < 1) {
			this->m_debug_harness->queueErrorMessage(std::string("findAngleLineClosestToDistance: error 1 invalid distance of: ").append(std::to_string(dist)).append("!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return CoordinatePair();
		}

		std::vector < df::Vector> avalablePoint = this->getRandomCoordListOfValue(map, pointType);
		if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
			this->m_debug_harness->queueErrorMessage(std::string("findAngleLineClosestToDistance: error 2 avalable point retreival failed!"));
			return CoordinatePair();
		}

		if (avalablePoint.size() < 2) {
			this->m_debug_harness->queueErrorMessage(std::string("findAngleLineClosestToDistance: error 4 too few avalable points! avalable points: ").append(std::to_string(avalablePoint.size())).append("!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return CoordinatePair();
		}

		df::Vector closestStartPoint0= avalablePoint.at(0);
		df::Vector closestStartPoint1= avalablePoint.at(1);

		df::Vector testPoint;
		df::Vector testPoint2;

		float resultdist;

		float closestDistance;
		testPoint = avalablePoint.at(0);
		testPoint2 = avalablePoint.at(1);
		closestStartPoint0 = testPoint;
		closestStartPoint1 = testPoint2;
		closestDistance = this->findDistance(testPoint, testPoint2);
		if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
			this->m_debug_harness->queueErrorMessage(std::string("findAngleLineClosestToDistance: error 5  distance retreival failed!"));
			
			return CoordinatePair();
		}

		for (int point0 = 0; point0 < avalablePoint.size(); point0++) {
			for (int point1 = 0; point1 < avalablePoint.size(); point1++) {
				testPoint = avalablePoint.at(point0);
				testPoint2 = avalablePoint.at(point1);
				if (testPoint != testPoint2) {
					resultdist = this->findDistance(testPoint, testPoint2);
					if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
						this->m_debug_harness->queueErrorMessage(std::string("findAngleLineClosestToDistance: error 5  distance retreival failed!"));

						return CoordinatePair();
					}
					
					if (abs(resultdist - dist) < abs(closestDistance - dist)) {
						closestStartPoint0 = testPoint;
						closestStartPoint1 = testPoint2;
						closestDistance = resultdist;
					}

				}
			}
		}
		return CoordinatePair(closestStartPoint0, closestStartPoint1);
	}


	void MapBuilder::resetTimer() {
		if (!this->getDeleteMode()) {
			m_state_gate.lock();
			m_timer.delta();
			m_state_gate.unlock();
		}
		
	}
	bool MapBuilder::checkTimer() {
		bool temp = false;
		if (!this->getDeleteMode()) {
			m_state_gate.lock();
			temp= ((m_timer.split() / 1000) >= (m_timeout * 33 * 30));
			m_state_gate.unlock();
		}
		
		return temp;
	
	}
	unsigned long long MapBuilder::getTimerTime() {
		unsigned long long temp = 0;
		if (!this->getDeleteMode()) {
			m_state_gate.lock();
			temp = m_timer.split();
			m_state_gate.unlock();
		}
		
		return temp;
	}
	int MapBuilder::getTimeout() {
		int temp = 0;
		if (!this->getDeleteMode()) {
			m_state_gate.lock();
			temp = m_timeout;
			m_state_gate.unlock();
		}
		
		return temp;
	}
	int MapBuilder::setTimeout(int new_timeout) {
		if (new_timeout < 1) {

			return -1;
		}
		if (!this->getDeleteMode()) {
			m_state_gate.lock();
			m_timeout = new_timeout;
			m_state_gate.unlock();
		}
		
		return 0;
	}



	int MapBuilder::ensureSpace(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId empty, mapTileIds::mapTileId tree,  int neededOpenSpaces) {
		if (map.empty()) {
			
				this->m_debug_harness->queueErrorMessage(std::string("ensureSpace: error 0 empty map!"));
				this->setCurrentMode(GenerationStages::GENERATION_ERROR);
				return -1;
			
		}



		int currentOpen = this->findCoordsOfValueCount(map, empty);
		if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
			this->m_debug_harness->queueErrorMessage(std::string("ensureSpace: error 1 open count retreival failed!"));
			return -1;
		}
		if (neededOpenSpaces < 3) {
			this->m_debug_harness->queueErrorMessage(std::string("ensureSpace: error 2 invalid meeded open of: ").append(std::to_string(neededOpenSpaces)).append("!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			return -1;
		}

		if (currentOpen >= neededOpenSpaces) {
			return 0;
		}

		int mode = 0;

		CoordinatePair targetPair;
		std::vector<df::Vector> ClosedCoordsList;
		std::vector<df::Vector> repairSpaces;

		df::Box repairbox;

		int startx = 0;
		int endx = 0;
		int starty = 0;
		int endy = 0;
		int width = 0;
		int height = 0;
		int temp = 0;
		while (currentOpen < neededOpenSpaces) {

			
			ClosedCoordsList= this->getRandomCoordListOfValue(map,tree);
			if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
				this->m_debug_harness->queueErrorMessage(std::string("ensureSpace: error 3 closed coords retreival failed!"));
				return -1;
			}
			if (ClosedCoordsList.empty()) {
				this->m_debug_harness->queueErrorMessage(std::string("ensureSpace: error 4 empty closed spaces list!"));
				this->setCurrentMode(GenerationStages::GENERATION_ERROR);
				return -1;
			}
			if (ClosedCoordsList.size()<neededOpenSpaces-currentOpen) {
				this->m_debug_harness->queueErrorMessage(std::string("ensureSpace: error 5 too small closed spaces list! size: ").append(std::to_string(ClosedCoordsList.size())).append(" needed open: ").append(std::to_string(neededOpenSpaces-currentOpen)).append("!"));
				this->setCurrentMode(GenerationStages::GENERATION_ERROR);
				return -1;
			}
			bool yFirst = false;
			bool point1First = false;
			int area = 0;
			int addTrees = 0;
			mode = this->getRandom(0,4);
			df::Vector closeCoord;
			int fatCount = 0;
			switch (mode) {
				case 0:
					//line mode
					targetPair = this->findDiagLineClosestToDistance(map, neededOpenSpaces-currentOpen, empty);
					if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
						this->m_debug_harness->queueErrorMessage(std::string("ensureSpace: error 6 draw points retreival failed!"));


						return -1;
					}
					repairSpaces = this->generateBresenhamLine(targetPair.getPoint0().getX(), targetPair.getPoint0().getY(), targetPair.getPoint1().getX(), targetPair.getPoint1().getY());
					this->drawLine(map, repairSpaces, empty);
					if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
						this->m_debug_harness->queueErrorMessage(std::string("ensureSpace: error 7 draw failed!"));


						return -1;
					}

					break;
				case 1:
					//xy line mode
					targetPair = this->findAngleLineClosestToDistance(map, neededOpenSpaces - currentOpen, empty);
					if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
						this->m_debug_harness->queueErrorMessage(std::string("ensureSpace: error 8 draw points retreival failed!"));


						return -1;
					}
					yFirst = false;
					point1First = false;
					if ((this->getRandom(0,1))==0) {
						yFirst = true;
					}
					if ((this->getRandom(0, 1)) == 0) {
						point1First = true;
					}
					repairSpaces = this->generateXYLine(targetPair.getPoint0().getX(), targetPair.getPoint0().getY(), targetPair.getPoint1().getX(), targetPair.getPoint1().getY(), yFirst, point1First);
					this->drawLine(map, repairSpaces, empty);
					if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
						this->m_debug_harness->queueErrorMessage(std::string("ensureSpace: error 9 draw failed!"));


						return -1;
					}
					break;

				case 2:
					///rect mode
					targetPair = this->findAngleLineClosestToDistance(map, neededOpenSpaces - currentOpen, empty);
					if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
						this->m_debug_harness->queueErrorMessage(std::string("ensureSpace: error 10 draw points retreival failed!"));


						return -1;
					}
					startx = targetPair.getPoint0().getX();
					endx = targetPair.getPoint1().getX();
					starty = targetPair.getPoint0().getY();
					endy = targetPair.getPoint1().getY();
					width = 0;
					height = 0;
					temp = 0;
					if (startx > endx) {
						temp = startx;
						startx = endx;
						endx = temp;
						
					}
					width = endx - startx;
					if (starty > endy) {
						temp = starty;
						starty = endy;
						endy = temp;
						
					}
					height = endy - starty;
					repairbox = df::Box(df::Vector(startx, starty), width, height);

					this->drawRectangle(map, repairbox, empty);

					if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
						this->m_debug_harness->queueErrorMessage(std::string("ensureSpace: error 11 draw failed!"));


						return -1;
					}
					 area = width * height;
					 addTrees = area - (neededOpenSpaces - currentOpen);

					this->sprinkleTrees(map, addTrees, empty, tree);
					if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
						this->m_debug_harness->queueErrorMessage(std::string("ensureSpace: error 12 draw failed!"));


						return -1;
					}
					break;
				case 3:
					//random point mode;
					for (int i = 0; i < neededOpenSpaces-currentOpen; i++) {
						closeCoord = ClosedCoordsList.at(i);
						if ((closeCoord.getX() < 0) || (closeCoord.getX() >= map.size())) {
							this->m_debug_harness->queueErrorMessage(std::string("ensureSpace: error 13 invalid target coord x: ").append(std::to_string(closeCoord.getX())).append(" map width: ").append(std::to_string(map.size())).append("!"));
							this->setCurrentMode(GenerationStages::GENERATION_ERROR);
							return -1;
						}
						if ((closeCoord.getY() < 0) || (closeCoord.getY() >= map.at((int)closeCoord.getX()).size())) {
							this->m_debug_harness->queueErrorMessage(std::string("ensureSpace: error 14 invalid target coord y: ").append(std::to_string(closeCoord.getY())).append(" map width: ").append(std::to_string(map.at((int)closeCoord.getX()).size())).append("!"));
							this->setCurrentMode(GenerationStages::GENERATION_ERROR);
							return -1;
						}
						map.at((int)closeCoord.getX()).at((int)closeCoord.getY()) = empty;
						
					}
					break;
				case 4:
					//random fat point mode;
					fatCount = (neededOpenSpaces - currentOpen) / 4;
					if (fatCount <= 0) {
						fatCount = 1;
					}
					for (int i = 0; i < fatCount; i++) {
						closeCoord = ClosedCoordsList.at(i);

						if ((closeCoord.getX() < 0) || (closeCoord.getX() >= map.size())) {
							this->m_debug_harness->queueErrorMessage(std::string("ensureSpace: error 13 invalid target coord x: ").append(std::to_string(closeCoord.getX())).append(" map width: ").append(std::to_string(map.size())).append("!"));
							this->setCurrentMode(GenerationStages::GENERATION_ERROR);
							return -1;
						}
						if ((closeCoord.getY() < 0) || (closeCoord.getY() >= map.at((int)closeCoord.getX()).size())) {
							this->m_debug_harness->queueErrorMessage(std::string("ensureSpace: error 14 invalid target coord y: ").append(std::to_string(closeCoord.getY())).append(" map width: ").append(std::to_string(map.at((int)closeCoord.getX()).size())).append("!"));
							this->setCurrentMode(GenerationStages::GENERATION_ERROR);
							return -1;
						}
						if (!(((closeCoord.getX() < 0) || (closeCoord.getX() >= ((int)map.size())-1))|| ((closeCoord.getY() < 0) || (closeCoord.getY() >= ((int)map.at((int)closeCoord.getX()).size()) - 1)))) {
							this->squarePlot(map, closeCoord,empty);
							if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
								this->m_debug_harness->queueErrorMessage(std::string("ensureSpace: error 15 square plot failrue!"));
								this->setCurrentMode(GenerationStages::GENERATION_ERROR);
								return -1;
							}
						}
						
							
						

					}
					break;

				default:
					this->m_debug_harness->queueErrorMessage(std::string("ensureSpace: error 16 invalid random repair mode!"));
					this->setCurrentMode(GenerationStages::GENERATION_ERROR);
					return -1;
			}

			currentOpen = this->findCoordsOfValueCount(map, empty);

			if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
				this->m_debug_harness->queueErrorMessage(std::string("ensureSpace: error 17 open count retreival failed!"));
				return -1;
			}
		}

		return 0;
	}
	//inclusve of both limits
	int MapBuilder::getRandom(int min, int max) {
		std::uniform_int_distribution<int> dist(min, max);
		return dist(m_RandomEngine);
	
	}



	void MapBuilder::generateMap() {

		
		if (m_configObj.getRandomSeed() != 0) {
			m_RandomEngine = std::mt19937(m_configObj.getRandomSeed());
			
		}
		else {
			m_configObj.setRandomSeed(std::chrono::high_resolution_clock().now().time_since_epoch().count());
			m_RandomEngine = std::mt19937(m_configObj.getRandomSeed());
		}



		int errorNumber = 0;
		errorNumber++;
		int timeout = this->m_configObj.getTimeoutSeconds();
		if (timeout < 1) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured timeout. must be at least 1 second. value is: ").append(std::to_string(timeout)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		this->setTimeout(timeout);
		this->resetTimer();
		int mapBorderThickness = this->m_configObj.getMapBorderThickness();
		if (mapBorderThickness < 1) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append("invalid configured map border thickeness.must be at least 1. value is : ").append(std::to_string(mapBorderThickness)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		int mapHeight = this->m_configObj.getMapHeight();
		if (mapHeight < 1) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured map height. must be at least 1. value is: ").append(std::to_string(mapHeight)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		int mapObjectAltitude = this->m_configObj.getMapObjectAltitude();
		if ((mapObjectAltitude < 0 )||(mapObjectAltitude > df::MAX_ALTITUDE)) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured map object altitude. must be between 0 and ").append(std::to_string(df::MAX_ALTITUDE)).append(". value is: ").append(std::to_string(mapObjectAltitude)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		int mapWidth=this->m_configObj.getMapWidth();
		if (mapWidth < 1) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured map width. must be at least 1. value is: ").append(std::to_string(mapWidth)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		int maxRightAngleLine = this->m_configObj.getMaxRightAngleLine();
		if (maxRightAngleLine < 0) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured max right angle line. must be at least 0. value is: ").append(std::to_string(maxRightAngleLine)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		int maxRightAngleLineHeight = this->m_configObj.getMaxAngleLineHeight();
		if (maxRightAngleLineHeight < 1) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured max right angle line height. must be at least 1. value is: ").append(std::to_string(maxRightAngleLineHeight)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		int maxRightAngleLineWidth = this->m_configObj.getMaxAngleLineWidth();
		if (maxRightAngleLineWidth < 1) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured max right angle line width. must be at least 1. value is: ").append(std::to_string(maxRightAngleLineWidth)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		int maxDiagLineHeight = this->m_configObj.getMaxDiagLineHeight();
		if (maxDiagLineHeight < 1) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured max diag line height. must be at least 1. value is: ").append(std::to_string(maxDiagLineHeight)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}

		errorNumber++;
		int maxDiagLines = this->m_configObj.getMaxDiagLines();
		if (maxDiagLines < 0) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured max diag lines. must be at least 0. value is: ").append(std::to_string(maxDiagLines)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}

		errorNumber++;
		int maxDiagLinesWidth = this->m_configObj.getMaxDiagLineWidth();
		if (maxDiagLinesWidth < 1) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured max diag line width. must be at least 1. value is: ").append(std::to_string(maxDiagLinesWidth)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}

		errorNumber++;
		int maxRandTrees = this->m_configObj.getMaxRandTrees();
		if (maxRandTrees < 0) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured max rand trees. must be at least 0. value is: ").append(std::to_string(maxRandTrees)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}

		errorNumber++;
		int maxRoomHeight = this->m_configObj.getMaxRoomHeight();
		if (maxRoomHeight < 1) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured room height. must be at least 1. value is: ").append(std::to_string(maxRoomHeight)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}

		errorNumber++;
		int maxRooms = this->m_configObj.getMaxRooms();
		if (maxRooms < 0) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured max rooms. must be at least 0. value is: ").append(std::to_string(maxRooms)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}

		errorNumber++;
		int maxRoomWidth = this->m_configObj.getMaxRoomWidth();
		if (maxRoomWidth < 1) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured max room width. must be at least 1. value is: ").append(std::to_string(maxRoomWidth)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}

		errorNumber++;
		int maxSeeds = this->m_configObj.getMaxSeeds();
		if (maxSeeds < 1) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured max seeds. must be at least 1. value is: ").append(std::to_string(maxSeeds)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}

		errorNumber++;
		int minRightAngleLines = this->m_configObj.getMinRightAngleLines();
		if (minRightAngleLines < 0) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured min angle lines. must be at least 0. value is: ").append(std::to_string(minRightAngleLines)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}


		errorNumber++;
		int minRightAngleLineHeight = this->m_configObj.getMinAngleLineHeight();
		if (minRightAngleLineHeight < 1) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured min right angle lines height. must be at least 1. value is: ").append(std::to_string(minRightAngleLineHeight)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}

		errorNumber++;
		int minRightAngleLinesWidth = this->m_configObj.getMinAngleLineWidth();
		if (minRightAngleLinesWidth < 1) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured min angle line width. must be at least 1. value is: ").append(std::to_string(minRightAngleLinesWidth)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}

		errorNumber++;
		int minDiagLineHeight = this->m_configObj.getMinDiagLineHeight();
		if (minDiagLineHeight < 1) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured min diag line height. must be at least 1. value is: ").append(std::to_string(minDiagLineHeight)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}

		errorNumber++;
		int minDiagLines = this->m_configObj.getMinDiagLines();
		if (minDiagLines < 0) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured min diag lines. must be at least 0. value is: ").append(std::to_string(minDiagLines)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}

		errorNumber++;
		int minDiagLineWidth = this->m_configObj.getMinDiagLineWidth();
		if (minDiagLineWidth < 1) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured min diag line width. must be at least 1. value is: ").append(std::to_string(minDiagLineWidth)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}

		errorNumber++;
		int minRandTrees = this->m_configObj.getMinRandTrees();
		if (minRandTrees < 0) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured min randTrees. must be at least 0. value is: ").append(std::to_string(minRandTrees)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}

		errorNumber++;
		int minRoomHeight = this->m_configObj.getMinRoomHeight();
		if (minRoomHeight < 1) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured min room height. must be at least 1. value is: ").append(std::to_string(minRoomHeight)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}

		errorNumber++;
		int minRooms = this->m_configObj.getMinRooms();
		if (minRooms < 0) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured min rooms. must be at least 0. value is: ").append(std::to_string(minRooms)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}

		errorNumber++;
		int minRoomsWidth = this->m_configObj.getMinRoomWidth();
		if (minRoomsWidth < 1) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured min room width. must be at least 1. value is: ").append(std::to_string(minRoomsWidth)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}

		errorNumber++;
		int minSeeds = this->m_configObj.getMinSeeds();
		if (minSeeds < 1) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured min seeds. must be at least 1. value is: ").append(std::to_string(minSeeds)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		if (this->checkTimer()) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" generation timeout! timer setting: ").append(std::to_string(this->getTimeout())).append(" seconds, timer elapsed: ").append(std::to_string(((double)((double)((double)(this->getTimerTime()/1000))/33)/30))).append(" seconds!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}




		
		
		int mapArea = mapHeight * mapWidth;

		int neededSeeds = 0;




		
		errorNumber++;
		if (minSeeds > maxSeeds) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured seeds. min seeds must be less than max seeds. min seeds is: ").append(std::to_string(minSeeds)).append(" max seeds is: ").append(std::to_string(maxSeeds)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		// Every other min/max pair must be in order too: getRandom() can't pick from a backwards range
		const struct { const char* name; int min; int max; } ranges[] = {
			{ "rooms", minRooms, maxRooms },
			{ "room width", minRoomsWidth, maxRoomWidth },
			{ "room height", minRoomHeight, maxRoomHeight },
			{ "random trees", minRandTrees, maxRandTrees },
			{ "right angle lines", minRightAngleLines, maxRightAngleLine },
			{ "right angle line width", minRightAngleLinesWidth, maxRightAngleLineWidth },
			{ "right angle line height", minRightAngleLineHeight, maxRightAngleLineHeight },
			{ "diag lines", minDiagLines, maxDiagLines },
			{ "diag line width", minDiagLineWidth, maxDiagLinesWidth },
			{ "diag line height", minDiagLineHeight, maxDiagLineHeight },
		};
		for (const auto& range : ranges) {
			if (range.min > range.max) {
				this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured ").append(range.name).append(". min must not be more than max. min is: ").append(std::to_string(range.min)).append(" max is: ").append(std::to_string(range.max)));
				this->setCurrentMode(GenerationStages::GENERATION_ERROR);
				this->setBaseFunctionExit(true);
				return;
			}
		}
		errorNumber++;
		if (minSeeds + 2 >= mapArea) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured seeds. must be less than map area -2. min seeds is: ").append(std::to_string(minSeeds)).append(" map area is: ").append(std::to_string(mapArea)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		if (maxSeeds + 2 >= mapArea) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured seeds. must be less than map area -2. max seeds is: ").append(std::to_string(maxSeeds)).append(" map area is: ").append(std::to_string(mapArea)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		if (minRandTrees > (mapArea-2) - maxSeeds) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured random trees. min random trees must be less than map area-maxSeeds-2. map area is: ").append(std::to_string(mapArea)).append(" max seeds is: ").append(std::to_string(maxSeeds)).append(" min rand trees is: ").append(std::to_string(minRandTrees)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		if (maxRandTrees > (mapArea - 2) - maxSeeds) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured random trees. max random trees must be less than map area-maxSeeds-2. map area is: ").append(std::to_string(mapArea)).append(" max seeds is: ").append(std::to_string(maxSeeds)).append(" max rand trees is: ").append(std::to_string(maxRandTrees)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}


		errorNumber++;
		if (minRightAngleLines > maxRightAngleLine) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured right angle lines. min right angle lines  must be less than max right angle lines. min right angle lines  is: ").append(std::to_string(minRightAngleLines)).append(" max right angle lines is: ").append(std::to_string(maxRightAngleLine)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		if (minRightAngleLinesWidth > maxRightAngleLineWidth) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured right angle lines width. min angle lines width must be less than max angle lines width. min right angle lines width is: ").append(std::to_string(minRightAngleLinesWidth)).append(" max right angle line width is: ").append(std::to_string(maxRightAngleLineWidth)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		if (maxRightAngleLineWidth >= mapWidth) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured max right angle line width.max right angle line width must be less than map width. max right angle lines width is: ").append(std::to_string(maxRightAngleLineWidth)).append(" map width is: ").append(std::to_string(mapWidth)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		if (minRightAngleLinesWidth >= mapWidth) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured right agnle lines width. min right angle lines width must be less than map width. min right angle lines width is: ").append(std::to_string(minRightAngleLinesWidth)).append(" map width is: ").append(std::to_string(mapWidth)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		if (minRightAngleLineHeight > maxRightAngleLineHeight) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured angle lines height. min angle lines height must be less than max angle lines height. min right angle lines height is: ").append(std::to_string(minRightAngleLineHeight)).append(" max right angle line hieght is: ").append(std::to_string(maxRightAngleLineHeight)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		if (maxRightAngleLineHeight >= mapHeight) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured max right angle line hieght. must be less than map heigth. max right angle lines height is: ").append(std::to_string(maxRightAngleLineHeight)).append(" map hieght is: ").append(std::to_string(mapHeight)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		if (minRightAngleLineHeight >= mapHeight) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid min right angle line height. min right angle line height must be less than map height. min right angle line hieght is: ").append(std::to_string(minRightAngleLineHeight)).append(" map height is: ").append(std::to_string(mapHeight)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		
		errorNumber++;
		if (minDiagLines > maxDiagLines) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured daig lines. min daig lines  must be less than max daig lines. min diag lines  is: ").append(std::to_string(minDiagLines)).append(" max diag lines is: ").append(std::to_string(maxDiagLines)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		if (minDiagLineWidth > maxDiagLinesWidth) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured diag lines width. mindiag lines width must be less than maxdaig lines width. min daig lines width is: ").append(std::to_string(minDiagLineWidth)).append(" max diag line width is: ").append(std::to_string(maxDiagLinesWidth)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		if (minDiagLineWidth >= mapWidth) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured diag lines width. mindiag lines width must be less than map width. min daig lines width is: ").append(std::to_string(minDiagLineWidth)).append(" map width is: ").append(std::to_string(mapWidth)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		if (maxDiagLinesWidth >= mapWidth) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured max diag line width. must be less than map width. max daig lines width is: ").append(std::to_string(maxDiagLinesWidth)).append(" map width is: ").append(std::to_string(mapWidth)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		if (minDiagLineHeight > maxDiagLineHeight) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured diag lines height. min daig lines height must be less than max diag lines height. min daig lines height is: ").append(std::to_string(minDiagLineHeight)).append(" max diag line hieght is: ").append(std::to_string(maxDiagLineHeight)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		if (maxDiagLineHeight >= mapHeight) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured max diag line hieght. must be less than map heigth. max daig lines height is: ").append(std::to_string(maxDiagLineHeight)).append(" map hieght is: ").append(std::to_string(mapHeight)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		if (minDiagLineHeight >= mapHeight) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid min diag line height. min diag line height must be less than map height. min daig line hieght is: ").append(std::to_string(minDiagLineHeight)).append(" map height is: ").append(std::to_string(mapHeight)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}


		errorNumber++;
		if (minRooms > maxRooms) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured rooms lines. min rooms must be less thanmax rooms. min rooms is: ").append(std::to_string(minRooms)).append(" max rooms is: ").append(std::to_string(maxRooms)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		if (minRoomsWidth > maxRoomWidth) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured rooms width. min rooms width must be less thanmax rooms width. min room width is: ").append(std::to_string(minRoomsWidth)).append(" max roome width is: ").append(std::to_string(maxRoomWidth)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		if (minRoomsWidth >= mapWidth) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured min rooms width. min rooms width must be less than map width. min rooms width is: ").append(std::to_string(minRoomsWidth)).append(" map width is: ").append(std::to_string(mapWidth)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		if (maxRoomWidth >= mapWidth) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured max room width. must be less than map width. max rooms width is: ").append(std::to_string(maxRoomWidth)).append(" map width is: ").append(std::to_string(mapWidth)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		if (maxRoomHeight < minRoomHeight) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured room height. min room height must be less than max rooms height. min room height is: ").append(std::to_string(minRoomHeight)).append(" max room hieght is: ").append(std::to_string(maxRoomHeight)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		if (maxRoomHeight >= mapHeight) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid configured max room hieght. must be less than map heigth. max room height is: ").append(std::to_string(maxRoomHeight)).append(" map hieght is: ").append(std::to_string(mapHeight)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		if (minRoomHeight >= mapHeight) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid min room height. min room height must be less than map height. min room hieght is: ").append(std::to_string(minRoomHeight)).append(" map height is: ").append(std::to_string(mapHeight)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}


		errorNumber++;
		if (this->checkTimer()) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" generation timeout! timer setting: ").append(std::to_string(this->getTimeout())).append(" seconds, timer elapsed: ").append(std::to_string(((double)((double)((double)(this->getTimerTime() / 1000)) / 33) / 30))).append(" seconds!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		

		if (minSeeds == maxSeeds) {
			neededSeeds = minSeeds;
		}
		else {
		
			neededSeeds = this->getRandom(minSeeds,maxSeeds);
		}

		int requestedDiagLines = 0;
		int requestedRooms = 0;
		int requestedRightAngleLines = 0;

		
		int neededOpen = this->calculateNeededOpenSpaces(neededSeeds);
		errorNumber++;
		if (this->checkTimer()) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" generation timeout! timer setting: ").append(std::to_string(this->getTimeout())).append(" seconds, timer elapsed: ").append(std::to_string(((double)((double)((double)(this->getTimerTime() / 1000)) / 33) / 30))).append(" seconds!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		
		if ((minDiagLines >= 0)&&(maxDiagLines > 0 )) {
			if (minDiagLines == maxDiagLines) {
				requestedDiagLines = minDiagLines;
			
			}
			else {
			
				requestedDiagLines = this->getRandom(minDiagLines,maxDiagLines);
			}
		}
		
		if ((minRightAngleLines >= 0) && (maxRightAngleLine > 0)) {
			
			if (minRightAngleLines == maxRightAngleLine) {
				requestedRightAngleLines = minRightAngleLines;
			}
			else {
				
				requestedRightAngleLines = this->getRandom(minRightAngleLines,maxRightAngleLine);
			}
		}

		if ((minRooms >= 0) && (maxRooms > 0)) {
			
			if (minRooms == maxRooms) {
				requestedRooms = minRooms;

			}
			else {
				
				requestedRooms = this->getRandom(minRooms,maxRooms);
			}
		}

		errorNumber++;
		if (requestedDiagLines + requestedRooms + requestedRightAngleLines <= 0) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" invalid open area. collective generated number of open structure (right angle lines, rooms, diag lines) must be greater than 0!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}

		std::vector<std::vector<mapTileIds::mapTileId>> protomap = this->createStartingMap(mapWidth, mapHeight, mapTileIds::TREE);
		errorNumber++;
		if (this->checkTimer()) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" generation timeout! timer setting: ").append(std::to_string(this->getTimeout())).append(" seconds, timer elapsed: ").append(std::to_string(((double)((double)((double)(this->getTimerTime() / 1000)) / 33) / 30))).append(" seconds!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		if (requestedRooms > 0) {
			
			
			int width = 0;
			int height = 0;
			int startx = 0;
			int starty = 0;
			for (int index = 0; index < requestedRooms; index++) {
				width = this->getRandom(minRoomsWidth, maxRoomWidth);
				height = this->getRandom(minRoomHeight, maxRoomHeight);
				startx = this->getRandom(0, ((mapWidth) - width));
				starty = (this->getRandom(0, ((mapHeight) - height)));
				df::Box room = df::Box(df::Vector(startx,starty),(width), (height));
				this->drawRectangle(protomap, room, mapTileIds::EMPTY);
				if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
					
					this->m_debug_harness->queueErrorMessage(std::string("generate map error: ").append(std::to_string(errorNumber)).append(" room draw failed! box number: ").append(std::to_string(index)).append(" out of ").append(std::to_string(requestedRooms)).append("x: ").append(std::to_string(startx)).append(" y: ").append(std::to_string(starty)).append(" width: ").append(std::to_string(width)).append("height").append(std::to_string(height)).append(" map height: ").append(std::to_string(mapHeight)).append(" map width:").append(std::to_string(mapWidth)));
					this->setBaseFunctionExit(true);
					return;
				}
			}
		}
		errorNumber++;
		if (this->checkTimer()) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" generation timeout! timer setting: ").append(std::to_string(this->getTimeout())).append(" seconds, timer elapsed: ").append(std::to_string(((double)((double)((double)(this->getTimerTime() / 1000)) / 33) / 30))).append(" seconds!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		if (requestedDiagLines > 0) {
			int width = 0;
			int height = 0;
			int startX = 0;
			int startY = 0;
			std::vector<df::Vector> toDraw;
			for (int index = 0; index < requestedDiagLines; index++) {
				width = this->getRandom(minDiagLineWidth,maxDiagLinesWidth);
				height = this->getRandom(minDiagLineHeight,maxDiagLineHeight);
				startX= this->getRandom(0,(mapWidth-1)-width);
				startY= this->getRandom(0, (mapHeight-1) - height);

				toDraw = this->generateBresenhamLine(startX, startY, startX + width, startY + height);

				this->drawLine(protomap, toDraw, mapTileIds::EMPTY);

				if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
					this->m_debug_harness->queueErrorMessage(std::string("generate map error: ").append(std::to_string(errorNumber)).append(" diag line draw failed! diag line number: ").append(std::to_string(index)).append(" out of ").append(std::to_string(requestedDiagLines)));
					this->setBaseFunctionExit(true);
					return;
				}
				
			}
		}
		errorNumber++;
		if (this->checkTimer()) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" generation timeout! timer setting: ").append(std::to_string(this->getTimeout())).append(" seconds, timer elapsed: ").append(std::to_string(((double)((double)((double)(this->getTimerTime() / 1000)) / 33) / 30))).append(" seconds!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		if (requestedRightAngleLines > 0) {
			int width = 0;
			int height = 0;
			int startX = 0;
			int startY = 0;
			bool yfirst=false;
			bool point1First=false;
			std::vector<df::Vector> toDraw;
			for (int index = 0; index < requestedRightAngleLines; index++) {
				width = this->getRandom(minRightAngleLinesWidth,maxRightAngleLineWidth);
				height = this->getRandom(minRightAngleLineHeight, maxRightAngleLineHeight);
				startX = this->getRandom(0, (mapWidth-1) - width);
				startY = this->getRandom(0,(mapHeight-1)-height);
				yfirst = false;
				if (this->getRandom(0,1) == 0) {
					yfirst = true;
				}
				point1First = false;
				if (this->getRandom(0, 1) == 0) {
					point1First = true;
				}
				toDraw = this->generateXYLine(startX, startY, startX + width, startY + height,yfirst,point1First);

				this->drawLine(protomap, toDraw, mapTileIds::EMPTY);

				if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
					this->m_debug_harness->queueErrorMessage(std::string("generate map error: ").append(std::to_string(errorNumber)).append(" right angle line draw failed! right angle line number: ").append(std::to_string(index)).append(" out of ").append(std::to_string(requestedDiagLines)));
					this->setBaseFunctionExit(true);
					return;
				}

			}
		}
		errorNumber++;
		if (this->checkTimer()) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" generation timeout! timer setting: ").append(std::to_string(this->getTimeout())).append(" seconds, timer elapsed: ").append(std::to_string(((double)((double)((double)(this->getTimerTime() / 1000)) / 33) / 30))).append(" seconds!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}


		int openCount = this->findCoordsOfValueCount(protomap, mapTileIds::EMPTY);
		errorNumber++;
		if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error: ").append(std::to_string(errorNumber)).append(" open space count failed!"));
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		if (this->checkTimer()) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" generation timeout! timer setting: ").append(std::to_string(this->getTimeout())).append(" seconds, timer elapsed: ").append(std::to_string(((double)((double)((double)(this->getTimerTime() / 1000)) / 33) / 30))).append(" seconds!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		int requestedRandomTrees = 0;
		if (openCount > neededOpen) {

			requestedRandomTrees = this->getRandom(minRandTrees, maxRandTrees);
			if (openCount - requestedRandomTrees < neededOpen) {
				requestedRandomTrees = openCount - neededOpen;
			}

			this->sprinkleTrees(protomap, requestedRandomTrees, mapTileIds::EMPTY, mapTileIds::TREE);
			errorNumber++;
			if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
				this->m_debug_harness->queueErrorMessage(std::string("generate map error: ").append(std::to_string(errorNumber)).append(" tree sprinkle failed!"));
				this->setBaseFunctionExit(true);
				return;
			}
		}
		errorNumber++;
		if (this->checkTimer()) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" generation timeout! timer setting: ").append(std::to_string(this->getTimeout())).append(" seconds, timer elapsed: ").append(std::to_string(((double)((double)((double)(this->getTimerTime() / 1000)) / 33) / 30))).append(" seconds!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}

		this->ensureSpace(protomap, mapTileIds::EMPTY, mapTileIds::TREE, neededOpen);
		errorNumber++;
		if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error: ").append(std::to_string(errorNumber)).append(" space ensurance failed!"));
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		if (this->checkTimer()) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" generation timeout! timer setting: ").append(std::to_string(this->getTimeout())).append(" seconds, timer elapsed: ").append(std::to_string(((double)((double)((double)(this->getTimerTime() / 1000)) / 33) / 30))).append(" seconds!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		
		std::vector<std::vector<df::Vector>> zones;
		zones = this->findZones(protomap,mapTileIds::FLOOD, mapTileIds::EMPTY);
		errorNumber++;
		if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error: ").append(std::to_string(errorNumber)).append(" zone search failed!"));
			this->setBaseFunctionExit(true);
			return;
		}
		errorNumber++;
		if (this->checkTimer()) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" generation timeout! timer setting: ").append(std::to_string(this->getTimeout())).append(" seconds, timer elapsed: ").append(std::to_string(((double)((double)((double)(this->getTimerTime() / 1000)) / 33) / 30))).append(" seconds!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		int zoneCount = 0;
		zoneCount = zones.size();

		if (zoneCount <= 0) {
			errorNumber++;
			
			
			this->m_debug_harness->queueErrorMessage(std::string("generate map error: ").append(std::to_string(errorNumber)).append("map gen failed! invalid zone count zone must be at least 1! zone count: ").append(std::to_string(zoneCount)));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}

		if (zoneCount > 1) {
			this->eliminateDisperateZones(protomap, mapTileIds::FLOOD, mapTileIds::EMPTY, m_configObj);
			if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
				this->m_debug_harness->queueErrorMessage(std::string("generate map error: ").append(std::to_string(errorNumber)).append(" zone search failed!"));
				this->setBaseFunctionExit(true);
				return;
			}
		}
		errorNumber++;
		if (this->checkTimer()) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" generation timeout! timer setting: ").append(std::to_string(this->getTimeout())).append(" seconds, timer elapsed: ").append(std::to_string(((double)((double)((double)(this->getTimerTime() / 1000)) / 33) / 30))).append(" seconds!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		int openSpaceCount = this->findCoordsOfValueCount(protomap, mapTileIds::EMPTY);
		if (openSpaceCount < neededOpen) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error: ").append(std::to_string(errorNumber)).append("map gen failed! too few emtpy spaces! empty space count: ").append(std::to_string(openSpaceCount)).append(" needed oepn spaces: ").append(std::to_string(neededOpen)).append("!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		
		this->sprinkleSeeds(protomap, neededSeeds, mapTileIds::EMPTY, mapTileIds::SEED);
		if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error: ").append(std::to_string(errorNumber)).append(" seed sprinkle failed!"));
			this->setBaseFunctionExit(true);
			return;
		}

		this->placeExit(protomap, mapTileIds::EMPTY, mapTileIds::EXIT);
		if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error: ").append(std::to_string(errorNumber)).append(" exit place failed!"));
			this->setBaseFunctionExit(true);
			return;
		}

		this->placeOwl(protomap, mapTileIds::EMPTY, mapTileIds::OWL);
		if (this->getCurrentMode()==GenerationStages::GENERATION_ERROR) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error: ").append(std::to_string(errorNumber)).append(" owl place failed!"));
			this->setBaseFunctionExit(true);
			return;
		}

		if (this->checkTimer()) {
			this->m_debug_harness->queueErrorMessage(std::string("generate map error:").append(std::to_string(errorNumber)).append(" generation timeout! timer setting: ").append(std::to_string(this->getTimeout())).append(" seconds, timer elapsed: ").append(std::to_string(((double)((double)((double)(this->getTimerTime() / 1000)) / 33) / 30))).append(" seconds!"));
			this->setCurrentMode(GenerationStages::GENERATION_ERROR);
			this->setBaseFunctionExit(true);
			return;
		}
		
		this->m_map_plan = this->copyMap(protomap);
		if (this->m_debug_harness->getDebugMode()) {
			
			int protomapArea = 0;
			int mapY = protomap[0].size();
			bool error = false;
			for (int i = 0; i < protomap.size(); i++) {
				if (protomap[i].size() != mapY) {
					error = true;
					m_debug_harness->queueErrorMessage("map gen debug log failed: map y sizes inconsitent!");
					m_debug_harness->drainErrorMessageQueueToLog();
					break;
				}
			}
			if (!error) {
				m_debug_harness->setDebugMap1("");
				for (int y = 0; y < mapY; y++) {
					for (int x = 0; x < protomap.size(); x++) {
						switch (protomap[x][y]) {
						case mapTileIds::EMPTY:
							this->m_debug_harness->addToDebugMap1("..");
							break;

						case mapTileIds::EXIT:
							this->m_debug_harness->addToDebugMap1("EE");
							break;
						
						case mapTileIds::OWL:
							this->m_debug_harness->addToDebugMap1("/\\");
							break;

						case mapTileIds::SEED:
							this->m_debug_harness->addToDebugMap1("@@");
							break;
						
						case mapTileIds::TREE:
							this->m_debug_harness->addToDebugMap1("##");
							break;
						}
					}
					this->m_debug_harness->addToDebugMap1("\n");
				}
				
				this->m_debug_harness->logDebugMap1();
			}
			
		}
		
		this->setGenTime(this->getTimerTime());
		this->setCurrentMode(GenerationStages::WAITING_FOR_THREAD_EXIT);
		//put this before every return, otherwise bad things start happening
		this->setBaseFunctionExit(true);
		return;
	}
	
	

	void MapBuilder::setGenTime(unsigned long long new_gen_time) {
		if (!this->getDeleteMode()) {
			m_state_gate.lock();
			m_genTime = new_gen_time;
			m_state_gate.unlock();
		}
		
	}
	unsigned long long MapBuilder::getGenTime() {
		unsigned long long temp = 0;
		if (!this->getDeleteMode()) {
			m_state_gate.lock();
			temp = m_genTime;
			m_state_gate.unlock();
		}
		
		return temp;
	}


	
	bool MapBuilder::getBaseFunctionExit() {
		bool temp = false;
		if (!this->getDeleteMode()) {
			m_state_gate.lock();
			temp = m_base_function_exit;
			m_state_gate.unlock();
		}
		
		return temp;
	}
	void MapBuilder::setBaseFunctionExit(bool new_base_function_exit) {
		if (!this->getDeleteMode()) {
			m_state_gate.lock();
			m_base_function_exit = new_base_function_exit;
			m_state_gate.unlock();
		}
		
	}


	int MapBuilder::configureMapBuilding(MapGenConfig config, std::vector<std::vector<mapTileIds::mapTileId>> mapPlan, df::Object* owl) {
		this->resetTimer();
		int borderThickness = config.getMapBorderThickness();
		df::Vector mapOrigin = config.getMapOrigin();
		df::Vector currentPos = df::Vector();

		int tileWidth = config.getMapObjectWidth();
		int tileHeight = config.getMapObjectHeight();

		int playAreaWidth = config.getMapWidth();
		int playAreaHeight = config.getMapHeight();

		int altitude = config.getMapObjectAltitude();

		int buildPerFrame = config.getObjectsConstructedPerFrame();

		if ((playAreaWidth <= 0)) {
			this->m_debug_harness->queueErrorMessage(std::string("buildMap: error 1 invalid play area width of: ").append(std::to_string(playAreaWidth)).append("!"));
			this->setCurrentMode(GenerationStages::BUILD_ERROR);
			return -1;
		}
		else if ((playAreaHeight <= 0)) {
			this->m_debug_harness->queueErrorMessage(std::string("buildMap: error 2 invalid play area height of: ").append(std::to_string(playAreaHeight)).append("!"));
			this->setCurrentMode(GenerationStages::BUILD_ERROR);
			return -1;
		}
		else if ((borderThickness < 0)) {
			this->m_debug_harness->queueErrorMessage(std::string("buildMap: error 3 invalid border thickness of: ").append(std::to_string(borderThickness)).append("!"));
			this->setCurrentMode(GenerationStages::BUILD_ERROR);
			return -1;
		}
		else if ((tileWidth < 1)) {
			this->m_debug_harness->queueErrorMessage(std::string("buildMap: error 4 invalid tile width of: ").append(std::to_string(tileWidth)).append("!"));
			this->setCurrentMode(GenerationStages::BUILD_ERROR);
			return -1;
		}
		else if ((tileHeight < 1)) {
			this->m_debug_harness->queueErrorMessage(std::string("buildMap: error 5 invalid tile height of: ").append(std::to_string(tileHeight)).append("!"));
			this->setCurrentMode(GenerationStages::BUILD_ERROR);
			return -1;
		}
		else if ((altitude < 0) || (altitude > df::MAX_ALTITUDE)) {
			this->m_debug_harness->queueErrorMessage(std::string("buildMap: error 6 invalid tile height of: ").append(std::to_string(altitude)).append("!"));
			this->setCurrentMode(GenerationStages::BUILD_ERROR);
			return -1;
		}
		else if (playAreaWidth > mapPlan.size()) { // error only if the plan is narrower than configured
			this->m_debug_harness->queueErrorMessage(std::string("buildMap: error 7 invalid map x size of: ").append(std::to_string(mapPlan.size())).append(" configured width: ").append(std::to_string(playAreaWidth)).append("!"));
			this->setCurrentMode(GenerationStages::BUILD_ERROR);
			return -1;
		}
		else if (buildPerFrame <= 0) {
			this->m_debug_harness->queueErrorMessage(std::string("buildMap: error 8 invalid per frame object construction count, must be at least one, give count of: ").append(std::to_string(buildPerFrame)).append("!"));
			this->setCurrentMode(GenerationStages::BUILD_ERROR);
			return -1;

		}
		bool UnmatchingHeight = false;
		int badx = 0;
		for (int i = 0; i < mapPlan.size(); i++) {
			if (mapPlan[i].size() != playAreaHeight) {
				UnmatchingHeight = true;
				badx = i;
				break;
			}
		}
		if (UnmatchingHeight) {
			this->m_debug_harness->queueErrorMessage(std::string("buildMap: error 9 invalid map y size of: ").append(std::to_string(mapPlan[badx].size())).append(" at map x: ").append(std::to_string(badx)).append(" configured width: ").append(std::to_string(playAreaHeight)).append("!"));
			this->setCurrentMode(GenerationStages::BUILD_ERROR);
			return -1;
		}

		m_builder_state.reset();
		m_builder_state.setAltitude(altitude);
		m_builder_state.setBorderThickness(borderThickness);
		m_builder_state.setCurrentGlobalPos(currentPos);
		m_builder_state.setMapHeight(playAreaHeight);
		m_builder_state.setMapOrigin(mapOrigin);
		m_builder_state.setMapPlan(mapPlan);
		m_builder_state.setMapWidth(playAreaWidth);
		m_builder_state.setObjectSpawnPerFrame(buildPerFrame);
		m_builder_state.setPlayer(owl);
		m_builder_state.setTileHeight(tileHeight);
		m_builder_state.setTileWidth(tileWidth);

		return 0;
	}


	int MapBuilder::buildMapV2(MapBuildStateObject& state) {
		df::Object* newGround = nullptr;
		df::Object* newTree = nullptr;
		df::Object* newExit = nullptr;
		df::Object* newSeed = nullptr;
		int lastY = 0;
		for (int i = 0; i < state.getObjectSpawnPerFrame(); i++) {
			if (state.getFinished()) {
				
				this->setCurrentMode(GenerationStages::BUILD_DONE);

				return 0;
			}
			switch (state.getValueAtCurrentPosition()) {
			case mapTileIds::EMPTY:
				if (CREATE_GROUND_OBJECTS) {
					newGround = new Ground();
					newGround->setAltitude(state.getAltitude());
					newGround->setPosition(state.getTrueCursorPos());
					state.addMapObject(newGround);
				}
				if (m_debug_harness->getDebugMode()) {
					m_debug_harness->addToCurrentDebugStrip("..");
				}
				break;
			case mapTileIds::OWL:
				state.getPlayer()->setPosition(state.getTrueCursorPos());
				if (m_debug_harness->getDebugMode()) {
					m_debug_harness->addToCurrentDebugStrip("/\\");
				}
				break;
			case mapTileIds::EXIT:
				newExit = new MapExit();
				newExit->setAltitude(state.getAltitude());
				newExit->setPosition(state.getTrueCursorPos());
				state.addMapObject(newExit);
				if (m_debug_harness->getDebugMode()) {
					m_debug_harness->addToCurrentDebugStrip("EE");
				}
				break;
			case mapTileIds::SEED:
				newSeed = new Seed();
				newSeed->setAltitude(state.getAltitude());
				newSeed->setPosition(state.getTrueCursorPos());
				state.addMapObject(newSeed);
				if (m_debug_harness->getDebugMode()) {
					m_debug_harness->addToCurrentDebugStrip("@@");
				}
				break;
			case mapTileIds::TREE:
				newTree = new Tree();
				newTree->setAltitude(state.getAltitude());
				newTree->setPosition(state.getTrueCursorPos());
				state.addMapObject(newTree);
				if (m_debug_harness->getDebugMode()) {
					m_debug_harness->addToCurrentDebugStrip("##");
				}
				break;
			default:
				state.deleteAllMapObjects();
				this->m_debug_harness->queueErrorMessage(std::string("buildMapV2: error 0 invalid tile id!").append(" values: id Value: ").append(std::to_string((int) (state.getMapPlan().at(state.getCurrentX()).at(state.getCurrentY())))).append("!"));
				this->setCurrentMode(GenerationStages::BUILD_ERROR);
				return -1;

			}
			lastY = state.getCurrentY();
			state.advanceCursor();
			if (m_debug_harness->getDebugMode()) {
				if (lastY != state.getCurrentY()) {
					m_debug_harness->storeCurrentDebugStrip();
				}
			}
			
		}
		return 0;
	}



	/*
	
	//this is the important function to change to swap out the different game objects the gnerator instances
	//need to rewrite this function and its infastructure with cooperative multitasking in mind
	int MapBuilder::buildMap(MapGenConfig config, std::vector<std::vector<mapTileIds::mapTileId>>& map, df::Object* owl) {
		//the current version is broken and dperecated, an new v2 will be made shortly
		if (map.empty()) {
			this->m_debug_harness->queueErrorMessage("buildMap: error 0 map empty!");
			this->setCurrentMode(GenerationStages::BUILD_ERROR);
			return -1;
		}

		
		
		int borderThickness = config.getMapBorderThickness();
		df::Vector start = config.getMapOrigin();
		df::Vector currentPos = df::Vector();
		currentPos = this->getCurrentBuildPos();
		int tileWidth = config.getMapObjectWidth();
		int tileHeight = config.getMapObjectHeight();

		int playAreaWidth = config.getMapWidth();
		int playAreaHeight = config.getMapHeight();

		int altitude = config.getMapObjectAltitude();

		if ((playAreaWidth <= 0)) {
			this->m_debug_harness->queueErrorMessage(std::string("buildMap: error 1 invalid play area width of: ").append(std::to_string(playAreaWidth)).append("!"));
			this->setCurrentMode(GenerationStages::BUILD_ERROR);
			return -1;
		}
		else if ((playAreaHeight <= 0)) {
			this->m_debug_harness->queueErrorMessage(std::string("buildMap: error 2 invalid play area height of: ").append(std::to_string(playAreaHeight)).append("!"));
			this->setCurrentMode(GenerationStages::BUILD_ERROR);
			return -1;
		}
		else if ((borderThickness < 0)) {
			this->m_debug_harness->queueErrorMessage(std::string("buildMap: error 3 invalid border thickness of: ").append(std::to_string(borderThickness)).append("!"));
			this->setCurrentMode(GenerationStages::BUILD_ERROR);
			return -1;
		}
		else if ((tileWidth < 1)) {
			this->m_debug_harness->queueErrorMessage(std::string("buildMap: error 4 invalid tile width of: ").append(std::to_string(tileWidth)).append("!"));
			this->setCurrentMode(GenerationStages::BUILD_ERROR);
			return -1;
		}
		else if ((tileHeight < 1)) {
			this->m_debug_harness->queueErrorMessage(std::string("buildMap: error 5 invalid tile height of: ").append(std::to_string(tileHeight)).append("!"));
			this->setCurrentMode(GenerationStages::BUILD_ERROR);
			return -1;
		}
		else if ((altitude < 0) || (altitude > df::MAX_ALTITUDE)) {
			this->m_debug_harness->queueErrorMessage(std::string("buildMap: error 6 invalid tile height of: ").append(std::to_string(altitude)).append("!"));
			this->setCurrentMode(GenerationStages::BUILD_ERROR);
			return -1;
		}
		else if (playAreaWidth > map.size()) { // error only if the plan is narrower than configured
			this->m_debug_harness->queueErrorMessage(std::string("buildMap: error 7 invalid map size of: ").append(std::to_string(map.size())).append(" configured width: ").append(std::to_string(playAreaWidth)).append("!"));
			this->setCurrentMode(GenerationStages::BUILD_ERROR);
			return -1;
		}
		else if (this->getBuildPerFrame() <= 0) {
			this->m_debug_harness->queueErrorMessage(std::string("buildMap: error 8 invalid per frame object construction count, must be at least one, give count of: ").append(std::to_string(this->getBuildPerFrame())).append("!"));
			this->setCurrentMode(GenerationStages::BUILD_ERROR);
			return -1;
		}
		df::Object* newGround=nullptr;
		df::Object* newTree=nullptr;
		df::Object* newExit = nullptr;
		df::Object* newSeed = nullptr;
		int framePorgressCount = 0;
		int startX = this->getLastX();
		int startY = this->getlastY();
		if (startY >= playAreaHeight + (borderThickness * 2) - 1) {
			startY = 0;
		}
		if (this->getBuildProgress() < this->getToBuild()) {
			for (int x = startX; x < playAreaWidth + (borderThickness * 2);x++) {


				for (int y = startY; y < playAreaHeight + (borderThickness * 2); y++) {

					if (m_debug) {
						m_debug_Coords.push_back(currentPos);
						m_debug_map_positions.push_back(df::Vector(x, y));
					}
					
					
					if (((x < borderThickness) || (x >= borderThickness + playAreaWidth)) || ((y < borderThickness) || (y >= borderThickness + playAreaHeight))) {

						newTree = new Tree();
						newTree->setAltitude(0);
						newTree->setPosition(start + currentPos);
						if (m_debug) {
							m_current_debug_strip.push_back("##");

						}
						m_mapReturn.insert(newTree);

					}
					else {

						if (map.at(x - borderThickness).size() < playAreaHeight) {
							df::WorldManager& wm = df::WorldManager::getInstance();
							for (int i = 0; i < m_mapReturn.getCount(); i++) {
								wm.markForDelete(m_mapReturn[i]);
							}
							this->m_debug_harness->queueErrorMessage(std::string("buildMap: error 9 map Y vector less than configured!").append(" values: map y: ").append(std::to_string(map.at(x - borderThickness).size())).append(" configured y: ").append(std::to_string(playAreaHeight)).append("!"));
							this->setCurrentMode(GenerationStages::BUILD_ERROR);
							return -1;
						}
						//if you want to swap out the gameobjects the object instances, this switch statement is where you do that
						switch (map.at((x)-borderThickness).at((y)-borderThickness)) {
						case mapTileIds::EMPTY:
							if (CREATE_GROUND_OBJECTS) {
								newGround = new Ground();
								newGround->setAltitude(altitude);
								newGround->setPosition(currentPos + start);
								m_mapReturn.insert(newGround);
							}
							if (m_debug) {
								m_current_debug_strip.push_back("..");
							}
							break;
						case mapTileIds::OWL:
							owl->setPosition(currentPos + start);
							if (m_debug) {
								m_current_debug_strip.push_back("/\\");
							}
							break;
						case mapTileIds::EXIT:
							newExit = new MapExit();
							newExit->setAltitude(altitude);
							newExit->setPosition(currentPos + start);
							m_mapReturn.insert(newExit);
							if (m_debug) {
								m_current_debug_strip.push_back("EE");
							}
							break;
						case mapTileIds::SEED:
							newSeed = new Seed();
							newSeed->setAltitude(altitude);
							newSeed->setPosition(currentPos + start);
							m_mapReturn.insert(newSeed);
							if (m_debug) {
								m_current_debug_strip.push_back("@@");
							}
							break;
						case mapTileIds::TREE:
							newTree = new Tree();
							newTree->setAltitude(altitude);
							newTree->setPosition(start + currentPos);
							m_mapReturn.insert(newTree);
							if (m_debug) {
								m_current_debug_strip.push_back("##");
							}
							break;
						default:
							df::WorldManager& wm = df::WorldManager::getInstance();
							for (int i = 0; i < m_mapReturn.getCount(); i++) {
								wm.markForDelete(m_mapReturn[i]);
							}
							this->m_debug_harness->queueErrorMessage(std::string("buildMap: error 10 invalid tile id!").append(" values: id Value: ").append(std::to_string((int)map.at((x)-borderThickness).at((y)-borderThickness))).append("!"));
							this->setCurrentMode(GenerationStages::BUILD_ERROR);
							return -1;

						}
					}
					currentPos.setY(currentPos.getY() + tileHeight);
					
					if (framePorgressCount >= this->getBuildPerFrame()) {
						if (y >= tileHeight*(playAreaHeight + (borderThickness * 2))) {
							currentPos.setY(0);
							currentPos.setX(currentPos.getX() + tileWidth);
							this->setLastX(this->getLastX() + 1);
							this->setLastY(0);
							if (m_debug) {
								m_debug_strips.push_back(m_current_debug_strip);
								m_current_debug_strip.clear();
							}
							
						}
						else {
							this->setLastY(y + 1);
							

						}
						this->setCurrentBuildPos(currentPos);
						this->setBuildProgress(this->getBuildProgress() + framePorgressCount);
						return 0;

					}
					framePorgressCount++;
				}
				if (m_debug) {
					m_debug_strips.push_back(m_current_debug_strip);
					m_current_debug_strip.clear();
				}
				
				startY = 0;
				currentPos.setY(0);
				currentPos.setX(currentPos.getX() + tileWidth);
			}
		}
		
		
		m_mapReturn.insert(owl);

		if (m_debug) {
			df::LogManager& lm = df::LogManager::getInstance();
			m_debug_map2 = "built map:\n";
			int stripy = m_debug_strips[0].size();
			int error = false;
			std::string coordsString="visited coords: ";
			for (int i = 0; i < m_debug_Coords.size(); i++) {
				coordsString.append(std::string("< coord: ").append(std::to_string(i)).append(" map pos:  x:").append(std::to_string(m_debug_map_positions[i].getX())).append(" y: ").append(std::to_string(m_debug_map_positions[i].getY())).append(" object pos: x: ").append(std::to_string(m_debug_Coords[i].getX())).append(" y: ").append(std::to_string(m_debug_Coords[i].getY())).append(" >, "));
			}
			
			for (int i = 0; i < m_debug_strips.size(); i++) {
				if (m_debug_strips[i].size() != stripy) {
					error = true;
					m_log_man_access.lock();
					lm.writeLog("map build debug log failed: strip sizes inconsitent!");
					m_log_man_access.unlock();
					break;
				}
			}
			if (!error) {
				for(int y=0; y< stripy; y++){
					for (int x = 0; x < m_debug_strips.size(); x++) {
						m_debug_map2.append(m_debug_strips[x][y]);
					}
					m_debug_map2.append("\n");
				}
				m_log_man_access.lock();
				lm.writeLog(m_debug_map2.c_str());
				m_log_man_access.unlock();
			}
			m_log_man_access.lock();
			lm.writeLog(coordsString.c_str());
			m_log_man_access.unlock();
		}

		this->setBuildProgress(this->getBuildProgress() + framePorgressCount);
		
		
		this->setBuildDone(true);
		return 0;
	}

	*/
	


	int MapBuilder::startGenerateMap(MapGenConfig config, df::Object* owl){
		// The build places the owl on its start tile, so there must be one
		if (owl == nullptr) {
			df::LogManager::getInstance().writeLog("startGenerateMap: error, no owl given; map not started");
			return -1;
		}
		if ((this->getCurrentMode()==GenerationStages::READY)||(this->getCurrentMode()==GenerationStages::DONE)) {
			
			m_configObj = MapGenConfig();
			if (m_debug_harness != nullptr) {
				m_debug_harness->setDeleteMode();
				delete m_debug_harness;
				m_debug_harness = nullptr;
			}
			
			m_debug_harness = new MapGenDebugObj(config.getGenDebugMode());
			m_timer = df::Clock();
			m_map_plan = std::vector<std::vector<mapTileIds::mapTileId>>();
			
			m_player = nullptr;
			if (m_genThread != nullptr) {
				if (m_genThread->joinable()) {
					m_genThread->join();
				}
			
				delete m_genThread;
			}
			m_genThread = nullptr;
			m_genTime = 0;
			m_error_handled = false;
			m_timeout = 0;
			
			m_RandomEngine = std::mt19937();
			
			m_player = owl;
			m_configObj = config;
			m_requested_config = config;
			this->setBaseFunctionExit(false);
			// Wait for the generation thread; it moves to WAITING_FOR_THREAD_EXIT when the map plan is ready
			this->setCurrentMode(GenerationStages::GENERATING);
			m_builder_state = MapBuildStateObject();
			m_genThread = new std::thread(&MapBuilder::generateMap, this);

			
			return 0;
			
			
		}
		return -1;
		
	}

	void MapBuilder::retryGeneration() {
		df::LogManager& lm = df::LogManager::getInstance();
		for (const std::string& message : m_debug_harness->getErrorMessages()) {
			lm.writeLog("map generation error: %s", message.c_str());
		}
		// A fixed seed would fail the same way again, and a bad config fails every time
		if (m_requested_config.getRandomSeed() != 0 || m_retries >= MAX_GENERATION_RETRIES) {
			lm.writeLog("map generation failed, not retrying (retries used: %d)", m_retries);
			return;
		}
		m_retries++;
		lm.writeLog("map generation retry %d of %d", m_retries, MAX_GENERATION_RETRIES);
		this->setCurrentMode(GenerationStages::READY); // startGenerateMap() only starts from READY or DONE
		this->startGenerateMap(m_requested_config, m_player);
	}

	bool MapBuilder::isMapGenFinished() {
		
		return this->getCurrentMode() >= GenerationStages::GENERATION_DONE;
	}

	bool MapBuilder::isMapBuildFinished() {
		
		return this->getCurrentMode() >= GenerationStages::BUILD_DONE;
	}

	
	bool MapBuilder::getDeleteMode() {
		bool temp = true;
		if (!m_delete_mode) {
			m_state_gate.lock();
			m_mode_gate.lock();
			temp = m_delete_mode;
			m_state_gate.unlock();
			m_mode_gate.unlock();
		}
		return temp;
	}
	void MapBuilder::setDeleteMode(bool new_delete_mode) {
		if (!m_delete_mode) {
			m_state_gate.lock();
			m_mode_gate.lock();
			m_delete_mode = new_delete_mode;
			m_state_gate.unlock();
			m_mode_gate.unlock();
		}
	}

	MapBuilder::MapBuilder() {
		m_configObj = MapGenConfig();
		m_debug_harness = nullptr;
		m_timer = df::Clock();
		m_map_plan = std::vector<std::vector<mapTileIds::mapTileId>>();
		m_timeout = 0;
		m_player = nullptr;
		m_genThread = nullptr;
		m_genTime = 0;
		m_genTime = 0;
		m_error_handled=false;
		m_requested_config = MapGenConfig();
		m_retries = 0;
		m_RandomEngine = std::mt19937();
		this->setCurrentMode(GenerationStages::READY);
		this->setCameraAffected(false);
		this->setPosition(df::Vector(57, 15)); // centre of the 115x30 window
		this->setType("mapBuilder");
		m_builder_state = MapBuildStateObject();
		// Step events drive generation and building; the engine only sends them to registered objects
		df::EventManager::getInstance().registerEvent(this, df::STEP_EVENT);

	}
	MapBuilder::~MapBuilder() {
		// The thread is null if generation never started, or after eventHandler joined it
		if (m_genThread != nullptr) {
			if (m_genThread->joinable()) {
				m_genThread->join();
			}
			delete m_genThread;
			m_genThread = nullptr;
		}
		if (m_debug_harness != nullptr) {
			m_debug_harness->setDeleteMode();
			delete m_debug_harness;
		}
		

		this->setDeleteMode(true);
		
	}


	GenerationStages::GenerationStage MapBuilder::getCurrentMode() {
		GenerationStages::GenerationStage temp = GenerationStages::STAGE_ERROR;
		if (!this->getDeleteMode()) {
			m_mode_gate.lock();
			temp = m_current_mode;
			m_mode_gate.unlock();
		}
		return temp;
	}
	void MapBuilder::setCurrentMode(GenerationStages::GenerationStage new_mode) {
		if (!this->getDeleteMode()) {
			m_mode_gate.lock();
			GenerationStages::GenerationStage old_mode = m_current_mode; // TEMP-MAPGEN-DEBUG
			m_current_mode = new_mode;
			m_mode_gate.unlock();
			// TEMP-MAPGEN-DEBUG: record every stage change, which thread made it, and any queued errors
			static std::mutex debug_file_gate; // TEMP-MAPGEN-DEBUG
			std::lock_guard<std::mutex> debug_lock(debug_file_gate); // TEMP-MAPGEN-DEBUG
			std::ofstream debug_file("mapgen_debug.log", std::ios::app); // TEMP-MAPGEN-DEBUG
			debug_file << "builder " << this->getId() << " thread " << std::this_thread::get_id() << " mode " << (int)old_mode << " -> " << (int)new_mode << "\n"; // TEMP-MAPGEN-DEBUG
			if ((new_mode == GenerationStages::GENERATION_ERROR || new_mode == GenerationStages::BUILD_ERROR) && m_debug_harness != nullptr) { // TEMP-MAPGEN-DEBUG
				for (const std::string& message : m_debug_harness->getErrorMessages()) { // TEMP-MAPGEN-DEBUG
					debug_file << "    error: " << message << "\n"; // TEMP-MAPGEN-DEBUG
				} // TEMP-MAPGEN-DEBUG
			} // TEMP-MAPGEN-DEBUG
		}
	}

	int configureMapBuilding();


	int MapBuilder::destroyMap(df::ObjectList map) {
		df::WorldManager& wm = df::WorldManager::getInstance();
		if (map.isEmpty()) {
			return 0;
		}
		for (int index = 0; index < map.getCount(); index++) {
			wm.markForDelete(map[index]);
		}
		return 0;
	}

	

	int MapBuilder::eventHandler(const df::Event* m_p) {

		if (m_p->getType() == df::STEP_EVENT) {
			if ((this->getCurrentMode() != GenerationStages::DONE)&& (this->getCurrentMode() != GenerationStages::READY)) {
				df::GameManager& gm = df::GameManager::getInstance();
				EventMapGenDone done;

				switch (this->getCurrentMode()) {
				case GenerationStages::GENERATION_ERROR:
					if (this->getBaseFunctionExit()) {
						if (!this->m_error_handled) {
							df::GameManager& gm = df::GameManager::getInstance();
							df::LogManager& lm = df::LogManager::getInstance();
							
							if (m_genThread != nullptr) {
								if (m_genThread->joinable()) {
									m_genThread->join();
								}
								delete m_genThread;
								m_genThread = nullptr;
							}


							done = EventMapGenDone(m_debug_harness->getErrorMessages());
							gm.onEvent(&done);
							m_error_handled = true;
							this->retryGeneration();
						}

					}
					break;
				case GenerationStages::BUILD_ERROR:
					if (!this->m_error_handled) {
						df::GameManager& gm = df::GameManager::getInstance();
						df::LogManager& lm = df::LogManager::getInstance();

						done = EventMapGenDone(m_debug_harness->getErrorMessages());
						gm.onEvent(&done);
						m_error_handled = true;
						this->retryGeneration();
					}
					break;
				case GenerationStages::WAITING_TO_GENERATE:
					this->setCurrentMode(GenerationStages::GENERATING);
					break;
				case GenerationStages::GENERATING:

					break;
				case GenerationStages::GENERATION_DONE:
					this->setCurrentMode(GenerationStages::WAITING_FOR_BUILD_START);
					break;
				case GenerationStages::WAITING_FOR_THREAD_EXIT:
					if (this->getBaseFunctionExit()) {
						
						if (this->m_genThread != nullptr) {
							if (m_genThread->joinable()) {
								m_genThread->join();
							}
							delete m_genThread;
							m_genThread = nullptr;
						}
						this->setCurrentMode(GenerationStages::GENERATION_DONE);
					}

					break;
				case GenerationStages::WAITING_FOR_BUILD_START:
					this->configureMapBuilding(m_configObj, m_map_plan, m_player);
					this->setCurrentMode(GenerationStages::BUILDING);
					break;
				case GenerationStages::BUILDING:
					this->buildMapV2(m_builder_state);
					if (m_builder_state.getFinished()) {
						this->setCurrentMode(GenerationStages::BUILD_DONE);
					}
					break;
				case GenerationStages::BUILD_DONE:
					this->setBuildTime(this->checkTimer());
					if (m_debug_harness->getDebugMode()) {
						m_debug_harness->compileDebugStripsIntoMap2();
						m_debug_harness->logDebugMap2();
					}
					m_builder_state.addMapObject(m_builder_state.getPlayer());
					this->setCurrentMode(GenerationStages::SENDING_EVENT);
					break;
				case GenerationStages::SENDING_EVENT:

					done = EventMapGenDone(m_builder_state.getCurrentMapObjects(), this->getGenTime(), this->getBuildTime());
					gm.onEvent(&done);
					this->setCurrentMode(GenerationStages::DONE);
					this->setVisible(false);
					break;
				case GenerationStages::STAGE_ERROR:

					break;
				case GenerationStages::READY:

					break;
				case GenerationStages::DONE:
					this->setVisible(false);
					break;
				default:
					this->setCurrentMode(GenerationStages::STAGE_ERROR);
					break;
				}
			}
			
			
			
			
			
			return 1;
		}
		return 0;
	}

	int MapBuilder::draw() {
		if (this->getVisible()) {
			df::DisplayManager& dm = df::DisplayManager::getInstance();
			switch (this->getCurrentMode()) {
				case GenerationStages::GENERATION_ERROR:
					return dm.drawString(this->getPosition(), "map generation ERROR!", df::CENTER_JUSTIFIED, df::WHITE);
					break;
				case GenerationStages::BUILD_ERROR:
					return dm.drawString(this->getPosition(), "map build ERROR!", df::CENTER_JUSTIFIED, df::WHITE);
					break;
				case GenerationStages::WAITING_TO_GENERATE:
					return dm.drawString(this->getPosition(), "waiting to generate!", df::CENTER_JUSTIFIED, df::WHITE);
					break;
				case GenerationStages::GENERATING:
					return dm.drawString(this->getPosition(), "generating map!", df::CENTER_JUSTIFIED, df::WHITE);
					break;
				case GenerationStages::GENERATION_DONE:
					return dm.drawString(this->getPosition(), "map generation done!", df::CENTER_JUSTIFIED, df::WHITE);
					break;
				case GenerationStages::WAITING_FOR_THREAD_EXIT:
					return dm.drawString(this->getPosition(), "waiting for thread exit!", df::CENTER_JUSTIFIED, df::WHITE);
					break;
				case GenerationStages::WAITING_FOR_BUILD_START:
					return dm.drawString(this->getPosition(), "waiting for build start!", df::CENTER_JUSTIFIED, df::WHITE);
					break;
				case GenerationStages::BUILDING:
					return dm.drawString(this->getPosition(), "building map!", df::CENTER_JUSTIFIED, df::WHITE);
					break;
				case GenerationStages::BUILD_DONE:
					return dm.drawString(this->getPosition(), "map building done!", df::CENTER_JUSTIFIED, df::WHITE);
					break;
				case GenerationStages::SENDING_EVENT:
					return dm.drawString(this->getPosition(), "sending event!", df::CENTER_JUSTIFIED, df::WHITE);
					break;
				case GenerationStages::STAGE_ERROR:
					return dm.drawString(this->getPosition(), "stage error done!", df::CENTER_JUSTIFIED, df::WHITE);
					break;
				case GenerationStages::READY:
					return dm.drawString(this->getPosition(), "ready!", df::CENTER_JUSTIFIED, df::WHITE);
					break;
				case GenerationStages::DONE:
					return dm.drawString(this->getPosition(), "done!", df::CENTER_JUSTIFIED, df::WHITE);
					break;
				default:
					return dm.drawString(this->getPosition(), "unknown state!", df::CENTER_JUSTIFIED, df::WHITE);
				}
		}
		return 0;
	}



	
}