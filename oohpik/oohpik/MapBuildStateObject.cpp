#include "MapBuildStateObject.h"
#include <iostream>
namespace ookpik {
	MapBuildStateObject::MapBuildStateObject() {
		m_current_X=0;
		m_current_Y=0;
		m_map_width=0;
		m_map_height=0;
		m_object_spawn_per_frame=0;
		m_border_thickness=0;
		m_full_world_width=0;
		m_full_world_height=0;
		m_tile_width=0;
		m_tile_height=0;
		m_finished=false;
		m_map_origin=df::Vector();
		m_current_global_pos=df::Vector();
		m_map_objects=df::ObjectList();
		m_player=nullptr;
		m_altitude = 0;
		m_map_plan= std::vector<std::vector<mapTileIds::mapTileId>>();
	}
	void MapBuildStateObject::reset() {
		m_current_X = 0;
		m_current_Y = 0;
		m_map_width = 0;
		m_map_height = 0;
		m_object_spawn_per_frame = 0;
		m_border_thickness = 0;
		m_full_world_width = 0;
		m_full_world_height = 0;
		m_tile_width = 0;
		m_tile_height = 0;
		m_finished = false;
		m_map_origin = df::Vector();
		m_current_global_pos = df::Vector();
		m_map_objects = df::ObjectList();
		m_player = nullptr;
		m_altitude = 0;
		m_map_plan = std::vector<std::vector<mapTileIds::mapTileId>>();
	}

	int MapBuildStateObject::getAltitude()const {
		return m_altitude;
	}
	void MapBuildStateObject::setAltitude(int new_altitude) {
		m_altitude = new_altitude;
	}

	void MapBuildStateObject::setMapPlan(std::vector<std::vector<mapTileIds::mapTileId>> new_map_plan) {
		m_map_plan = new_map_plan;
	}
	std::vector<std::vector<mapTileIds::mapTileId>> MapBuildStateObject::getMapPlan()const {
		return m_map_plan;
	}


	void MapBuildStateObject::deleteAllMapObjects() {
		df::WorldManager& wm = df::WorldManager::getInstance();
		for (int i = 0; i < m_map_objects.getCount(); i++) {
			wm.markForDelete(m_map_objects[i]);
		}
	}
	int MapBuildStateObject::getCurrentX()const {
		return m_current_X;
	}
	int MapBuildStateObject::getCurrentY()const {
		return m_current_Y;
	}
	int MapBuildStateObject::getMapWidth()const {
		return m_map_width;
	}
	int MapBuildStateObject::getMapheight()const {
		return m_map_height;
	}
	int MapBuildStateObject::getObjectSpawnPerFrame()const {
		return m_object_spawn_per_frame;
	}
	int MapBuildStateObject::getBorderThickness()const {
		return m_border_thickness;
	}
	int MapBuildStateObject::getFullWorldWidth()const {
		return m_full_world_width;
	}
	int MapBuildStateObject::getFullWorldHeight()const {
		return m_full_world_height;
	}
	int MapBuildStateObject::getTileWidth()const {
		return m_tile_width;
	}
	int MapBuildStateObject::getTileHeigth()const {
		return m_tile_height;
	}
	bool MapBuildStateObject::getFinished()const {
		return m_finished;
	}
	df::Vector MapBuildStateObject::getMapOrigin()const {
		return m_map_origin;
	}
	df::Vector MapBuildStateObject::getCurrentGlobalPos()const {
		return m_current_global_pos;
	}
	df::ObjectList MapBuildStateObject::getCurrentMapObjects()const {
		return m_map_objects;
	}
	df::Object* MapBuildStateObject::getPlayer()const {
		return m_player;
	}

	void MapBuildStateObject::setCurrentX(int new_x) {
		m_current_X = new_x;
	}
	void MapBuildStateObject::setCurrentY(int new_y) {
		m_current_Y = new_y;
	}
	void MapBuildStateObject::setMapWidth(int new_width) {
		m_map_width = new_width;
		m_full_world_width = m_map_width + (m_border_thickness * 2);
	}
	void MapBuildStateObject::setMapHeight(int new_height) {
		m_map_height = new_height;
		m_full_world_height = m_map_height + (m_border_thickness * 2);
	}
	void MapBuildStateObject::setObjectSpawnPerFrame(int new_object_frame_limit) {
		m_object_spawn_per_frame = new_object_frame_limit;
	}
	void MapBuildStateObject::setBorderThickness(int new_border_thickness) {
		m_border_thickness = new_border_thickness;
		m_full_world_width = m_map_width + (m_border_thickness * 2);
		m_full_world_height = m_map_height + (m_border_thickness * 2);
	}
	void MapBuildStateObject::setTileWidth(int new_tile_width) {
		m_tile_width = new_tile_width;
	}
	void MapBuildStateObject::setTileHeight(int new_tile_height) {
		m_tile_height = new_tile_height;
	}
	void MapBuildStateObject::setFinished(bool new_finished) {
		m_finished = new_finished;
	}
	void MapBuildStateObject::setMapOrigin(df::Vector new_map_origin) {
		m_map_origin = new_map_origin;
	}
	void MapBuildStateObject::setCurrentGlobalPos(df::Vector new_global_pos) {
		m_current_global_pos = new_global_pos;
	}
	void MapBuildStateObject::setMapObjects(df::ObjectList new_map_objects) {
		m_map_objects = new_map_objects;
	}
	void MapBuildStateObject::setPlayer(df::Object* new_player) {
		m_player = new_player;
	}

	void MapBuildStateObject::addMapObject(df::Object* new_object) {
		m_map_objects.insert(new_object);
	}
	int MapBuildStateObject::advanceCursor() {
		if (m_finished) {
			return -1;
		}
		m_current_X++;
		m_current_global_pos.setX(m_current_global_pos.getX() + m_tile_width);
		if (m_current_X >= m_full_world_width) {
			m_current_X = 0;
			m_current_Y++;
			m_current_global_pos.setXY(0, m_current_global_pos.getY() + m_tile_height);
			if (m_current_Y >= m_full_world_height) {
				m_finished = true;
			}
		}
		return 0;
	}



	bool MapBuildStateObject::isCurrentPosBorder() {
		
		return ((m_current_X < m_border_thickness) ||(m_current_X >= (m_border_thickness + (m_map_width))) ||(m_current_Y < m_border_thickness) ||(m_current_Y >= (m_border_thickness +( m_map_height))));
	}
	df::Vector  MapBuildStateObject::getTrueCursorPos() {
		return m_map_origin + m_current_global_pos;
	}


	mapTileIds::mapTileId MapBuildStateObject::getValueAtCurrentPosition() {
		if (m_finished) {
			return mapTileIds::TILE_ERROR;
		}
		if (this->isCurrentPosBorder()) {
			return mapTileIds::TREE;
		}
		
		return  m_map_plan[m_current_X-m_border_thickness][m_current_Y-m_border_thickness];

	}
}