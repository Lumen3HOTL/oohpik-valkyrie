#pragma once
//the kitchen sink
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
#include <mutex>
#include <chrono>
#include "MapExit.h"
#include "Tree.h"
#include "Ground.h"
#include "Seed.h"
#include "MapGenDebugObj.h"
#include "MapBuildStateObject.h"
#include "EventManager.h"
#include "ookpikEnums.h"
namespace ookpik {
	// Fresh generations a map builder starts after errors before it gives up
	const int MAX_GENERATION_RETRIES = 5;
	
	class MapBuilder : public df::Object {
	private:
		MapGenConfig m_configObj;
		std::thread* m_genThread;
		unsigned long long m_genTime;
		unsigned long long m_buildTime;
		int m_timeout;
		df::Clock m_timer;
		df::Object* m_player;
		std::vector<std::vector<mapTileIds::mapTileId>> m_map_plan;
		
		std::mutex m_state_gate=std::mutex();
		std::mutex m_mode_gate = std::mutex();
		std::mt19937 m_RandomEngine;
		MapGenDebugObj* m_debug_harness;
		MapBuildStateObject m_builder_state;

		bool m_demo_mode;
		bool m_delete_mode;
		GenerationStages::GenerationStage m_current_mode;
		bool m_base_function_exit;
		bool m_error_handled;
		MapGenConfig m_requested_config; // the config as passed in, before generateMap() picks a random seed
		int m_retries; // fresh generations started after an error


		int configureMapBuilding(MapGenConfig config, std::vector<std::vector<mapTileIds::mapTileId>> mapPlan, df::Object* owl);

		int buildMapV2(MapBuildStateObject& state);

		GenerationStages::GenerationStage getCurrentMode();
		void setCurrentMode(GenerationStages::GenerationStage new_mode);
		bool getBaseFunctionExit();
		void setBaseFunctionExit(bool new_base_function_exit);

		
		int getRandom(int low, int high);
		void setMapReturn(df::ObjectList new_map_return);
		df::ObjectList getMapReturn();

		bool getDeleteMode();
		void setDeleteMode(bool new_delete_mode);
		
		
		unsigned long long getBuildTime();
		void setBuildTime(unsigned long long n_build_time);

		void setGenTime(unsigned long long new_gen_time);
		unsigned long long getGenTime();

		void resetTimer();
		bool checkTimer();
		unsigned long long getTimerTime();
		int getTimeout();
		int setTimeout(int new_timeout);


		

		float findDistance(df::Vector p0, df::Vector p1);

		int eliminateDisperateZones(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId flood, mapTileIds::mapTileId empty, MapGenConfig configObj);

		std::vector<df::Vector> generateBresenhamLine(int x1, int y1, int x2, int y2);

		std::vector<df::Vector> generateXYLine(int x1, int y1, int x2, int y2, bool yFirst, bool point1First);

		std::vector<std::vector<mapTileIds::mapTileId>> createStartingMap(int width, int height, mapTileIds::mapTileId canvas);

		std::vector<std::vector<mapTileIds::mapTileId>> copyMap(std::vector<std::vector<mapTileIds::mapTileId>> map);

		int drawLine(std::vector<std::vector<mapTileIds::mapTileId>> &map, std::vector<df::Vector> &line, mapTileIds::mapTileId type);

		int drawRectangle(std::vector<std::vector<mapTileIds::mapTileId>>& map, df::Box square, mapTileIds::mapTileId value);

		int floodFill(std::vector<std::vector<mapTileIds::mapTileId>> &map, df::Vector location, mapTileIds::mapTileId fillValue, mapTileIds::mapTileId emptyValue);

		std::vector<df::Vector> floodFillReturnCoords(std::vector<std::vector<mapTileIds::mapTileId>>& map, df::Vector location, mapTileIds::mapTileId fillValue, mapTileIds::mapTileId emptyValue);

		std::vector<df::Vector> findCoordsOfValue(std::vector<std::vector<mapTileIds::mapTileId>> &map, mapTileIds::mapTileId open);

		int findCoordsOfValueCount(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId open);

		std::vector<std::vector<df::Vector>> findZones(std::vector<std::vector<mapTileIds::mapTileId>> &map, mapTileIds::mapTileId flood, mapTileIds::mapTileId open);
		
		int findLargestZone(std::vector<std::vector<df::Vector>> &zones);

		df::Vector findClosestPointInOtherZone(df::Vector targetPoint, std::vector<df::Vector> &otherPoints);

		CoordinatePair findSmallestConnectionLine(std::vector<df::Vector>& startZone, std::vector<std::vector<df::Vector>>& otherZones);

		std::vector<df::Vector> getRandomCoordListOfValue(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId open);

		df::Vector getRandomCoordOfValue(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId open);

		int sprinkleTrees(std::vector<std::vector<mapTileIds::mapTileId>>& map,int trees, mapTileIds::mapTileId open, mapTileIds::mapTileId tree);

		int sprinkleSeeds(std::vector<std::vector<mapTileIds::mapTileId>>& map,int seeds, mapTileIds::mapTileId open, mapTileIds::mapTileId seed);

		int placeOwl(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId open, mapTileIds::mapTileId);

		int placeExit(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId open, mapTileIds::mapTileId exit);

		int squarePlot(std::vector<std::vector<mapTileIds::mapTileId>>& map, df::Vector location, mapTileIds::mapTileId value);

		//deprecated function
		//int buildMap(MapGenConfig config,std::vector<std::vector<mapTileIds::mapTileId>>& map, df::Object* owl);

		df::Vector getXYDistanceBetweenTwoPoint(df::Vector p0, df::Vector p1);

		CoordinatePair findAngleLineClosestToDistance(std::vector<std::vector<mapTileIds::mapTileId>>& map,int dist, mapTileIds::mapTileId pointType);
		CoordinatePair findDiagLineClosestToDistance(std::vector<std::vector<mapTileIds::mapTileId>>& map,int dist, mapTileIds::mapTileId pointType);

		

		int ensureSpace(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId empty, mapTileIds::mapTileId tree, int neededOpen);

		int calculateNeededOpenSpaces(int seeds);

		void generateMap();

		// After a generation or build error: log the errors and start a fresh map
		void retryGeneration();
		

		

		

	public:

		
		int startGenerateMap(MapGenConfig config, df::Object* owl);

		bool isMapGenFinished();
		bool isMapBuildFinished();

		int destroyMap(df::ObjectList map);

		int eventHandler(const df::Event* m_p) override;

		int draw() override;
	
		MapBuilder();
		~MapBuilder();


	};
}
