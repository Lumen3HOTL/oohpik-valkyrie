#include "MapGenConfigObj.h"

namespace ookpik {
	MapGenConfig::MapGenConfig() {
		m_map_width=0;
		m_map_height=0;
		m_map_origin=df::Vector();


		m_min_rand_trees=0;
		m_max_rand_trees=0;

		m_min_rand_diag_lines=0;
		m_max_rand_diag_lines=0;

		m_min_rand_angle_lines=0;
		m_max_rand_angle_lines=0;

		m_max_rand_angle_line_width=0;
		m_max_rand_angle_line_height=0;

		m_max_rand_daig_line_width=0;
		m_max_rand_diag_line_height=0;




		m_min_rooms=0;
		m_max_rooms=0;

		m_min_room_width=0;
		m_max_room_width=0;

		m_min_room_height=0;
		m_max_room_height=0;

		m_map_border_thickness=0;

		m_min_seeds=0;
		m_max_seeds=0;
	}

	df::Vector MapGenConfig::getMapOrigin()const {
		return m_map_origin;
	}

	void MapGenConfig::setMapOrigin(df::Vector new_origin) {
		m_map_origin = new_origin;
	}

	int MapGenConfig::getMapWidth()const {
		return m_map_width;
	}
	int MapGenConfig::getMapHeight()const {
		return m_map_height;
	}

	void MapGenConfig::setMapWidth(int new_width) {
		m_map_width = new_width;
	}
	void MapGenConfig::setMapHeight(int new_height) {
		m_map_height = new_height;
	}

	int MapGenConfig::getMaxRandTrees()const {
		return m_max_rand_trees;
	}
	int MapGenConfig::getMinRandTrees()const {
		return m_min_rand_trees;
	}

	void MapGenConfig::setMaxRandTrees(int new_max_trees) {
		m_max_rand_trees = new_max_trees;
	}
	void MapGenConfig::setMinRandTrees(int new_min_trees) {
		m_min_rand_trees = new_min_trees;
	}

	int MapGenConfig::getMaxDiagLines()const {
		return m_max_rand_diag_lines;
	}
	int MapGenConfig::getMinDiagLines()const {
		return m_min_rand_diag_lines;
	}

	void MapGenConfig::setMaxDiagLine(int new_max_daigs) {
		m_max_rand_diag_lines = new_max_daigs;
	}
	void MapGenConfig::setMinDiagLines(int new_min_diags) {
		m_min_rand_diag_lines = new_min_diags;
	}

	int MapGenConfig::getMaxAngleLine()const {
		return m_max_rand_angle_lines;
	}
	int MapGenConfig::getMinAngleines()const {
		return m_min_rand_angle_lines;
	}

	void MapGenConfig::setMaxAngleLine(int new_max_angles) {
		m_max_rand_angle_lines = new_max_angles;
	}
	void MapGenConfig::setMinAngleLines(int new_min_Angles) {
		m_min_rand_angle_lines = new_min_Angles;
	}


	int MapGenConfig::getMinRooms()const {
		return m_min_rooms;
	}
	int MapGenConfig::getMaxRooms()const {
		return m_max_rooms;
	}

	void MapGenConfig::setMinRooms(int new_min_rooms) {
		m_min_rooms = new_min_rooms;
	}
	void MapGenConfig::setMaxRooms(int new_max_rooms) {
		m_max_rooms = new_max_rooms;
	}

	int MapGenConfig::getMinRoomWidth()const {
		return m_min_room_width;
	}
	int MapGenConfig::getMaxRoomWidth()const {
		return m_max_room_width;
	}

	void MapGenConfig::setMinRoomWidth(int new_min_room_width) {
		m_min_room_width = new_min_room_width;
	}
	void MapGenConfig::setMaxRoomWidth(int new_max_room_width) {
		m_max_room_width = new_max_room_width;
	}

	int MapGenConfig::getMinRoomHeight()const {
		return m_min_room_height;
	}
	int MapGenConfig::getMaxRoomHeight()const {
		return m_max_room_height;
	}

	void MapGenConfig::setMinRoomHeight(int new_min_room_height) {
		m_min_room_height = new_min_room_height;
	}
	void MapGenConfig::setMaxRoomHeight(int new_max_room_height) {
		m_max_room_height = new_max_room_height;
	}

	int MapGenConfig::getMapBorderThickness()const {
		return m_map_border_thickness;
	}

	void MapGenConfig::setMapBorderThickness(int new_map_border_thickness) {
		m_map_border_thickness = new_map_border_thickness;
	}

	void MapGenConfig::setMinSeeds(int new_seed_min_count) {
		m_min_seeds = new_seed_min_count;
	}
	void MapGenConfig::setMaxSeeds(int new_seed_max_count) {
		m_max_seeds = new_seed_max_count;
	}

	int MapGenConfig::getMinSeeds()const {
		return m_min_seeds;
	}
	int MapGenConfig::getMaxSeeds()const {
		return  m_max_seeds;
	}


	int MapGenConfig::getMaxAngleLineWidth()const {
		return m_max_rand_angle_line_width;
	}
	int MapGenConfig::getMaxAngleLineHeight()const {
		return m_max_rand_angle_line_height;
	}

	int MapGenConfig::getMinAngleLineWidth()const {
		return m_min_rand_angle_line_width;
	}
	int MapGenConfig::getMinAngleLineHeight()const {
		return m_min_rand_angle_line_height;
	}

	int MapGenConfig::setMaxAngleLineWidth(int new_max_angle_line_width) {
		m_max_rand_angle_line_width = new_max_angle_line_width;
	}
	int MapGenConfig::setMaxAngleLineHeight(int new_max_angle_line_height) {
		m_max_rand_angle_line_height = new_max_angle_line_height;
	}

	int MapGenConfig::setMinAngleLineWidth(int new_min_angle_line_width) {
		m_min_rand_angle_line_width = new_min_angle_line_width;
	}
	int MapGenConfig::setMinAngleLineHeight(int new_min_angle_line_height) {
		m_min_rand_angle_line_height = new_min_angle_line_height;
	}


	int MapGenConfig::getMaxDiagLineWidth()const {
		return m_max_rand_daig_line_width;
	}
	int MapGenConfig::getMaxDiagLineHeight()const {
		return m_max_rand_daig_line_width;
	}

	int MapGenConfig::getMinDiagLineWidth()const {
		return m_min_rand_daig_line_width;
	}
	int MapGenConfig::getMinDiagLineHeight()const {
		return m_min_rand_diag_line_height;
	}

	int MapGenConfig::setMaxDiagLineWidth(int new_max_diag_line_width) {
		m_max_rand_daig_line_width = new_max_diag_line_width;
	}
	int MapGenConfig::setMaxDiagLineHeight(int new_max_diag_line_height) {
		m_max_rand_diag_line_height = new_max_diag_line_height;
	}

	int MapGenConfig::setMinDiagLineWidth(int new_min_diag_line_width) {
		m_min_rand_daig_line_width = new_min_diag_line_width;
	}
	int MapGenConfig::setMinDiagLineHeight(int new_min_diag_line_height) {
		m_min_rand_diag_line_height = new_min_diag_line_height;
	}
}