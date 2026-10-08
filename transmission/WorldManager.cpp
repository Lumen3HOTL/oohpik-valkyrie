#include "WorldManager.h"
#include <vector>
#include <iostream>
#include "utility.h"
#include "EventCollision.h"
#include <math.h>
#include "DisplayManager.h"
#include "EventOut.h"
#include "CameraManager.h"
#include "EventCameraOut.h"
namespace df {
	//create the world manager init our vars, and set our type
	WorldManager::WorldManager() {
		this->setType("WorldManager");
		m_updates = ObjectList();
		m_deletions = ObjectList();
		m_world_box = Box(Vector(), WORLD_HORIZONTAL_CHARS_DEFAULT, WORLD_VERTICAL_CHARS_DEFAULT);

		
	}
	bool WorldManager::frustromCheck(Object* p_o) {
		if (this->isStarted()) {
			DisplayManager& window = DisplayManager::getInstance();
			CameraManager& cameras = CameraManager::getInstance();
			Box windowBox = Box(Vector(), window.getHorizontal(), window.getVertical());
			Box checkBox;
			EventCameraOut Out;
			
			checkBox = p_o->getBox();
			if (!cameras.isEmpty()) {
				if (p_o->getCameraAffected()) {
					 checkBox.setCorner( checkBox.getCorner() - cameras.getCurrentCamera()->getCameraPos());
				}
			}
			

			if (boxIntersectsBox(checkBox, windowBox)) {
				return true;
			}
			
		}

		return false;
	}


	int WorldManager::massFrustromCheck() {
		if (this->isStarted()) {
			DisplayManager& window = DisplayManager::getInstance();
			CameraManager& cameras = CameraManager::getInstance();
			Box windowBox = Box(Vector(), window.getHorizontal(), window.getVertical());
			Box checkBox;
			EventCameraOut Out;
			for (int check = 0; check < m_updates.getCount(); check++) {
				if (!this->frustromCheck(m_updates[check])) {
					m_updates[check]->eventHandler(&Out);
				}
			}

			return 0;
		}
		return -1;
	}
	
	int WorldManager::setWorldBox(Box new_world_space) {
		if (this->isStarted()) {
			if ((new_world_space.getHorizontal() < 1)||(new_world_space.getVertical())) {
				return -1;
			}
			m_world_box = new_world_space;
			return 0;
		}
		return -1;
	}

	Box WorldManager::getWorldSpaceBox()const {
		if (this->isStarted()) {
			return m_world_box;
		}
		return Box();
	}


	//singleton logic
	WorldManager& WorldManager::getInstance() {
		static WorldManager worldWrangler = WorldManager();
		return worldWrangler;
	}

	//if we havent already started, start then init vars
	int WorldManager::startUp() {
		if (Manager::startUp() < 0) {
			return -1;
		}
		
		m_updates.clear();
		m_deletions.clear();
		return 0;
	}
	
	void WorldManager::shutDown() {
		//go through and tell every object to kill itself

		for (int i = 0; i < m_updates.getCount(); i++) {

			delete m_updates[0];
		}
		//clear our lists
		m_updates.clear();
		m_deletions.clear();
		//shutourself down
		Manager::shutDown();
	}

	int WorldManager::insertObject(Object* p_o) {
		if (this->isStarted()) {
			return m_updates.insert(p_o);
		}
		return -1;
	}

	int WorldManager::removeObject(Object* p_o) {
		if (this->isStarted()) {
			return m_updates.remove(p_o);
		}
		return -1;
	}

	ObjectList WorldManager::getAllObjects() const {
		if (this->isStarted()) {
			//perform a deep copy of the active objects list
			ObjectList copy = ObjectList();

			for (int i = 0; i < m_updates.getCount(); i++) {

				copy.insert(m_updates[i]);
			}
			return copy;
		}
		return ObjectList();
	}

	int WorldManager::AllObjectsCount() const {
		if (this->isStarted()) {
			return m_updates.getCount();
		}
		return -1;
	}

	int WorldManager::objectsOfTypeCount(std::string type) const {
		if (this->isStarted()) {
			return m_updates.objectsOfTypeCount(type);
		}
		return -1;
	}

	ObjectList WorldManager::objectsOfType(std::string type) {
		if (this->isStarted()) {
			//efficently find and assemble a new list of all the ovbjects of a type
			return m_updates.objectsOfType(type);
		}
		return ObjectList();
	}

	void WorldManager::update() {
		if (this->isStarted()) {
			
			//run the deletions script
			for (int i = 0; i < m_deletions.getCount(); i++) {
				delete m_deletions[i];
			}
			//clear the deletetions
			m_deletions.clear();
			//put update code here 
			Object* objectCache;
			
			for (int index = 0; index < m_updates.getCount(); index++) {
				objectCache = m_updates[index];
				// Add velocity to position.
				Vector new_pos = objectCache->predictPosition();
				if (new_pos != objectCache->getPosition()) {
					//move the object
					this->moveObject(objectCache, new_pos);
					
				}
			}
				
			
		}
		
	}
	// Indicate Object is to be deleted at end of current game loop.
			// Return 0 if ok, else -1.
	int WorldManager::markForDelete(Object* p_o) {
		if (this->isStarted()) {
			return m_deletions.insert(p_o);
		}
		return -1;
	}

