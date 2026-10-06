#include "MapBuilder.h"

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
		if (map.empty()) {
			this->addErrorMessage("drawLineError: error 0 map empty!");
			this->setGenError(true);
			
			
			return -1;
		}
		else if (line.empty()) {
			this->addErrorMessage("drawLineError: error 1 line empty!");
			this->setGenError(true);


			return -1;
		}

		df::Vector point;
		for (int pointIndex = 0; pointIndex < line.size(); pointIndex++) {

			point = line[pointIndex];
			if ((((int)point.getX()) > map.size()) || (((int)point.getX()) < 0) ){
				this->addErrorMessage(std::string("drawLineError: error 2 line extends outside map!").append(" invalid point is x: ").append(std::to_string(point.getX())).append(" map size is: Width: ").append(std::to_string(map.size())).append("!"));
				this->setGenError(true);
				return -1;
			}
			if ((((int)point.getY()) > map[((int)point.getX())].size()) || (((int)point.getY()) < 0)) {
				this->addErrorMessage(std::string("drawLineError: error 2 line extends outside map!").append(" invalid point is ").append(" y: ").append(std::to_string(point.getY())).append(" map size is: ").append(" height: ").append(std::to_string(map[((int)point.getX())].size())).append("!"));
				this->setGenError(true);
				return -1;
			}
			
			map[(int)point.getX()][(int)point.getY()] = type;
		}
		return 0;
	}

	int MapBuilder::drawRectangle(std::vector<std::vector<mapTileIds::mapTileId>>& map, df::Box square, mapTileIds::mapTileId value) {
		if (map.empty()) {
			this->addErrorMessage("drawSquare: error 0 map empty!");
			this->setGenError(true);
			return -1;
		}
		int width = (int)square.getHorizontal();
		int height = (int)square.getVertical();
		int x = (int)square.getCorner().getX();
		int y = ((int)square.getCorner().getY());
		if ((width < 1) || (height < 1) || (x < 0) || (y < 0)) {
			this->addErrorMessage(std::string("drawSquare: error 1 invalid square dimensions or coordinates!").append(" values are: width:").append(std::to_string(width)).append(" height: ").append(std::to_string(height)).append(" x: ").append(std::to_string(x)).append(" y: ").append(std::to_string(y)).append("!"));
			this->setGenError(true);
			return -1;
		}

		if (x + width - 1 > map.size()) {
			this->addErrorMessage(std::string("drawSquare: error 2 invalid square size or coordinates!").append(" values are: max square x: ").append(std::to_string(x+width -1)).append(" map width: ").append(std::to_string(map.size())).append("!"));
			this->setGenError(true);
			return -1;
		}

		for (int i = x; i < width + x; i++) {
			for (int j = y; j < y + height; j++) {
				if (map[i].size() <= j) {
					this->addErrorMessage(std::string("drawSquare: error 3 invalid square size or coordinates!").append(" values are: y:").append(std::to_string(j)).append(" map x: ").append(std::to_string(i)).append(" map height at map x: ").append(std::to_string(map[i].size())).append("!"));
					this->setGenError(true);
					return -1;
				}
				map[i][j] = value;
			}
		}
		
		return 0;

	}

	int MapBuilder::floodFill(std::vector<std::vector<mapTileIds::mapTileId>>& map, df::Vector location, mapTileIds::mapTileId fillValue, mapTileIds::mapTileId emptyValue) {
		if (map.empty()) {
			this->addErrorMessage("floodfill: error 0 map empty!");
			this->setGenError(true);
			return -1;
		}
		
		std::unordered_set<df::Vector> visited;

		std::queue<df::Vector> toVisit;

		toVisit.push(location);
		
		df::Vector currentCoord;
		df::Vector newCoord;

		while (!toVisit.empty()) {
			currentCoord = toVisit.front();
			toVisit.pop();
			if (!visited.contains(currentCoord)) {
				if ((((int)currentCoord.getX()) < 0) || (((int)currentCoord.getX()) >= map.size()) || (((int)currentCoord.getY()) < 0) || (((int)currentCoord.getY()) >= map[((int)currentCoord.getX())].size())) {
					this->addErrorMessage("floodfill: error 1 point outside map!");
					this->setGenError(true);
				}

				if (map.at(((int)currentCoord.getX())).at(((int)currentCoord.getY())) == emptyValue) {
					map[((int)currentCoord.getX())][((int)currentCoord.getY())] = fillValue;
					newCoord = currentCoord;
					newCoord.setX(((int)newCoord.getX()) - 1);
					if ((((int)newCoord.getX()) >= 0)&&map[((int)newCoord.getX())][((int)newCoord.getY())]==emptyValue) {
						toVisit.push(newCoord);
					}
					newCoord = currentCoord;
					newCoord.setX(((int)newCoord.getX()) + 1);
					if ((((int)newCoord.getX()) < map.size()) && map[((int)newCoord.getX())][((int)newCoord.getY())] == emptyValue) {
						toVisit.push(newCoord);
					}
					newCoord = currentCoord;
					newCoord.setY(((int)newCoord.getY()) - 1);
					if ((((int)newCoord.getY()) >= 0) && map[((int)newCoord.getX())][((int)newCoord.getY())] == emptyValue) {
						toVisit.push(newCoord);
					}
					newCoord = currentCoord;
					newCoord.setY(((int)newCoord.getY()) + 1);
					if ((((int)newCoord.getX()) < map[((int)newCoord.getX())].size()) && map[((int)newCoord.getX())][((int)newCoord.getY())] == emptyValue) {
						toVisit.push(newCoord);
					}
				}

				visited.insert(currentCoord);
			}
			
		}



		return 0;
	}

	std::vector<df::Vector> MapBuilder::floodFillReturnCoords(std::vector<std::vector<mapTileIds::mapTileId>>& map, df::Vector location, mapTileIds::mapTileId fillValue, mapTileIds::mapTileId emptyValue) {
		if (map.empty()) {
			this->addErrorMessage("floodfillReturnCoords: error 0 map empty!");
			this->setGenError(true);
			return std::vector<df::Vector>();
		}
		else if (fillValue == emptyValue) {
			this->addErrorMessage(std::string("floodfillReturnCoords: error 1 fill value is equal to emtpy value! fill and empty value: ").append(std::to_string((int)fillValue)));
			this->setGenError(true);
			return std::vector<df::Vector>();
		}

		std::unordered_set<df::Vector> visited;

		std::queue<df::Vector> toVisit;

		std::vector<df::Vector> foundCoords;

		toVisit.push(location);

		df::Vector currentCoord;
		df::Vector newCoord;

		while (!toVisit.empty()) {
			currentCoord = toVisit.front();
			toVisit.pop();
			if (!visited.contains(currentCoord)) {
				if ((((int)currentCoord.getX()) < 0) || (((int)currentCoord.getX()) >= map.size()) || (((int)currentCoord.getY()) < 0) || (((int)currentCoord.getY()) >= map[((int)currentCoord.getX())].size())) {
					return std::vector<df::Vector>();
				}

				if (map.at(((int)currentCoord.getX())).at(((int)currentCoord.getY())) == emptyValue) {
					map[((int)currentCoord.getX())][((int)currentCoord.getY())] = fillValue;
					foundCoords.push_back(currentCoord);
					newCoord = currentCoord;
					newCoord.setX(((int)newCoord.getX()) - 1);
					if ((((int)newCoord.getX()) >= 0) && map[((int)newCoord.getX())][((int)newCoord.getY())] == emptyValue) {
						toVisit.push(newCoord);
					}
					newCoord = currentCoord;
					newCoord.setX(((int)newCoord.getX()) + 1);
					if ((((int)newCoord.getX()) < map.size()) && map[((int)newCoord.getX())][((int)newCoord.getY())] == emptyValue) {
						toVisit.push(newCoord);
					}
					newCoord = currentCoord;
					newCoord.setY(((int)newCoord.getY()) - 1);
					if ((((int)newCoord.getY()) >= 0) && map[((int)newCoord.getX())][((int)newCoord.getY())] == emptyValue) {
						toVisit.push(newCoord);
					}
					newCoord = currentCoord;
					newCoord.setY(((int)newCoord.getY()) + 1);
					if ((((int)newCoord.getX()) < map[((int)newCoord.getX())].size()) && map[((int)newCoord.getX())][((int)newCoord.getY())] == emptyValue) {
						toVisit.push(newCoord);
					}
				}

				visited.insert(currentCoord);
			}

		}



		return foundCoords;
	}

	std::vector<df::Vector> MapBuilder::findCoordsOfValue(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId type) {
		if (map.empty()) {
			this->addErrorMessage("findOpenCoords: error 0 map empty!");
			this->setGenError(true);
			return std::vector<df::Vector>();
		}

		std::vector<df::Vector> openCoords;

		for (int x = 0; x < map.size();x++) {
			if (map.at(x).empty()) {
				this->addErrorMessage("FindOpenCoords: error 1 map collumn empty!");
				this->setGenError(true);
				return std::vector<df::Vector>();
			}
			for (int y = 0; y < map.at(x).size(); y++) {
				if (map[x][y] == type) {
					openCoords.push_back(df::Vector(x, y));
				}
			
			}
		}
		return openCoords;
	}

	int MapBuilder::findCoordsOfValueCount(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId type) {
		if (map.empty()) {
			this->addErrorMessage("FindOpencoordsCount: error 0 map empty!");
			this->setGenError(true);
			-1;
		}
		int openCount = 0;
		for (int x = 0; x < map.size();x++) {
			if (map.at(x).empty()) {
				this->addErrorMessage(std::string("FindOpenCoordsCount: error 1 map collumn empty!").append(" empty y at x: ").append(std::to_string(x)).append("!"));
				this->setGenError(true);

				return -1;
			}
			for (int y = 0; y < map.at(x).size(); y++) {
				if (map[x][y] == type) {
					openCount++;
				}

			}
		}
		return openCount;
	}

	std::vector<std::vector<df::Vector>> MapBuilder::findZones(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId open) {
		if (map.empty()) {
			this->addErrorMessage("FindZones: error 0 map empty!");
			this->setGenError(true);
			return std::vector<std::vector<df::Vector>>();
		}

		std::vector<df::Vector> openCoords;

		std::vector<std::vector<mapTileIds::mapTileId>> tempMap=this->copyMap(map);

		std::vector<std::vector<df::Vector>> zones;

		std::vector<df::Vector> currentZone;

		df::Vector randomSeed;

		

		openCoords = this->findCoordsOfValue(tempMap,open);

		if (this->getGenError()) {
			this->addErrorMessage("FindZones: error 1 find open coords error!");
			return std::vector<std::vector<df::Vector>>();
		}


		while (!openCoords.empty()) {
			randomSeed = openCoords.at(std::rand() % openCoords.size());
			currentZone = this->floodFillReturnCoords(tempMap, randomSeed, mapTileIds::FLOOD, mapTileIds::EMPTY);
			if (this->getGenError()) {
				this->addErrorMessage("FindZones: error 2 flood fill failure!");
				return std::vector<std::vector<df::Vector>>();
			}
			zones.push_back(currentZone);
			currentZone.clear();
			openCoords = this->findCoordsOfValue(tempMap, open);

			if (this->getGenError()) {
				this->addErrorMessage("FindZones: error 3 find open coords error!");
				return std::vector<std::vector<df::Vector>>();
			}
		}

		return zones;
	}

	int MapBuilder::findLargestZone(std::vector<std::vector<df::Vector>>& zones) {
		if (zones.empty()) {
			this->addErrorMessage("findLargestZone: error 0 empty zones list!");
			this->setGenError(true);
			
			return -1;
		}
		int biggest = 0;
		int biggestZone = -1;
		for (int i = 0; i < zones.size(); i++) {
			if (zones[i].size() > biggest) {
				biggest = zones[i].size();
				biggestZone = i;
			}
		}

		if ((biggest <= 0 ) || (biggestZone == -1)) {
			this->addErrorMessage("findLargestZone: error 1 all zones empty!");
			this->setGenError(true);
			
			return -1;
		}

		return biggestZone;
	}

	float MapBuilder::findDistance(df::Vector p0, df::Vector p1) {
		float precursor = ((p1.getX()-p0.getX()) * (p1.getX() - p0.getX()))+ ((p1.getY()-p0.getY()) * (p1.getY() - p0.getY()));
		if (precursor < 0.000001f) {
			
			this->addErrorMessage(std::string("findDistance: error 0 potential deivide by zero or negative number! suspect precursor: ").append(std::to_string(precursor)).append("!"));
			this->setGenError(true);

			
			return -1;
		}
		return sqrtf(precursor);
	}


	df::Vector MapBuilder::findClosestPointInOtherZone(df::Vector targetPoint, std::vector<df::Vector>& otherPoints) {
		if (otherPoints.empty()) {
			this->addErrorMessage(std::string("find closest point in other zone: error 0 empty other points vector!"));
			this->setGenError(true);
			return df::Vector();
		}

		df::Vector closestPoint = otherPoints.at(0);
		float shortestDistance = this->findDistance(targetPoint,closestPoint);
		float testDistance = 0;
		for (int i = 0; i < otherPoints.size(); i++) {
			testDistance = this->findDistance(targetPoint, otherPoints[i]);
			if (this->getGenError()) {
				this->addErrorMessage(std::string("find closest point in other zone: error 1 failed distance calculation!"));
				return df::Vector();
			}
			if (testDistance < shortestDistance) {
				closestPoint = otherPoints[i];
			}
		}

		return closestPoint;
	}

	CoordinatePair MapBuilder::findSmallestConnectionLine(std::vector<df::Vector>& startZone, std::vector<std::vector<df::Vector>>& otherZones) {
		if (startZone.empty()) {
			this->addErrorMessage(std::string("findSmallestConnectionLine: error 0 empty start zone!"));
			this->setGenError(true);
			return CoordinatePair();
		}
		else if (otherZones.empty()) {
			this->addErrorMessage(std::string("findSmallestConnectionLine: error 1 empty other zones vector!"));
			this->setGenError(true);
			return CoordinatePair();
		}
		else if (otherZones[0].empty()) {
			this->addErrorMessage(std::string("findSmallestConnectionLine: error 2 empty other zone vector 0!"));
			this->setGenError(true);
			return CoordinatePair();
		}

		
		df::Vector overallShortestStartZonePoint=startZone[0];
		df::Vector overallShortestOtherZonePoint=otherZones[0][0];
		float overallShortestDistance = this->findDistance(overallShortestStartZonePoint,overallShortestOtherZonePoint);

		df::Vector startCheckPoint;
		df::Vector endCheckPoint;
		float checkDistance=0;
		int currentZone=0;
		for (int start = 0; start < startZone.size(); start++) {
			startCheckPoint = startZone[start];
			for (int zone = 0; zone < otherZones.size(); zone++) {
				currentZone = zone;
				if (otherZones.at(zone).empty()) {
					this->addErrorMessage(std::string("findSmallestConnectionLine: error 2 empty other zone!").append(" other zones index: ").append(std::to_string(zone)).append("!"));
					this->setGenError(true);
					return CoordinatePair();
				}
				for (int end = 0; end < otherZones[zone].size(); end++) {
					endCheckPoint = this->findClosestPointInOtherZone(startCheckPoint,otherZones[zone]);
					if (this->getGenError()) {
						this->addErrorMessage(std::string("findSmallestConnectionLine: error 3 closest point search failed!"));
						return CoordinatePair();
					}

					checkDistance = this->findDistance(startCheckPoint, endCheckPoint);
					if (this->getGenError()) {
						this->addErrorMessage(std::string("findSmallestConnectionLine: error 4 failed distance calculation!"));
						return CoordinatePair();
					}
					if (checkDistance < overallShortestDistance) {
						overallShortestStartZonePoint = startCheckPoint;
						overallShortestOtherZonePoint = endCheckPoint;
						overallShortestDistance = checkDistance;
					}
				}
			}
		}

		return CoordinatePair(overallShortestStartZonePoint, overallShortestOtherZonePoint);

	}

	int MapBuilder::eliminateDisperateZones(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId empty, MapGenConfig configObj) {
		if (map.empty()) {
			this->addErrorMessage(std::string("eliminateDisperateZones: error 0 empty map!"));
			this->setGenError(true);
			return -1;
		}
		df::Clock timer;
		timer.delta();

		std::vector<std::vector<df::Vector>> zones=this->findZones(map,empty);
		std::vector<df::Vector> startZone = zones[zones.size() - 1];
		zones.pop_back();
		std::vector<df::Vector> fixLine;
		
		
		if (this->getGenError()) {
			this->addErrorMessage(std::string("eliminateDisperateZones: error 1 zone search failed!"));
			return -1;
		}
		int timeout = configObj.getTimeoutSeconds();

		CoordinatePair fixPoints;
		while (zones.size() > 1) {
			if ((timer.split() / 1000) >= (33 * timeout * 60)) {
				this->addErrorMessage(std::string("eliminateDisperateZones: error 2 process timeout!"));
				return -1;
			}

			fixPoints = this->findSmallestConnectionLine(startZone, zones);
			if (this->getGenError()) {
				this->addErrorMessage(std::string("eliminateDisperateZones: error 3 connection line search failed!"));
				return -1;
			}


			fixLine = this->generateBresenhamLine(fixPoints.getPoint0().getX(), fixPoints.getPoint0().getY(), fixPoints.getPoint1().getX(), fixPoints.getPoint1().getY());

			this->drawLine(map, fixLine, empty);
			if (this->getGenError()) {
				this->addErrorMessage(std::string("eliminateDisperateZones: error 4 connection line draw failed!"));
				return -1;
			}
			zones = this->findZones(map, empty);
			startZone = zones[zones.size() - 1];
			zones.pop_back();
		}
		return 0;
	}

	std::vector<df::Vector> MapBuilder::getRandomCoordListOfValue(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId open, MapGenConfig config) {
		if (map.empty()) {
			this->addErrorMessage(std::string("generateRandomOpenCoordList: error 0 empty map!"));
			this->setGenError(true);
			return std::vector<df::Vector>();
		}
		std::vector<df::Vector> openSpace = this->findCoordsOfValue(map, open);
		
		if (this->getGenError()) {
			this->addErrorMessage(std::string("generateRandomOpenCoordList: error 1 retreive empty space failed!"));
			return std::vector<df::Vector>();
		}
		if (openSpace.empty()) {
			this->addErrorMessage(std::string("generateRandomOpenCoordList: error 2 no open space!"));
			this->setGenError(true);
			return std::vector<df::Vector>();
		}
		std::shuffle(openSpace.begin(), openSpace.end(), std::default_random_engine(config.getRandomSeed()));

		return openSpace;
	}

	df::Vector MapBuilder::getRandomCoordOfValue(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId open) {
		if (map.empty()) {
			this->addErrorMessage(std::string("generateRandomOpenCoord: error 0 empty map!"));
			this->setGenError(true);
			return df::Vector();
		}
		std::vector<df::Vector> openSpace = this->findCoordsOfValue(map, open);

		if (this->getGenError()) {
			this->addErrorMessage(std::string("generateRandomOpenCoord: error 1 retreive empty space failed!"));
			return df::Vector();
		}
		if (openSpace.empty()) {
			this->addErrorMessage(std::string("generateRandomOpenCoord: error 2 no open space!"));
			this->setGenError(true);
			return df::Vector();
		}
		return openSpace[rand() % openSpace.size()];
	}
	

	int MapBuilder::sprinkleTrees(std::vector<std::vector<mapTileIds::mapTileId>>& map, int trees, mapTileIds::mapTileId open, mapTileIds::mapTileId tree, MapGenConfig config) {
		if (map.empty()) {
			this->addErrorMessage(std::string("sprinkleTrees: error 0 empty map!"));
			this->setGenError(true);
			return -1;
		}
		
		std::vector<df::Vector> openSpace = this->getRandomCoordListOfValue(map, open, config);
		if (this->getGenError()) {
			this->addErrorMessage(std::string("sprinkleTrees: error 1 random open coord list genration failed!"));
			return -1;
		}

		if (trees > openSpace.size()) {
			this->addErrorMessage(std::string("sprinkleTrees: error 2 too little open space for specified trees! values: open space: ").append(std::to_string(openSpace.size())).append(" trees: ").append(std::to_string(trees)).append("!"));
			this->setGenError(true);
			return -1;
		}


		for (int treeIndex = 0; treeIndex < trees; treeIndex++) {
			if ((((int)openSpace[treeIndex].getX()) >= map.size()) || (((int)openSpace[treeIndex].getX()) < 0)) {
				this->addErrorMessage(std::string("sprinkleTrees: error 3 map x smaller than random open coord x or random open coord x less than zero! values: map x: ").append(std::to_string(map.size())).append(" coord x: ").append(std::to_string((int)openSpace[treeIndex].getX())).append("!"));

				this->setGenError(true);
				return -1;
			}
			if (map[(int)openSpace[treeIndex].getX()].size() <= (int)openSpace[treeIndex].getY()) {
				this->addErrorMessage(std::string("sprinkleTrees: error 4 map y smaller than random open coord y! values: small y x:").append(std::to_string((int)openSpace[treeIndex].getX())).append(" map y size: ").append(std::to_string(map[(int)openSpace[treeIndex].getX()].size())).append(" coord y: ").append(std::to_string((int)openSpace[treeIndex].getY())).append("!"));

				this->setGenError(true);
				return -1;
			}
			map[(int)openSpace[treeIndex].getX()][(int)openSpace[treeIndex].getY()] = tree;
		}
		return 0;
	}

	int MapBuilder::sprinkleSeeds(std::vector<std::vector<mapTileIds::mapTileId>>& map, int seeds, mapTileIds::mapTileId open, mapTileIds::mapTileId seed, MapGenConfig config){
		if (map.empty()) {
			this->addErrorMessage(std::string("sprinkeSeeds: error 0 empty map!"));
			this->setGenError(true);
			return -1;
		}

		std::vector<df::Vector> openSpace = this->getRandomCoordListOfValue(map, open, config);
		if (this->getGenError()) {
			this->addErrorMessage(std::string("sprinkleSeeds: error 1 random open coord list genration failed!"));
			return -1;
		}

		if (seeds > openSpace.size()) {
			this->addErrorMessage(std::string("sprinkleSeeds: error 2 too little open space for specified trees! values: open space: ").append(std::to_string(openSpace.size())).append(" seeds: ").append(std::to_string(seeds)).append("!"));
			this->setGenError(true);
			return -1;
		}


		for (int treeIndex = 0; treeIndex < seeds; treeIndex++) {
			if ((((int)openSpace[treeIndex].getX()) >= map.size())|| (((int)openSpace[treeIndex].getX()) < 0)) {
				this->addErrorMessage(std::string("sprinkleSeeds: error 3 map x smaller than random open coord x or random open cooord x less than zero! values: map x: ").append(std::to_string(map.size())).append(" coord x: ").append(std::to_string((int)openSpace[treeIndex].getX())).append("!"));

				this->setGenError(true);
				return -1;
			}
			if (map[(int)openSpace[treeIndex].getX()].size() <= (int)openSpace[treeIndex].getY()) {
				this->addErrorMessage(std::string("sprinkleSeeds: error 4 map y smaller than random open coord y! values: small y x:").append(std::to_string((int)openSpace[treeIndex].getX())).append(" map y size: ").append(std::to_string(map[(int)openSpace[treeIndex].getX()].size())).append(" coord y: ").append(std::to_string((int)openSpace[treeIndex].getY())).append("!"));

				this->setGenError(true);
				return -1;
			}
			map[(int)openSpace[treeIndex].getX()][(int)openSpace[treeIndex].getY()] = seed;
		}
		return 0;
	}

	int MapBuilder::placeOwl(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId open, mapTileIds::mapTileId owl) {

		if (map.empty()) {
			this->addErrorMessage(std::string("placeOwl: error 0 empty map!"));
			this->setGenError(true);
			return -1;
		}

		df::Vector openPos = this->getRandomCoordOfValue(map,open);
		if (this->getGenError()) {
			this->addErrorMessage(std::string("placeOwl: error 1 open coord generation failed!"));
			return -1;
		}

		if ((((int)openPos.getX()) < 0) || (((int)openPos.getX()) >= map.size()) ) {
			this->addErrorMessage(std::string("placeOwl: error 2 random open pos x invalid! values: pos x: ").append(std::to_string(((int)openPos.getX()))).append(" map x: ").append(std::to_string(map.size())).append("!"));
			this->setGenError(true);
			return -1;
		}
		if ((((int)openPos.getY()) < 0) || (((int)openPos.getY()) >= map[((int)openPos.getX())].size())) {
			this->addErrorMessage(std::string("placeOwl: error 3 random open pos Y invalid! values: pos y: ").append(std::to_string(((int)openPos.getY()))).append(" map Y: ").append(std::to_string(map[(int)openPos.getX()].size())).append("!"));
			this->setGenError(true);
			return -1;
		}


		map[((int)openPos.getX())][(int)openPos.getY()] = owl;
		return 0;
	}

	int MapBuilder::placeExit(std::vector < std::vector < mapTileIds::mapTileId >> &map, mapTileIds::mapTileId open, mapTileIds::mapTileId exit) {
		if (map.empty()) {
			this->addErrorMessage(std::string("placeExit: error 0 empty map!"));
			this->setGenError(true);
			return -1;
		}

		df::Vector openPos = this->getRandomCoordOfValue(map, open);
		if (this->getGenError()) {
			this->addErrorMessage(std::string("placeExit: error 1 open coord generation failed!"));
			return -1;
		}

		if ((((int)openPos.getX()) < 0) || (((int)openPos.getX()) >= map.size())) {
			this->addErrorMessage(std::string("placeExit: error 2 random open pos x invalid! values: pos x: ").append(std::to_string(((int)openPos.getX()))).append(" map x: ").append(std::to_string(map.size())).append("!"));
			this->setGenError(true);
			return -1;
		}
		if ((((int)openPos.getY()) < 0) || (((int)openPos.getY()) >= map[((int)openPos.getX())].size())) {
			this->addErrorMessage(std::string("placeExit: error 3 random open pos Y invalid! values: pos y: ").append(std::to_string(((int)openPos.getY()))).append(" map Y: ").append(std::to_string(map[(int)openPos.getX()].size())).append("!"));
			this->setGenError(true);
			return -1;
		}


		map[((int)openPos.getX())][(int)openPos.getY()] = exit;
		return 0;
	}

	

	

	int MapBuilder::squarePlot(std::vector<std::vector<mapTileIds::mapTileId>>& map, df::Vector location, mapTileIds::mapTileId value) {
		if (map.empty()) {
			this->addErrorMessage(std::string("squarePlot: error 0 empty map!"));
			this->setGenError(true);
			return -1;
		}
		if ((((int)location.getX()) < 0) || (((int)location.getX()) >= map.size())) {
			this->addErrorMessage(std::string("squarePlot: error 1 plot corner 0 x invalid! values: pos x: ").append(std::to_string(((int)location.getX()))).append(" map x: ").append(std::to_string(map.size())).append("!"));
			this->setGenError(true);
			return -1;
		}
		if ((((int)location.getY()) < 0) || (((int)location.getY()) >= map[((int)location.getX())].size())) {
			this->addErrorMessage(std::string("squarePlot: error 2 corner 0 Y invalid! values: pos y: ").append(std::to_string(((int)location.getY()))).append(" map Y: ").append(std::to_string(map[(int)location.getX()].size())).append("!"));
			this->setGenError(true);
			return -1;
		}
		if ((((int)location.getX())+1 >= map.size())) {
			this->addErrorMessage(std::string("squarePlot: error 1 corner 1 x invalid! values: pos x: ").append(std::to_string(((int)location.getX()))).append(" map x: ").append(std::to_string(map.size())).append("!"));
			this->setGenError(true);
			return -1;
		}
		if ((((int)location.getY())+1 >= map[((int)location.getX())].size())) {
			this->addErrorMessage(std::string("squarePlot: error 2 corner 1 Y invalid! values: pos y: ").append(std::to_string(((int)location.getY()))).append(" map Y: ").append(std::to_string(map[(int)location.getX()].size())).append("!"));
			this->setGenError(true);
			return -1;
		}

		map[((int)location.getX())][((int)location.getY())] = value;
		map[((int)location.getX())+1][((int)location.getY())] = value;
		map[((int)location.getX())][((int)location.getY())+1] = value;
		map[((int)location.getX())+1][((int)location.getY())+1] = value;
		return 0;
	}
	
	int MapBuilder::calculateNeededOpenSpaces(int seeds) {
		return seeds + 2;
	}

	df::Vector MapBuilder::getXYDistanceBetweenTwoPoint(df::Vector p0, df::Vector p1) {
		return df::Vector(abs(p0.getX() - p1.getX()), abs(p0.getY() - p1.getY()));
	}


	CoordinatePair  MapBuilder::findAngleLineClosestToDistance(std::vector<std::vector<mapTileIds::mapTileId>>& map, int dist, mapTileIds::mapTileId pointType,  MapGenConfig config) {
		if (map.empty()) {

			this->addErrorMessage(std::string("findAngleLineClosestToDistance: error 0 empty map!"));
			this->setGenError(true);
			return CoordinatePair();

		}

		if (dist < 1) {
			this->addErrorMessage(std::string("findAngleLineClosestToDistance: error 1 invalid distance of: ").append(std::to_string(dist)).append("!"));
			this->setGenError(true);
			return CoordinatePair();
		}

		std::vector < df::Vector> avalablePoint = this->getRandomCoordListOfValue(map, pointType,config);
		if (this->getGenError()) {
			this->addErrorMessage(std::string("findAngleLineClosestToDistance: error 2 avalable point retreival failed!"));
			return CoordinatePair();
		}

		if (avalablePoint.size() < 2) {
			this->addErrorMessage(std::string("findAngleLineClosestToDistance: error 4 too few avalable points! avalable points: ").append(std::to_string(avalablePoint.size())).append("!"));
			this->setGenError(true);
			return CoordinatePair();
		}

		df::Vector closestStartPoint0;
		df::Vector closestStartPoint1;

		df::Vector testPoint;
		df::Vector testPoint2;

		df::Vector resultPoint;

		float closestDistance;

		float testDistance;

		for (int point0 = 0; point0 < avalablePoint.size(); point0++) {
			for (int point1 = 0; point1 < avalablePoint.size(); point1++) {
				testPoint = avalablePoint[point0];
				testPoint2 = avalablePoint[point1];
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
	CoordinatePair  MapBuilder::findDiagLineClosestToDistance(std::vector<std::vector<mapTileIds::mapTileId>>& map, int dist, mapTileIds::mapTileId pointType, MapGenConfig config) {
		if (map.empty()) {

			this->addErrorMessage(std::string("findAngleLineClosestToDistance: error 0 empty map!"));
			this->setGenError(true);
			return CoordinatePair();

		}

		if (dist < 1) {
			this->addErrorMessage(std::string("findAngleLineClosestToDistance: error 1 invalid distance of: ").append(std::to_string(dist)).append("!"));
			this->setGenError(true);
			return CoordinatePair();
		}

		std::vector < df::Vector> avalablePoint = this->getRandomCoordListOfValue(map, pointType,config);
		if (this->getGenError()) {
			this->addErrorMessage(std::string("findAngleLineClosestToDistance: error 2 avalable point retreival failed!"));
			return CoordinatePair();
		}

		if (avalablePoint.size() < 2) {
			this->addErrorMessage(std::string("findAngleLineClosestToDistance: error 4 too few avalable points! avalable points: ").append(std::to_string(avalablePoint.size())).append("!"));
			this->setGenError(true);
			return CoordinatePair();
		}

		df::Vector closestStartPoint0;
		df::Vector closestStartPoint1;

		df::Vector testPoint;
		df::Vector testPoint2;

		float resultdist;

		float closestDistance;
		testPoint = avalablePoint[0];
		testPoint2 = avalablePoint[1];
		closestStartPoint0 = testPoint;
		closestStartPoint1 = testPoint2;
		closestDistance = this->findDistance(testPoint, testPoint2);
		if (this->getGenError()) {
			this->addErrorMessage(std::string("findAngleLineClosestToDistance: error 5  distance retreival failed!"));
			
			return CoordinatePair();
		}

		for (int point0 = 0; point0 < avalablePoint.size(); point0++) {
			for (int point1 = 0; point1 < avalablePoint.size(); point1++) {
				testPoint = avalablePoint[point0];
				testPoint2 = avalablePoint[point1];
				if (testPoint != testPoint2) {
					resultdist = this->findDistance(testPoint, testPoint2);
					if (this->getGenError()) {
						this->addErrorMessage(std::string("findAngleLineClosestToDistance: error 5  distance retreival failed!"));

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

	int MapBuilder::ensureSpace(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId empty, mapTileIds::mapTileId tree,  int neededOpenSpaces, MapGenConfig config) {
		if (map.empty()) {
			
				this->addErrorMessage(std::string("ensureSpace: error 0 empty map!"));
				this->setGenError(true);
				return -1;
			
		}



		int currentOpen = this->findCoordsOfValueCount(map, empty);
		if (this->getGenError()) {
			this->addErrorMessage(std::string("ensureSpace: error 1 open count retreival failed!"));
			return -1;
		}
		if (neededOpenSpaces < 3) {
			this->addErrorMessage(std::string("ensureSpace: error 2 invalid meeded open of: ").append(std::to_string(neededOpenSpaces)).append("!"));
			this->setGenError(true);
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


		while (currentOpen < neededOpenSpaces) {

			
			ClosedCoordsList= this->getRandomCoordListOfValue(map,tree,config);
			if (this->getGenError()) {
				this->addErrorMessage(std::string("ensureSpace: error 3 closed coords retreival failed!"));
				return -1;
			}
			if (ClosedCoordsList.empty()) {
				this->addErrorMessage(std::string("ensureSpace: error 4 empty closed spaces list!"));
				this->setGenError(true);
				return -1;
			}
			if (ClosedCoordsList.size()<neededOpenSpaces-currentOpen) {
				this->addErrorMessage(std::string("ensureSpace: error 5 too small closed spaces list! size: ").append(std::to_string(ClosedCoordsList.size())).append(" needed open: ").append(std::to_string(neededOpenSpaces-currentOpen)).append("!"));
				this->setGenError(true);
				return -1;
			}
			
			mode = rand() % 4;
			df::Vector closeCoord;
			switch (mode) {
				case 0:
					//line mode
					targetPair = this->findDiagLineClosestToDistance(map, neededOpenSpaces-currentOpen, empty, config);
					if (this->getGenError()) {
						this->addErrorMessage(std::string("ensureSpace: error 6 draw points retreival failed!"));


						return -1;
					}
					repairSpaces = this->generateBresenhamLine(targetPair.getPoint0().getX(), targetPair.getPoint0().getY(), targetPair.getPoint1().getX(), targetPair.getPoint1().getY());
					this->drawLine(map, repairSpaces, empty);
					if (this->getGenError()) {
						this->addErrorMessage(std::string("ensureSpace: error 7 draw failed!"));


						return -1;
					}

					break;
				case 1:
					//xy line mode
					targetPair = this->findAngleLineClosestToDistance(map, neededOpenSpaces - currentOpen, empty, config);
					if (this->getGenError()) {
						this->addErrorMessage(std::string("ensureSpace: error 8 draw points retreival failed!"));


						return -1;
					}
					bool yFirst = false;
					bool point1First = false;
					if ((rand() % 2)==0) {
						yFirst = true;
					}
					if ((rand() % 2) == 0) {
						point1First = true;
					}
					repairSpaces = this->generateXYLine(targetPair.getPoint0().getX(), targetPair.getPoint0().getY(), targetPair.getPoint1().getX(), targetPair.getPoint1().getY(), yFirst, point1First);
					this->drawLine(map, repairSpaces, empty);
					if (this->getGenError()) {
						this->addErrorMessage(std::string("ensureSpace: error 9 draw failed!"));


						return -1;
					}
					break;

				case 2:
					///rect mode
					targetPair = this->findAngleLineClosestToDistance(map, neededOpenSpaces - currentOpen, empty, config);
					if (this->getGenError()) {
						this->addErrorMessage(std::string("ensureSpace: error 10 draw points retreival failed!"));


						return -1;
					}
					int startx = targetPair.getPoint0().getX();
					int endx = targetPair.getPoint1().getX();
					int starty = targetPair.getPoint0().getY();
					int endy = targetPair.getPoint1().getX();
					int width = 0;
					int height = 0;
					int temp = 0;
					if (startx > endx) {
						temp = startx;
						startx = endx;
						endx = temp;
						width = endx - startx;
					}
					if (starty > endy) {
						temp = starty;
						starty = endy;
						endy = temp;
						height = endy - starty;
					}
					
					repairbox = df::Box(df::Vector(startx, starty), width, height);

					this->drawRectangle(map, repairbox, empty);

					if (this->getGenError()) {
						this->addErrorMessage(std::string("ensureSpace: error 11 draw failed!"));


						return -1;
					}
					int area = width * height;
					int addTrees = area - (neededOpenSpaces - currentOpen);

					this->sprinkleTrees(map, addTrees, empty, tree, config);
					if (this->getGenError()) {
						this->addErrorMessage(std::string("ensureSpace: error 12 draw failed!"));


						return -1;
					}
					break;
				case 3:
					//random point mode;
					for (int i = 0; i < neededOpenSpaces; i++) {
						closeCoord = ClosedCoordsList[i];
						if ((closeCoord.getX() < 0) || (closeCoord.getX() >= map.size())) {
							this->addErrorMessage(std::string("ensureSpace: error 13 invalid target coord x: ").append(std::to_string(closeCoord.getX())).append(" map width: ").append(std::to_string(map.size())).append("!"));
							this->setGenError(true);
						}
						if ((closeCoord.getY() < 0) || (closeCoord.getY() >= map.size())) {
							this->addErrorMessage(std::string("ensureSpace: error 14 invalid target coord y: ").append(std::to_string(closeCoord.getY())).append(" map width: ").append(std::to_string(map[(int)closeCoord.getX()].size())).append("!"));
							this->setGenError(true);
						}
						map[(int)closeCoord.getX()][(int)closeCoord.getY()] = empty;
						
					}
					break;
				default:
					this->addErrorMessage(std::string("ensureSpace: error 15 invalid random repair mode!"));
					this->setGenError(true);
					return -1;
			}

			currentOpen = this->findCoordsOfValueCount(map, empty);

			if (this->getGenError()) {
				this->addErrorMessage(std::string("ensureSpace: error 16 open count retreival failed!"));
				return -1;
			}
		}

		return 0;
	}


	void MapBuilder::generateMap() {
		if (m_configObj.getRandomSeed() != 0) {
			srand(m_configObj.getRandomSeed());
			
		}
		else {
			m_configObj.setRandomSeed(rand());
		}
		int mapBorderThickness = this->m_configObj.getMapBorderThickness();
		if (mapBorderThickness < 1) {
			this->addErrorMessage(std::string("error: invalid configured map border thickeness. value is: ").append(std::to_string(mapBorderThickness)));
			this->setGenError(true);
			this->setBaseFunctionExit(true);
			return;
		}
		int mapHeight = this->m_configObj.getMapHeight();
		if (mapHeight < 1) {
			this->addErrorMessage(std::string("error: invalid configured map border thickeness. value is: ").append(std::to_string(mapHeight)));
			this->setGenError(true);
			this->setBaseFunctionExit(true);
			return;
		}

		//put this before every return, otherwise bad things start happening
		this->setBaseFunctionExit(true);
		return;
	}

	df::ObjectList MapBuilder::buildMap(MapGenConfig config, std::vector<std::vector<mapTileIds::mapTileId>>& map, df::Object* owl) {

		if (map.empty()) {
			this->addErrorMessage("buildMap: error 0 map empty!");
			this->setGenError(true);
			return df::ObjectList();
		}

		df::ObjectList mapObjects;
		int borderThickness = config.getMapBorderThickness();
		df::Vector start = config.getMapOrigin();
		df::Vector currentPos = df::Vector();
		int tileWidth = config.getMapObjectWidth();
		int tileHeight = config.getMapObjectHeight();

		int playAreaWidth = config.getMapWidth();
		int playAreaHeight = config.getMapHeight();

		int altitude = config.getMapObjectAltitude();

		if ((playAreaWidth <= 0)) {
			this->addErrorMessage(std::string("buildMap: error 1 invalid play area width of: ").append(std::to_string(playAreaWidth)).append("!"));
			this->setGenError(true);
			return df::ObjectList();
		}
		else if ((playAreaHeight <= 0)) {
			this->addErrorMessage(std::string("buildMap: error 2 invalid play area height of: ").append(std::to_string(playAreaHeight)).append("!"));
			this->setGenError(true);
			return df::ObjectList();
		}
		else if ((borderThickness < 0)) {
			this->addErrorMessage(std::string("buildMap: error 3 invalid border thickness of: ").append(std::to_string(borderThickness)).append("!"));
			this->setGenError(true);
			return df::ObjectList();
		}
		else if ((tileWidth < 1)) {
			this->addErrorMessage(std::string("buildMap: error 4 invalid tile width of: ").append(std::to_string(tileWidth)).append("!"));
			this->setGenError(true);
			return df::ObjectList();
		}
		else if ((tileHeight < 1)) {
			this->addErrorMessage(std::string("buildMap: error 5 invalid tile height of: ").append(std::to_string(tileHeight)).append("!"));
			this->setGenError(true);
			return df::ObjectList();
		}
		else if ((altitude < 0) || (altitude > df::MAX_ALTITUDE)) {
			this->addErrorMessage(std::string("buildMap: error 6 invalid tile height of: ").append(std::to_string(altitude)).append("!"));
			this->setGenError(true);
			return df::ObjectList();
		}
		else if (playAreaWidth <= map.size()) {
			this->addErrorMessage(std::string("buildMap: error 7 invalid map size of: ").append(std::to_string(map.size())).append(" configured width: ").append(std::to_string(playAreaWidth)).append("!"));
			this->setGenError(true);
			return df::ObjectList();
		}
		df::Object* newGround=nullptr;
		df::Object* newTree=nullptr;
		df::Object* newExit = nullptr;
		df::Object* newSeed = nullptr;
		for (int x = 0; x < playAreaWidth + (borderThickness * 2);x++) {
			

			for (int y = 0; y < playAreaHeight + (borderThickness * 2); y++) {
				currentPos.setY(currentPos.getY() + tileHeight);
				if (((x < borderThickness) || (x > borderThickness + playAreaWidth)) || ((y < borderThickness) || (y > borderThickness + playAreaHeight))) {
					
					newTree= new Tree();
					newTree->setAltitude(0);
					newTree->setPosition(start + currentPos);
					mapObjects.insert(newTree);
					
				}
				else {

					if (map[x - borderThickness].size()>=playAreaHeight) {
						df::WorldManager& wm = df::WorldManager::getInstance();
						for (int i = 0; i < mapObjects.getCount(); i++) {
							wm.markForDelete(mapObjects[i]);
						}
						this->addErrorMessage(std::string("buildMap: error 8 map Y vector less than configured!").append(" values: map y: ").append(std::to_string(map[x - borderThickness].size())).append(" configured y: ").append(std::to_string(playAreaHeight)).append("!"));
						this->setGenError(true);
						return df::ObjectList();
					}

					switch (map[(x) - borderThickness][(y) - borderThickness]) {
						case mapTileIds::EMPTY:
							newGround =new Ground();
							newGround->setAltitude(0);
							newGround->setPosition(currentPos + start);
							mapObjects.insert(newGround);
							break;
						case mapTileIds::OWL:
							owl->setPosition(currentPos + start);
							break;
						case mapTileIds::EXIT:
							newExit= new MapExit();
							newExit->setAltitude(0);
							newExit->setPosition(currentPos + start);
							mapObjects.insert(newExit);
							break;
						case mapTileIds::SEED:
							newSeed = new Seed();
							newSeed-> setAltitude(0);
							newSeed->setPosition(currentPos + start);
							mapObjects.insert(newSeed);
							break;
						case mapTileIds::TREE:
							newTree = new Tree();
							newTree->setAltitude(0);
							newTree->setPosition(start + currentPos);
							mapObjects.insert(newTree);
							break;
						default:
							df::WorldManager& wm = df::WorldManager::getInstance();
							for (int i = 0; i < mapObjects.getCount(); i++) {
								wm.markForDelete(mapObjects[i]);
							}
							this->addErrorMessage(std::string("buildMap: error 9 invalid tile id!").append(" values: id Value: ").append(std::to_string((int)map[(x)-borderThickness][(y)-borderThickness])).append("!"));
							this->setGenError(true);
							return df::ObjectList();

					}
				}
			}
			currentPos.setY(0);
			currentPos.setX(currentPos.getX() + tileWidth);
		}

		mapObjects.insert(owl);
		return mapObjects;
	}


	int MapBuilder::startGenerateMap(MapGenConfig config, df::Object* owl){
		if (!m_generating) {
			m_genDone = false;
			m_generating = true;
			m_configObj = config;
			m_player = owl;
			m_gen_error = false;
			m_error_messages = std::vector < std::string>();
			m_error_gate.release();
			m_base_function_exit = false;
			m_genThread = &std::thread(generateMap, this);
			return 0;
			
			
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
		m_map_plan = std::vector<std::vector<mapTileIds::mapTileId>>();
		m_error_gate.release();
		m_error_messages=std::vector<std::string>();
		m_player = nullptr;
		m_genThread = nullptr;
		m_genTime = 0;
		m_gen_error = false;
		m_base_function_exit = false;
		this->setCameraAffected(false);
		this->setPosition(df::Vector(40, 12));
		this->setType("mapBuilder");
	}
	MapBuilder::~MapBuilder() {
		m_genThread->join();
	}


	void MapBuilder::setBaseFunctionExit(bool new_base_function_exit) {
		m_error_gate.acquire();
		m_base_function_exit = new_base_function_exit;
		m_error_gate.release();
	}
	bool MapBuilder::getBaseFunctionExit() {
		bool exitStateTemp;
		m_error_gate.acquire();
		exitStateTemp = m_base_function_exit;
		m_error_gate.release();
		return exitStateTemp;
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
				if(m_gen_error){
					if (this->getBaseFunctionExit()) {
						df::GameManager& gm = df::GameManager::getInstance();
						df::LogManager& lm = df::LogManager::getInstance();
						std::vector < std::string> errorMessages = this->getErrorMessages();
						for (int i = 0; i < errorMessages.size(); i++) {
							lm.writeLog(m_error_messages[i].c_str());
						}

						EventMapGenDone done = EventMapGenDone(errorMessages);
						gm.onEvent(&done);
						m_done_sent = true;
					}
					
				}

				else if (!m_generating) {
					if (m_genDone) {
						m_genThread->join();
						m_genThread = nullptr;
						m_mapReturn = this->buildMap(m_configObj,m_map_plan, m_player);
						if (m_gen_error) {
							df::GameManager& gm = df::GameManager::getInstance();
							df::LogManager& lm = df::LogManager::getInstance();
							std::vector < std::string> errorMessages = this->getErrorMessages();
							for (int i = 0; i < errorMessages.size(); i++) {
								lm.writeLog(m_error_messages[i].c_str());
							}

							EventMapGenDone done = EventMapGenDone(errorMessages);
							gm.onEvent(&done);
							m_done_sent = true;
						}
						else {
							df::GameManager& gm = df::GameManager::getInstance();
							EventMapGenDone done = EventMapGenDone(m_mapReturn, m_genTime);
							gm.onEvent(&done);
							m_done_sent = true;
						}
						
					}
				}
			}
			return 1;
		}
		return 0;
	}

	int MapBuilder::draw() {
		if (this->getVisible()) {
			df::DisplayManager& dm = df::DisplayManager::getInstance();
			if (m_gen_error) {
				return dm.drawString(this->getPosition(), "map generation ERROR!", df::CENTER_JUSTIFIED, df::WHITE);
			}
			else if (m_genDone) {
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


	void MapBuilder::addErrorMessage(std::string new_error_message) {
		m_error_gate.acquire();
		m_error_messages.push_back(new_error_message);
		m_error_gate.release();
	}

	void MapBuilder::resetErrorMessage() {
		m_error_gate.acquire();
		m_error_messages.clear();
		m_error_gate.release();
	}
	void MapBuilder::setErrorMessages(std::vector<std::string> new_error_messages) {
		m_error_gate.acquire();
		m_error_messages=new_error_messages;
		m_error_gate.release();
	}
	std::vector<std::string> MapBuilder::getErrorMessages() {
		std::vector<std::string> errorMessageCache;
		m_error_gate.acquire();
		errorMessageCache = m_error_messages;
		m_error_gate.release();
		return errorMessageCache;
	}

	void MapBuilder::setGenError(bool new_gen_error) {
		m_error_gate.acquire();
		m_gen_error = new_gen_error;
		m_error_gate.release();
		
	}
	bool MapBuilder::getGenError() {
		bool tempError;
		m_error_gate.acquire();
		tempError = m_gen_error;
		m_error_gate.release();

		return tempError;
	}
}