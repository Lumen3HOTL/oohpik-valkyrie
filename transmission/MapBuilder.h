#pragma once
#include "Object.h"
#include "ObjectList.h"
#include <vector>
#include "MapGenConfigObj.h"
#include "CoordinatePair.h"
#include <random>
#include "Clock.h"
#include "WorldManager.h"
#include <thread>
#include "DisplayManager.h"
#include "EventStep.h"
#include "GameManager.h"
#include "EventMapGenDone.h"
#include <unordered_set>
namespace ookpik {
	namespace mapTileIds {
		enum mapTileId {
			TILE_ERROR,
			EMPTY,
			TREE,
			SEED,
			OWL,
			EXIT,
			FLOOD
		};
	}
	
	class MapBuilder : public df::Object {
	private:
		MapGenConfig m_configObj;
		int m_needed_empty_spaces;
		df::ObjectList m_mapReturn;
		bool m_generating;
		bool m_genDone;
		bool m_done_sent;
		std::thread* m_genThread;
		unsigned long long m_genTime;
		df::Clock m_timer;
		df::Object* m_player;
		

		std::vector<df::Vector> generateBresenhamLine(int x1, int y1, int x2, int y2);

		std::vector<df::Vector> generateXYLine(int x1, int y1, int x2, int y2, bool yFirst);

		std::vector<std::vector<mapTileIds::mapTileId>> createStartingMap(int width, int height, mapTileIds::mapTileId canvas);

		std::vector<std::vector<mapTileIds::mapTileId>> copyMap(std::vector<std::vector<mapTileIds::mapTileId>> &map);

		int drawLine(std::vector<std::vector<mapTileIds::mapTileId>> &map, std::vector<df::Vector> &line, mapTileIds::mapTileId type);

		int drawSquare(std::vector<std::vector<mapTileIds::mapTileId>>& map, df::Box square, mapTileIds::mapTileId value);

		int floodFill(std::vector<std::vector<mapTileIds::mapTileId>> &map, df::Vector location, mapTileIds::mapTileId value);

		std::vector<df::Vector> floodFillReturnCoords(std::vector<std::vector<int>>& map, df::Vector location, mapTileIds::mapTileId value);

		std::vector<df::Vector> findOpenCoords(std::vector<std::vector<mapTileIds::mapTileId>> &map, mapTileIds::mapTileId open);

		int findOpenCoordsCount(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId open);

		std::vector<std::vector<df::Vector>> findZones(std::vector<std::vector<mapTileIds::mapTileId>> &map, mapTileIds::mapTileId open);
		
		int findLargestZone(std::vector<std::vector<df::Vector>> zones);

		df::Vector findClosestPointInOtherZone(df::Vector targetPoint, std::vector<df::Vector> &otherPoints);

		CoordinatePair findSmallestConnectionLine(std::vector<df::Vector>& startZone, std::vector<std::vector<df::Vector>>& otherZones);

		int sprinkleTrees(std::vector<std::vector<mapTileIds::mapTileId>>& map,int trees);

		int sprinkleSeeds(std::vector<std::vector<mapTileIds::mapTileId>>& map,int seeds);

		int placeOwl(std::vector<std::vector<mapTileIds::mapTileId>>& map);

		int placeExit(std::vector<std::vector<int>>& map);

		int squarePlot(std::vector<std::vector<int>>& map, df::Vector location, mapTileIds::mapTileId value);

		df::ObjectList buildMap(std::vector<std::vector<mapTileIds::mapTileId>>& map, df::Object* owl);

		df::Vector findRandomOpenCoord(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId open);

		int calculateNeededOpenSpaces(int seeds);

		void generateMap();
		

		

		

	public:

		
		int startGenerateMap(MapGenConfig config, df::Object* owl);

		bool isMapGenFinished()const;

		int destroyMap(df::ObjectList map);

		int eventHandler(df::Event* m_p);

		int draw();
	
		MapBuilder();
		~MapBuilder();


	};
}
