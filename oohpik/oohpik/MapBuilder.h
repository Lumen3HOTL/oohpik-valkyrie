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
#include <queue>
#include <math.h>
#include "LogManager.h"
#include <semaphore>

#include "MapExit.h"
#include "Tree.h"
#include "Ground.h"
#include "Seed.h"
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
		std::vector<std::vector<mapTileIds::mapTileId>> m_map_plan;
		bool m_gen_error;
		std::vector<std::string> m_error_messages;
		std::binary_semaphore m_error_gate=std::binary_semaphore(1);
		bool m_base_function_exit;


		void setBaseFunctionExit(bool new_base_function_exit);
		bool getBaseFunctionExit();

		void addErrorMessage(std::string new_error_message);
		void resetErrorMessage();
		void setErrorMessages(std::vector<std::string> new_error_messages);
		std::vector<std::string> getErrorMessages();

		void setGenError(bool new_gen_error);
		bool getGenError();

		float findDistance(df::Vector p0, df::Vector p1);

		int eliminateDisperateZones(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId empty, MapGenConfig configObj);

		std::vector<df::Vector> generateBresenhamLine(int x1, int y1, int x2, int y2);

		std::vector<df::Vector> generateXYLine(int x1, int y1, int x2, int y2, bool yFirst, bool point1First);

		std::vector<std::vector<mapTileIds::mapTileId>> createStartingMap(int width, int height, mapTileIds::mapTileId canvas);

		std::vector<std::vector<mapTileIds::mapTileId>> copyMap(std::vector<std::vector<mapTileIds::mapTileId>> &map);

		int drawLine(std::vector<std::vector<mapTileIds::mapTileId>> &map, std::vector<df::Vector> &line, mapTileIds::mapTileId type);

		int drawRectangle(std::vector<std::vector<mapTileIds::mapTileId>>& map, df::Box square, mapTileIds::mapTileId value);

		int floodFill(std::vector<std::vector<mapTileIds::mapTileId>> &map, df::Vector location, mapTileIds::mapTileId fillValue, mapTileIds::mapTileId emptyValue);

		std::vector<df::Vector> floodFillReturnCoords(std::vector<std::vector<mapTileIds::mapTileId>>& map, df::Vector location, mapTileIds::mapTileId fillValue, mapTileIds::mapTileId emptyValue);

		std::vector<df::Vector> findCoordsOfValue(std::vector<std::vector<mapTileIds::mapTileId>> &map, mapTileIds::mapTileId open);

		int findCoordsOfValueCount(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId open);

		std::vector<std::vector<df::Vector>> findZones(std::vector<std::vector<mapTileIds::mapTileId>> &map, mapTileIds::mapTileId open);
		
		int findLargestZone(std::vector<std::vector<df::Vector>> &zones);

		df::Vector findClosestPointInOtherZone(df::Vector targetPoint, std::vector<df::Vector> &otherPoints);

		CoordinatePair findSmallestConnectionLine(std::vector<df::Vector>& startZone, std::vector<std::vector<df::Vector>>& otherZones);

		std::vector<df::Vector> getRandomCoordListOfValue(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId open, MapGenConfig config);

		df::Vector getRandomCoordOfValue(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId open);

		int sprinkleTrees(std::vector<std::vector<mapTileIds::mapTileId>>& map,int trees, mapTileIds::mapTileId open, mapTileIds::mapTileId tree, MapGenConfig config);

		int sprinkleSeeds(std::vector<std::vector<mapTileIds::mapTileId>>& map,int seeds, mapTileIds::mapTileId open, mapTileIds::mapTileId seed, MapGenConfig config);

		int placeOwl(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId open, mapTileIds::mapTileId);

		int placeExit(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId open, mapTileIds::mapTileId exit);

		int squarePlot(std::vector<std::vector<mapTileIds::mapTileId>>& map, df::Vector location, mapTileIds::mapTileId value);

		df::ObjectList buildMap(MapGenConfig config,std::vector<std::vector<mapTileIds::mapTileId>>& map, df::Object* owl);

		df::Vector getXYDistanceBetweenTwoPoint(df::Vector p0, df::Vector p1);

		CoordinatePair findAngleLineClosestToDistance(std::vector<std::vector<mapTileIds::mapTileId>>& map,int dist, mapTileIds::mapTileId pointType, MapGenConfig config);
		CoordinatePair findDiagLineClosestToDistance(std::vector<std::vector<mapTileIds::mapTileId>>& map,int dist, mapTileIds::mapTileId pointType, MapGenConfig config);

		

		int ensureSpace(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId empty, mapTileIds::mapTileId tree, int neededOpen,MapGenConfig config);

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
