#pragma once
#include "Manager.h"
#include "ObjectList.h"
#include "Vector.h"
#include "Box.h"
namespace df {
	const unsigned int MAX_ALTITUDE = 4;
	const int WORLD_HORIZONTAL_CHARS_DEFAULT = 500;
	const int WORLD_VERTICAL_CHARS_DEFAULT = 500;
	class WorldManager : public Manager {
		private:
			WorldManager(); // Private (a singleton).
			WorldManager(WorldManager const&); // Don't allow copy.
			void operator =(WorldManager const&); // Don't allow assignment.
			
			ObjectList m_updates; // All Objects in world to update.
			ObjectList m_deletions; // All Objects in world to delete.
			
			Box m_world_box;
		 public:

			 int setWorldBox(Box new_world_space);

			 Box getWorldSpaceBox()const;
			 
			 int massFrustromCheck();
			 bool frustromCheck(Object* p_o);
			 

		// Get the one and only instance of the WorldManager.
			static WorldManager &getInstance();
		
			// Startup game world (initialize everything to empty).
			// Return 0 .
			int startUp();
	
			// Shutdown game world (delete all game world Objects).
			void shutDown();
		
			// Insert Object into world. Return 0 if ok, else -1.
			int insertObject(Object * p_o);
		
			// Remove Object from world. Return 0 if ok, else -1.
			int removeObject(Object * p_o);
		
			// Return list of all Objects in world.
			ObjectList getAllObjects() const;
		
			// Return list of all Objects in world matching type.
			ObjectList objectsOfType(std::string type);
		
			// Update world.
			// Delete Objects marked for deletion.
			void update();

			int AllObjectsCount() const;

			int objectsOfTypeCount(std::string type) const;
		
			// Indicate Object is to be deleted at end of current game loop.
			// Return 0 if ok, else -1.
			int markForDelete(Object * p_o);
		
			void draw();

			// Return list of Objects collided with at position 'where'.
			// Collisions only with solid Objects.
			// Does not consider if po is solid or not.
			ObjectList getCollisions(const Object * p_o, Vector where);
			
			// Move Object.
			// If collision with solid, send collision events.
			// If no collision with solid, move ok else don't move Object.
			// If Object is Spectral, move ok.
			// Return 0 if move ok, else -1 if collision with solid.
			int moveObject(Object * p_o, Vector where);
	};

}

#define WM df::WorldManager::getInstance()