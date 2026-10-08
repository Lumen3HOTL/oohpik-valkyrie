#pragma once
#include "Vector.h"
#include "ObjectList.h"
#include "WorldManager.h"
#include <vector>
#include "MapBuilder.h"
namespace ookpik {
	class MapBuildStateObject {
	private:
		int m_current_X;
		int m_current_Y;
		int m_map_width;
		int m_map_height;
		int m_object_spawn_per_frame;
		int m_border_thickness;
		int m_full_world_width;
		int m_full_world_height;
		int m_tile_width;
		int m_tile_height;
		int m_altitude;
		bool m_finished;
		df::Vector m_map_origin;
		df::Vector m_current_global_pos;
		df::ObjectList m_map_objects;
		df::Object* m_player;
		std::vector<std::vector<mapTileIds::mapTileId>> m_map_plan;
	public:

		void setMapPlan(std::vector<std::vector<mapTileIds::mapTileId>> new_map_plan);
		std::vector<std::vector<mapTileIds::mapTileId>> getMapPlan()const;

		MapBuildStateObject();
		void reset();
		void deleteAllMapObjects();
		int getCurrentX()const;
		int getCurrentY()const;
		int getMapWidth()const;
		int getMapheight()const;
		int getObjectSpawnPerFrame()const;
		int getBorderThickness()const;
		int getFullWorldWidth()const;
		int getFullWorldHeight()const;
		int getTileWidth()const;
		int getTileHeigth()const;
		bool getFinished()const;
		df::Vector getMapOrigin()const;
		df::Vector getCurrentGlobalPos()const;
		df::ObjectList getCurrentMapObjects()const;
		df::Object* getPlayer()const;
		int getAltitude()const;
		void setAltitude(int new_altitude);

		void setCurrentX(int new_x);
		void setCurrentY(int new_y);
		void setMapWidth(int new_width);
		void setMapHeight(int new_height);
		void setObjectSpawnPerFrame(int new_object_frame_limit);
		void setBorderThickness(int new_border_thickness);
		void setTileWidth(int new_tile_width);
		void setTileHeight(int new_tile_height);
		void setFinished(bool new_finished);
		void setMapOrigin(df::Vector new_map_origin);
		void setCurrentGlobalPos(df::Vector new_global_pos);
		void setMapObjects(df::ObjectList new_map_objects);
		void setPlayer(df::Object* new_player);

		void addMapObject(df::Object* new_object);
		int advanceCursor();
		bool isCurrentPosBorder();
		mapTileIds::mapTileId getValueAtCurrentPosition();
		df::Vector getTrueCursorPos();
	};
}