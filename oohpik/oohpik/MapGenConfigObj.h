#pragma once
#include "vector.h"
#include <string>
namespace ookpik {
	class MapGenConfig {
	private:
		int m_map_width;
		int m_map_height;
		df::Vector m_map_origin;

		int m_timeout_seconds;

		int m_objects_construct_per_frame;

		int m_map_object_altitude;

		bool m_map_gen_debug;
		bool m_map_gen_demo_mode;
		std::string m_map_gen_demo_mode_log_file_name;

		int m_min_rand_trees;
		int m_max_rand_trees;

		int m_min_rand_diag_lines;
		int m_max_rand_diag_lines;

		int m_min_rand_angle_lines;
		int m_max_rand_angle_lines;

		int m_max_rand_angle_line_width;
		int m_max_rand_angle_line_height;

		int m_min_rand_angle_line_width;
		int m_min_rand_angle_line_height;

		int m_max_rand_daig_line_width;
		int m_max_rand_diag_line_height;


		int m_min_rand_daig_line_width;
		int m_min_rand_diag_line_height;

		int m_random_seed;

		int m_min_rooms;
		int m_max_rooms;

		int m_min_room_width;
		int m_max_room_width;

		int m_min_room_height;
		int m_max_room_height;

		int m_map_border_thickness;

		int m_min_seeds;
		int m_max_seeds;

		int m_map_object_width;
		int m_map_object_height;



	public:
		
		MapGenConfig();

		void setObjectsConstructedPerFrame(int new_objects_constructed_per_frame);
		int getObjectsConstructedPerFrame()const;
	
		void setGenDebugMode(bool new_debug_mode);
		bool getGenDebugMode()const;

		void setGenDemoMode(bool new_demo_mode);
		bool getaGenDemoMode()const;

		void setGenDemoModeLogFileName(std::string new_demo_mode_log_file_name);
		std::string getGenDemoModeLogFileName()const;

		void setRandomSeed(int new_seed);
		int getRandomSeed()const;

		void setTimeoutSeconds(int new_timeout);
		int getTimeoutSeconds()const;

		df::Vector getMapOrigin()const;

		void setMapOrigin(df::Vector new_origin);

		int getMapWidth()const;
		int getMapHeight()const;

		void setMapWidth(int new_width);
		void setMapHeight(int new_height);

		int getMaxRandTrees()const;
		int getMinRandTrees()const;

		void setMaxRandTrees(int new_max_trees);
		void setMinRandTrees(int new_min_trees);

		int getMaxDiagLines()const;
		int getMinDiagLines()const;

		void setMaxDiagLine(int new_max_daigs);
		void setMinDiagLines(int new_min_diags);

		int getMaxRightAngleLine()const;
		int getMinRightAngleLines()const;

		void setMaxRightAngleLine(int new_max_right_angles);
		void setMinRightAngleLines(int new_min_right_angles);


		int getMinRooms()const;
		int getMaxRooms()const;

		void setMinRooms(int new_min_rooms);
		void setMaxRooms(int new_max_rooms);

		int getMinRoomWidth()const;
		int getMaxRoomWidth()const;

		void setMinRoomWidth(int new_min_rooms_width);
		void setMaxRoomWidth(int new_max_rooms_width);

		int getMinRoomHeight()const;
		int getMaxRoomHeight()const;

		void setMinRoomHeight(int new_min_rooms_height);
		void setMaxRoomHeight(int new_max_rooms_height);

		int getMapBorderThickness()const;

		void setMapBorderThickness(int new_map_border_thickness);

		void setMinSeeds(int new_seed_min_count);
		void setMaxSeeds(int new_seed_max_count);

		int getMinSeeds()const;
		int getMaxSeeds()const;

		int getMaxAngleLineWidth()const;
		int getMaxAngleLineHeight()const;

		int getMinAngleLineWidth()const;
		int getMinAngleLineHeight()const;

		void setMaxRightAngleLineWidth(int new_max_right_angle_line_width);
		void setMaxRightAngleLineHeight(int new_max_right_angle_line_height);

		void setMinRightAngleLineWidth(int new_min_right_angle_line_width);
		void setMinRightAngleLineHeight(int new_min_right_angle_line_height);


		int getMaxDiagLineWidth()const;
		int getMaxDiagLineHeight()const;

		int getMinDiagLineWidth()const;
		int getMinDiagLineHeight()const;

		void setMaxDiagLineWidth(int new_max_diag_line_width);
		void setMaxDiagLineHeight(int new_max_diag_line_height);

		void setMinDiagLineWidth(int new_min_diag_line_width);
		void setMinDiagLineHeight(int new_min_diag_line_height);

		int getMapObjectAltitude()const;
		void setMapObjectAltitude(int new_map_object_altitude);

		int getMapObjectWidth()const;
		int getMapObjectHeight()const;

		void setMapObjectWidth(int new_object_width);
		void setMapObjectHeight(int new_object_height);


	};
}