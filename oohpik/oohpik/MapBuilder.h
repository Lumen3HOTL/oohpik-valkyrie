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
#include <mutex>
#include <chrono>
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
		
		df::ObjectList m_mapReturn;
		bool m_generating;
		bool m_genDone;
		bool m_done_sent;
		bool m_build_done;
		bool m_building;
		std::thread* m_genThread;
		unsigned long long m_genTime;
		unsigned long long m_buildTime;
		int m_timeout;
		df::Clock m_timer;
		df::Object* m_player;
		std::vector<std::vector<mapTileIds::mapTileId>> m_map_plan;
		bool m_gen_error;
		std::vector<std::string> m_error_messages;
		std::mutex m_error_gate=std::mutex();
		std::mutex m_state_gate=std::mutex();
		std::mutex m_log_man_access = std::mutex();
		
		int m_build_progress;
		int m_to_build;
		int m_build_per_frame;
		df::Vector m_current_build_pos;
		bool m_base_function_exit;
		int m_lastx;
		int m_lasty;
		std::mt19937 m_RandomEngine;
		std::string m_debug_map1;
		std::string m_debug_map2;
		std::vector<std::string> m_current_debug_strip;
		std::vector<std::vector<std::string>> m_debug_strips;
		std::vector<df::Vector > m_debug_Coords;
		std::vector<df::Vector> m_debug_map_positions;
		bool m_debug = true;


		int getRandom(int low, int high);
		void setMapReturn(df::ObjectList new_map_return);
		df::ObjectList getMapReturn();

		bool isMapBuildFinished();
		
		void setLastX(int new_last_x);
		int getLastX();
		void setLastY(int new_last_y);
		int getlastY();

		int getBuildProgress();
		void setBuildProgress(int new_build_progress);
		int getToBuild();
		void setToBuild(int new_to_build);
		int getBuildPerFrame();
		void setBuildPerFrame(int new_build_per_frame);
		void setCurrentBuildPos(df::Vector  new_current_build_pos);
		df::Vector getCurrentBuildPos();

		unsigned long long getBuildTime();
		void setBuildTime(unsigned long long n_build_time);
		

		void setBuildDone(bool new_build_done);
		bool getBuildDone();
		void setGenDone(bool new_gen_done);
		bool getGenDone();
		void setBuilding(bool new_building);
		bool getBuilding();

		void setGenTime(unsigned long long new_gen_time);
		unsigned long long getGenTime();

		void resetTimer();
		bool checkTimer();
		unsigned long long getTimerTime();
		int getTimeout();
		int setTimeout(int new_timeout);

		void setGenerating(bool new_genrating);
		bool getGenerating();
		void setDoneSent(bool new_done_set);
		bool getDoneSet();

		void setBaseFunctionExit(bool new_base_function_exit);
		bool getBaseFunctionExit();

		void addErrorMessage(std::string new_error_message);
		void resetErrorMessage();
		void setErrorMessages(std::vector<std::string> new_error_messages);
		std::vector<std::string> getErrorMessages();

		void setGenError(bool new_gen_error);
		bool getGenError();

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

		int buildMap(MapGenConfig config,std::vector<std::vector<mapTileIds::mapTileId>>& map, df::Object* owl);

		df::Vector getXYDistanceBetweenTwoPoint(df::Vector p0, df::Vector p1);

		CoordinatePair findAngleLineClosestToDistance(std::vector<std::vector<mapTileIds::mapTileId>>& map,int dist, mapTileIds::mapTileId pointType);
		CoordinatePair findDiagLineClosestToDistance(std::vector<std::vector<mapTileIds::mapTileId>>& map,int dist, mapTileIds::mapTileId pointType);

		

		int ensureSpace(std::vector<std::vector<mapTileIds::mapTileId>>& map, mapTileIds::mapTileId empty, mapTileIds::mapTileId tree, int neededOpen);

		int calculateNeededOpenSpaces(int seeds);

		void generateMap();
		

		

		

	public:

		
		int startGenerateMap(MapGenConfig config, df::Object* owl);

		bool isMapGenFinished();

		int destroyMap(df::ObjectList map);

		int eventHandler(const df::Event* m_p) override;

		int draw() override;
	
		MapBuilder();
		~MapBuilder();


	};
}
