#include "Level.h"
#include "WorldManager.h"
#include "MapBuilder.h"
#include "MapGenConfigObj.h"

namespace {
	// Map layout: fills the 115x30 window like the original ookpik.
	// buildMap draws rows from y = 1, so 27 rows + a 1-tile border on each side = rows 1..29.
	ookpik::MapGenConfig makeMapConfig() {
		ookpik::MapGenConfig config;
		config.setMapOrigin(df::Vector(0, 0));


		config.setMapWidth(56);
		config.setMapHeight(27);
		config.setMapBorderThickness(1);
		config.setObjectsConstructedPerFrame(1000); // build the whole map in one frame

		
		//config.setMapWidth(500);
		//config.setMapHeight(500);
		//config.setMapBorderThickness(50);
		//config.setObjectsConstructedPerFrame(100); 
		
		
		
		config.setMapObjectWidth(2);
		config.setMapObjectHeight(1);
		config.setMapObjectAltitude(0);
		
		config.setTimeoutSeconds(60);
		config.setRandomSeed(0); // 0 = pick a new random seed each run

		// Open structures carved out of the forest
		config.setMinRooms(6);
		config.setMaxRooms(10);
		config.setMinRoomWidth(4);
		config.setMaxRoomWidth(12);
		config.setMinRoomHeight(3);
		config.setMaxRoomHeight(6);
		config.setMinRightAngleLines(20);
		config.setMaxRightAngleLine(30);
		config.setMinRightAngleLineWidth(10);
		config.setMaxRightAngleLineWidth(40);
		config.setMinRightAngleLineHeight(3);
		config.setMaxRightAngleLineHeight(12);
		config.setMinDiagLines(20);
		config.setMaxDiagLine(30);
		config.setMinDiagLineWidth(10);
		config.setMaxDiagLineWidth(30);
		config.setMinDiagLineHeight(3);
		config.setMaxDiagLineHeight(10);

		// Contents, based on the original game's small-map settings
		config.setMinRandTrees(50);
		config.setMaxRandTrees(150);
		config.setMinSeeds(15);
		config.setMaxSeeds(30);
		return config;
	}

	// Mark every object of a type for deletion (removed at the end of this frame)
	void removeObjectsOfType(const std::string& type) {
		df::ObjectList objects = WM.objectsOfType(type);
		for (int i = 0; i < objects.getCount(); i++) {
			WM.markForDelete(objects[i]);
		}
	}
}

void startNewMap(df::Object* p_owl) {
	// Clear the previous map by type. MapBuilder::destroyMap isn't used because its
	// object list also contains the owl, which must survive between maps.
	removeObjectsOfType("Tree");
	removeObjectsOfType("Ground");
	removeObjectsOfType("Seed");
	removeObjectsOfType("mapExit");
	removeObjectsOfType("mapBuilder");

	ookpik::MapBuilder* p_map_builder = new ookpik::MapBuilder();
	p_map_builder->startGenerateMap(makeMapConfig(), p_owl);
}