	void WorldManager::draw() {
		if (this->isStarted()) {
			std::vector<ObjectList> zWorld = m_updates.objectsByAltitude();
			DisplayManager& dm = DisplayManager::getInstance();
			ObjectList currentAlt;
			for (int alt = 0; alt <= MAX_ALTITUDE; alt++) {
				const ObjectList& currentAlt = zWorld[alt];
				
				if (!currentAlt.isEmpty()) {
				
					for (int layerIDX = 0; layerIDX < currentAlt.getCount(); layerIDX++) {
						
						Object* obj = currentAlt[layerIDX];
						
						if (obj->getVisible()) {
							
							dm.setApplyCamera(obj->getCameraAffected());
							obj->draw();
							
						}
						
					}
				}
				
			}
			
		}
	}

	// Return list of Objects collided with at position 'where'.
			// Collisions only with solid Objects.
			// Does not consider if po is solid or not.
	ObjectList WorldManager::getCollisions(const Object* p_o, Vector where) {
		ObjectList collsions;


		if (this->isStarted()) {
			Object* testObj;
			for (int obj = 0; obj < m_updates.getCount(); obj++) {
				testObj = m_updates[obj];

				
				if ((p_o != testObj) && (testObj->isSolid()) && (boxIntersectsBox(getWorldBox(testObj), getWorldBox(p_o, where)))) {
					collsions.insert(testObj);
				}
			}
		}
		
		
		return collsions;
	}

	// Move Object.
	// If collision with solid, send collision events.
	// If no collision with solid, move ok else don't move Object.
	// If Object is Spectral, move ok.
	// Return 0 if move ok, else -1 if collision with solid.
	int WorldManager::moveObject(Object* p_o, Vector where) {
		//ok, i hate this algorithm, its so strangely written and slow, its O((floor(d)+1)n^2) time complexity, but it prevents most collision errors, so im keeping it. there probably is better math for tihs, but i sure as hell dont know it!
		if (this->isStarted()) {
			DisplayManager& window = DisplayManager::getInstance();
			Box windowBox = Box(Vector(), window.getHorizontal(), window.getVertical());
			if (p_o->isSolid()) {

				//save the start position so we can use it later
				Vector startPos = p_o->getPosition();
				//create the vector we will be checking with
				Vector currentPos = Vector(startPos);
				//calculate the ammount we will be moving by each step
				Vector stepMovement = where - Vector(startPos);
				//find the number of times we will step
				int steps = stepMovement.getMagnitude();

				stepMovement.normalize();
				
				//init the objects on the stack here to avoid repeat allocations
				ObjectList collisions;
				bool canMove;

				Vector lastPostition = startPos;
				//event for if we end up
				//loop through heach step
				for (int step = 0; step < steps; step++) {
					//move to the next position to check
					lastPostition = currentPos;
					currentPos = currentPos + stepMovement;
					//find all collisions
					collisions = this->getCollisions(p_o, currentPos);

					
					//set up can move
					canMove = true;
					//if we hit something
					if (!collisions.isEmpty()) {
						//define these vars here to prevent frequent stack reallocs
						Object* objCache;
						EventCollision boop;
						//loop through the collisons and send out the collsion events for every collision and check hte can move value with this funny one liner
						for (int obj = 0; obj < collisions.getCount(); obj++) {
							objCache = collisions[obj];
							boop = EventCollision(p_o, objCache, currentPos);
							p_o->eventHandler(&boop);
							objCache->eventHandler(&boop);
							if (canMove) {
								canMove = !(((p_o->getSolidness() == HARD) && (objCache->getSolidness() == HARD)) || (p_o->getNoSoft() && (objCache->getSolidness() == SOFT)));
							}
						}
						if (!canMove) {
							if (currentPos != p_o->getPosition()) {
								
								if (!boxIntersectsBox(m_world_box, getWorldBox(p_o, lastPostition))) {
									EventOut out;
									p_o->eventHandler(&out);
								}
								if (!frustromCheck(p_o)) {
									EventCameraOut out2;
									p_o->eventHandler(&out2);
								}
							}
							return -1;
						}
						else {
							p_o->setPosition(currentPos);
							
						}
					}
					
				}
				//final check. i know dry principle, but im trying not to break the og spec too much
				//set the final pos
				currentPos = where;
				//check the final pos
				collisions = this->getCollisions(p_o, currentPos);
				//set up can move
				canMove = true;

				//if we hit something
				if (!collisions.isEmpty()) {
					//define these vars here to prevent frequent stack reallocs
					Object* objCache;
					EventCollision boop;
					//loop through the collisons and send out the collsion events for every collision and check hte can move value with this funny one liner
					for (int obj = 0; obj < collisions.getCount(); obj++) {
						objCache = collisions[obj];
						boop = EventCollision(p_o, objCache, where);
						p_o->eventHandler(&boop);
						objCache->eventHandler(&boop);
						if (canMove) {
							canMove = !(((p_o->getSolidness() == HARD) && (objCache->getSolidness() == HARD)) || (p_o->getNoSoft() && (objCache->getSolidness() == SOFT)));
						}
						
					}
					if (!canMove) {

						return -1;
					}
				}

			}
			
			if (where != p_o->getPosition()) {
				if (!boxIntersectsBox(m_world_box, getWorldBox(p_o, where))) {
					EventOut out;
					p_o->eventHandler(&out);
				}
				if (!frustromCheck(p_o)) {
					EventCameraOut out2;
					p_o->eventHandler(&out2);
				}
			}
			p_o->setPosition(where);
			return 0;
		}
		return -1;
	}
}