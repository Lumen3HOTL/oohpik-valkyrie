#include "TestObject.h"
#include "LogManager.h"
#include "EventStep.h"
#include "GameManager.h"
#include "WorldManager.h"
#include "Event.h"
#include "ObjectList.h"
#include "DisplayManager.h"
#include <iostream>
#include "Color.h"
#include "TestDrawObject.h"
#include "EventOut.h"
#include "AnimationTestobject.h"
#include "ResourceManager.h"
namespace test {
	TestObject::TestObject() {
		
		this->setType("TestObject");
		m_runtime = 20;
		m_frameCounter = 0;
		m_firstReceivedStep = false;
		m_testObjects = df::ObjectList();
		m_testAnimObjects = df::ObjectList();
		m_dmode = false;
		m_count2 = 0;
		m_count1 = 0;
		m_count0 = 0;
		m_dir = 0;
		m_timer = df::Clock();
		m_circulator = 0;
		this->setSolidness(df::SPECTRAL);
	}

	TestObject::~TestObject() {
		m_testObjects.clear();
	}

	int  TestObject::draw() {
		if (this->getVisible()) {
			std::string defaultTexture = "";
			switch (m_circulator) {
			case(0):
				defaultTexture = "<test>";
				break;
			case(1):
				defaultTexture = "<estt>";
				break;
			case(2):
				defaultTexture = "<stte>";
				break;
			case(3):
				defaultTexture = "<ttes>";
				break;
			default:
				m_circulator = 0;
				defaultTexture = "<test>";
				break;
			}
			

			
			df::DisplayManager& dman = df::DisplayManager::getInstance();
			
			if (!dman.setCustomColor(df::RGBAToUInt32ColorConverter(rand()%256,rand()%256,rand()%256,255))) {
				return -1;
			}
			if (m_frameCounter % 5 == 0) {
				return dman.drawString(this->getPosition(), defaultTexture, df::LEFT_JUSTIFIED, df::UNDEFINED_COLOR);
			}
			else {
				
				return dman.drawString(this->getPosition(), defaultTexture, df::LEFT_JUSTIFIED, df::CUSTOM_COLOR);
			}



		}
		
		return 0;
	}


	int TestObject::eventHandler(const df::Event* p_e) {
		//handle base events
		if (p_e->getType().compare(df::UNDEFINED_EVENT) == 0) {
			df::LogManager::getInstance().writeLog("base event received");
			return 1;
		}

		//the handler that makes the test code work
		if (p_e->getType().compare(df::STEP_EVENT) == 0) {
			//first step, test the otehr event, and test remove it
			if (m_firstReceivedStep == false) {
				df::LogManager::getInstance().setFlush(true);
				df::LogManager::getInstance().writeLog("first step event received");
				df::ObjectList testList = df::WorldManager::getInstance().objectsOfType(df::UNDEFINED_OBJECT);
				if (testList.getCount() > 0) {
					df::Object* testRemove = testList[0];
					df::WorldManager::getInstance().markForDelete(testRemove);
					df::Event* test =new  df::Event();
					this->eventHandler(test);
					delete test;
				}
				//go somewhere vsible to be polite
				this->setPosition(df::Vector(20, 10));
				//start the delta timer
				m_timer.delta();
				m_firstReceivedStep = true;
			}
			//test and log if the previous remove failed
			if (m_frameCounter == 1) {
				std::string passed = "failed";
				if (df::WorldManager::getInstance().objectsOfTypeCount(df::UNDEFINED_OBJECT) == 0) {
					passed = "passed";
				}
				df::LogManager::getInstance().writeLog("%s %s", "object remove test result: ", passed.c_str());
			}
			
			//between frame 5 and 10, incrmentally add new objects to the world
			else if ((m_frameCounter >= 5) && (m_frameCounter <= 10)) {
				df::Object* newPointer = new Object();
				//line up your test objects all in a row
				m_count2 += 8;
				newPointer->setPosition(df::Vector(m_count2, 8));
				m_testObjects.insert(newPointer);
			
				
			}
			//on frame 11, test that we have the correct number of objects
			else if ((m_frameCounter > 10) && (m_frameCounter < 12)) {
				std::string passed = "failed";
				if (df::WorldManager::getInstance().objectsOfTypeCount(df::UNDEFINED_OBJECT) == 6) {
					passed = "passed";
				}
				df::LogManager::getInstance().writeLog("%s %s", "object multiple frame add test result: ", passed.c_str());
			}
			//between frames 12 and 17, incrmentally remove the objects we just added
			else if ((m_frameCounter >= 12) && (m_frameCounter <= 17)) {
				
				df::WorldManager::getInstance().markForDelete(m_testObjects[0]);
				m_testObjects.remove(m_testObjects[0]);
				
			}
			//on frame 18, test that the removals were successful
			else if ((m_frameCounter > 17) && (m_frameCounter < 19)) {

				std::string passed = "failed";
				if (df::WorldManager::getInstance().objectsOfTypeCount(df::UNDEFINED_OBJECT) == 0) {
					passed = "passed";
				}
				df::LogManager::getInstance().writeLog("%s %s", "object multiple frame remove test result: ", passed.c_str());
				m_count2 = 0;
			}
			//on frame 20, add 5 objects all at once
			else if ((m_frameCounter > 19) && (m_frameCounter < 21)) {
				for (int i = 0; i < 5; i++) {
					df::Object* newPointer = new Object();
					m_count2 += 8;
					newPointer->setPosition(df::Vector(m_count2, 8));
					int error = m_testObjects.insert(newPointer);
					
				}
				
			}
			if (m_frameCounter % 5 == 1) {
				m_circulator++;
			}
			//on frame 21 check that they were added successfully
			else if ((m_frameCounter > 20) && (m_frameCounter < 22)) {
				std::string passed = "failed";
				if (df::WorldManager::getInstance().objectsOfTypeCount(df::UNDEFINED_OBJECT) == 5) {
					passed = "passed";
				}
				df::LogManager::getInstance().writeLog("%s %s", "object sigle frame multiple add test result: ", passed.c_str());
			}
			//on frame 22 mark the objects for removal all at once
			else if ((m_frameCounter > 21) && (m_frameCounter < 23)) {
				for (int i = 0; i < 5; i++) {
					
					
					df::WorldManager::getInstance().markForDelete(m_testObjects[0]);
					m_testObjects.remove(m_testObjects[0]);
				}

			}
			//on frame 23 check that the remove succeded
			else if ((m_frameCounter > 22) && (m_frameCounter < 24)) {
				std::string passed = "failed";
				if (df::WorldManager::getInstance().objectsOfTypeCount(df::UNDEFINED_OBJECT) == 0) {
					passed = "passed";
				}
				df::LogManager::getInstance().writeLog("%s %s", "object sigle frame multiple remove test result: ", passed.c_str());
			}
			
			else if (m_frameCounter == 90) {
				//change the background color at runtime test
				df::DisplayManager::getInstance().setBackgroundColor(df::BLUE);
			}



			
			//test differnt levels and overlapping and the kinematics system
			if (m_frameCounter == 31) {
				test::TestDrawObject* levelTest1 = new test::TestDrawObject();
				levelTest1->setPosition(df::Vector(20, 15));
				levelTest1->setAltitude(0);
				levelTest1->setColor(df::GREEN);
				levelTest1->setSprite("levelTest11111111");
				m_testObjects.insert(levelTest1);
				test::TestDrawObject* levelTest2 = new test::TestDrawObject();
				levelTest2->setSprite("levelTest22222222");
				levelTest2->setPosition(df::Vector(25, 15));
				levelTest2->setColor(df::RED);
				m_testObjects.insert(levelTest2);
				//grab and set up the collsion test objects in a new configuration to test collision
				df::Vector direction;
				direction.directionMovement(0, 0.5);
				df::ObjectList vtests = df::WorldManager::getInstance().getAllObjects().objectsOfType("VelocityTestObject");
				vtests[0]->setDirection(direction);
				
				vtests[0]->setSpeed(0.5);
				direction.directionMovement(90, -1);
				vtests[2]->setDirection(direction);
		
				vtests[2]->setSpeed(8);
				direction.directionMovement(138, 2000);
				vtests[1]->setDirection(direction);
			
				vtests[1]->setSpeed(-2);


			}else if (m_frameCounter == 60) {
				m_testObjects[0]->setPosition(df::Vector(10, 20));
				m_testObjects[1]->setPosition(df::Vector(10, 20));
				m_testObjects[0]->setAltitude(4);
				m_testObjects[1]->setAltitude(1);

				//grab and set up the collsion test objects in a new configuration to test collision
				df::ObjectList vtests = df::WorldManager::getInstance().getAllObjects().objectsOfType("VelocityTestObject");
				df::Vector direction;
				direction.setXY(-2,0);
				vtests[0]->setSpeed(0);
				vtests[0]->setPosition(df::Vector(10,5));
				vtests[0]->setDirection(direction);
			
				vtests[0]->setSpeed(1);

				direction.setXY(2, 0);
				vtests[1]->setSpeed(0);
				vtests[1]->setPosition(df::Vector(40, 5));
				vtests[1]->setDirection(direction);
				
				vtests[1]->setSpeed(8);

				vtests[2]->setSpeed(0);
				vtests[2]->setPosition(df::Vector(30, 5));
				direction.setXY(1, 0);
				vtests[2]->setDirection(direction);
				
				vtests[2]->setSpeed(1);
			} else if (m_frameCounter == 90) {
				m_testObjects[0]->setPosition(df::Vector(40, 12));
				m_testObjects[1]->setPosition(df::Vector(40, 12));
				m_testObjects[0]->setAltitude(3);
				m_testObjects[1]->setAltitude(4);

				//grab and set up the collsion test objects in a new configuration to test collision
				df::ObjectList vtests = df::WorldManager::getInstance().getAllObjects().objectsOfType("VelocityTestObject");
				df::Vector direction;
				direction.setXY(0,1);
				vtests[0]->setSpeed(0);
				vtests[0]->setPosition(df::Vector(10, 1));
				vtests[0]->setDirection(direction);

				vtests[0]->setSpeed(1);

				direction.setXY(0, -1);
				vtests[1]->setSpeed(0);
				vtests[1]->setPosition(df::Vector(10, 24));
				vtests[1]->setDirection(direction);

				vtests[1]->setSpeed(8);

				vtests[2]->setSpeed(0);
				vtests[2]->setPosition(df::Vector(10, 16));
				direction.setXY(0, -2);
				vtests[2]->setDirection(direction);

				vtests[2]->setSpeed(1);
			} else if (m_frameCounter == 120) {
				m_testObjects[0]->setPosition(df::Vector(15, 5));
				m_testObjects[1]->setPosition(df::Vector(20, 5));
				m_testObjects[0]->setAltitude(0);
				m_testObjects[1]->setAltitude(1);
				//grab and set up the collsion test objects in a new configuration to test collision
				df::ObjectList vtests = df::WorldManager::getInstance().getAllObjects().objectsOfType("VelocityTestObject");
				df::Vector direction;
				direction.directionMovement(180, 1);
				vtests[0]->setSpeed(0);
				vtests[0]->setSolidness(df::HARD);
				vtests[0]->setPosition(df::Vector(70, 1));
				vtests[0]->setDirection(direction);

				vtests[0]->setSpeed(1);

				direction.directionMovement(180, -1);
				vtests[1]->setSpeed(0);
				vtests[1]->setSolidness(df::HARD);
				vtests[1]->setPosition(df::Vector(70, 24));
				vtests[1]->setDirection(direction);

				vtests[1]->setSpeed(8);

				vtests[2]->setSpeed(0);
				vtests[2]->setSolidness(df::HARD);
				vtests[2]->setPosition(df::Vector(70, 16));
				direction.directionMovement(0, 1);
				vtests[2]->setDirection(direction);

				vtests[2]->setSpeed(1);
			}
			else if (m_frameCounter == 150) {
				m_testObjects[0]->setPosition(df::Vector(15, 5));
				m_testObjects[1]->setPosition(df::Vector(20, 5));
				m_testObjects[0]->setAltitude(2);
				m_testObjects[1]->setAltitude(2);
				//grab and set up the collsion test objects in a new configuration to test collision
				df::ObjectList vtests = df::WorldManager::getInstance().getAllObjects().objectsOfType("VelocityTestObject");
				df::Vector direction;
				direction.directionMovement(90, 1);
				vtests[0]->setSpeed(0);
				vtests[0]->setPosition(df::Vector(10, 12));
				vtests[0]->setDirection(direction);

				vtests[0]->setSpeed(1);

				direction.directionMovement(90, -1);
				vtests[1]->setSpeed(0);
				vtests[1]->setPosition(df::Vector(40, 12));
				vtests[1]->setDirection(direction);

				vtests[1]->setSpeed(8);

				vtests[2]->setSpeed(0);
				vtests[2]->setPosition(df::Vector(30, 12));
				direction.directionMovement(-90, 1);
				vtests[2]->setDirection(direction);

				vtests[2]->setSpeed(1);
			}
			else if (m_frameCounter == 180) {
				m_testObjects[0]->setPosition(df::Vector(30, 8));
				m_testObjects[1]->setPosition(df::Vector(25, 8));
				m_testObjects[0]->setAltitude(3);
				m_testObjects[1]->setAltitude(3);

				//grab and set up the collsion test objects in a new configuration to test collision
				df::ObjectList vtests = df::WorldManager::getInstance().getAllObjects().objectsOfType("VelocityTestObject");
				df::Vector direction;
				direction.directionMovement(180, 1);
				vtests[0]->setSpeed(0);
				vtests[0]->setSolidness(df::SPECTRAL);
				vtests[0]->setPosition(df::Vector(40, 1));
				vtests[0]->setDirection(direction);

				vtests[0]->setSpeed(1);

				direction.directionMovement(180, -1);
				vtests[1]->setSpeed(0);
				vtests[1]->setSolidness(df::SPECTRAL);
				vtests[1]->setPosition(df::Vector(40, 24));
				vtests[1]->setDirection(direction);

				vtests[1]->setSpeed(8);

				vtests[2]->setSpeed(0);
				vtests[2]->setSolidness(df::SPECTRAL);
				vtests[2]->setPosition(df::Vector(40, 16));
				direction.directionMovement(0, 1);
				vtests[2]->setDirection(direction);

				vtests[2]->setSpeed(1);
			}
			else if (m_frameCounter == 210) {
				m_testObjects[0]->setPosition(df::Vector(30, 8));
				m_testObjects[1]->setPosition(df::Vector(25, 8));
				int error = m_testObjects[0]->setAltitude(8);
				if (error == -1) {
					df::LogManager::getInstance().writeLog("%s %s", "object invalid layer set test 1 result: ", std::to_string(error).c_str());
					df::LogManager::getInstance().writeLog("%s %s", "object invalid layer set test 1 final layer: ", std::to_string(m_testObjects[0]->getAltitude()).c_str());
					df::LogManager::getInstance().writeLog("%s %s", "object invalid layer set test 1", "passed");
				}
				error = m_testObjects[1]->setAltitude(-10);
				if (error == -1) {
					df::LogManager::getInstance().writeLog("%s %s", "object invalid layer set test 2 result: ", std::to_string(error).c_str());
					df::LogManager::getInstance().writeLog("%s %s", "object invalid layer set test 2 final layer: ", std::to_string(m_testObjects[1]->getAltitude()).c_str());
					df::LogManager::getInstance().writeLog("%s %s", "object invalid layer set test 2", "passed");
				}
				//grab and set up the collsion test objects in a new configuration to test collision
				df::ObjectList vtests = df::WorldManager::getInstance().getAllObjects().objectsOfType("VelocityTestObject");
				df::Vector direction;
				direction.directionMovement(90, 1);
				vtests[0]->setSpeed(0);
				vtests[0]->setPosition(df::Vector(10, 20));
				vtests[0]->setDirection(direction);

				vtests[0]->setSpeed(1);

				direction.directionMovement(90, -1);
				vtests[1]->setSpeed(0);
				vtests[1]->setPosition(df::Vector(40, 20));
				vtests[1]->setDirection(direction);

				vtests[1]->setSpeed(8);

				vtests[2]->setSpeed(0);
				vtests[2]->setPosition(df::Vector(30, 20));
				direction.directionMovement(-90, 1);
				vtests[2]->setDirection(direction);

				vtests[2]->setSpeed(1);
			}
			else if (m_frameCounter == 240) {

				m_testObjects[0]->setPosition(df::Vector(30, 21));
				m_testObjects[1]->setPosition(df::Vector(30, 22));
				//grab and set up the collsion test objects in a new configuration to test collision
				df::ObjectList vtests = df::WorldManager::getInstance().getAllObjects().objectsOfType("VelocityTestObject");
				df::Vector direction;
				direction.directionMovement(180, 1);
				vtests[0]->setSpeed(0);
				vtests[0]->setSolidness(df::HARD);
				vtests[0]->setPosition(df::Vector(40, 1));
				vtests[0]->setDirection(direction);

				vtests[0]->setSpeed(1);

				direction.directionMovement(180, -1);
				vtests[1]->setSpeed(0);
				vtests[1]->setSolidness(df::SOFT);
				vtests[1]->setPosition(df::Vector(40, 24));
				vtests[1]->setDirection(direction);

				vtests[1]->setSpeed(8);

				vtests[2]->setSpeed(0);
				vtests[2]->setSolidness(df::SPECTRAL);
				vtests[2]->setPosition(df::Vector(40, 16));
				direction.directionMovement(0, 1);
				vtests[2]->setDirection(direction);

				vtests[2]->setSpeed(1);
				
				
			}
			else if (m_frameCounter == 280) {
				//grab and set up the collsion test objects in a new configuration to test collision
				df::ObjectList vtests = df::WorldManager::getInstance().getAllObjects().objectsOfType("VelocityTestObject");
				if (!vtests.isEmpty()) {
					for (int i = 0; i < vtests.getCount(); i++) {
						df::WorldManager::getInstance().markForDelete(vtests[i]);
					}
				}
				m_timer.delta();
			}
			
			//self explanitory frame timing test
			else if (m_frameCounter == 282) {
				long long timestamp = m_timer.delta();
				std::string result = "failed";
				if (((timestamp / 1000) > (33)) && ((timestamp / 1000) < (33 * 3))) {
					result = "passed";
				}
				df::LogManager::getInstance().writeLog("%s %s", "30th frame delta test:", result.c_str());
				df::LogManager::getInstance().writeLog("%s %s", "30th frame delta:", std::to_string(timestamp / 1000).c_str());
				df::LogManager::getInstance().writeLog("%s %s", "30th frame delta target:", std::to_string((66)).c_str());
				df::LogManager::getInstance().writeLog("%s%s", "30th frame delta tollerances: +-", std::to_string(33).c_str());
			}
			//update the frame counter
			m_frameCounter+=1;
			//increment one of the many counters
			m_count1 += 1;
			//every three frames move
			if (m_count1 >= 2) {

				//if we are in angular mode use my directional movement extention to move in a diamond
				if (m_dmode==1) {
					df::Vector movement;
					movement = df::Vector(this->getPosition().getX(), this->getPosition().getY());
					switch (m_dir) {
					case 0:
						movement.directionMovement(45, 0.4);
						break;
					case 1:
						movement.directionMovement(135, 0.4);
						break;
					case 2:
						movement.directionMovement(225, 0.4);
						break;
					case 3:
						movement.directionMovement(315, 0.4);
						break;
					}
					this->setPosition(this->getPosition() + movement);
				}
				//otherwise move in a square normally
				else if(m_dmode==0) {
					switch (m_dir) {
					case 0:
						this->setPosition(df::Vector(this->getPosition().getX() + 0.5, this->getPosition().getY()));
						break;
					case 1:
						this->setPosition(df::Vector(this->getPosition().getX(), this->getPosition().getY() + 0.2));
						break;
					case 2:
						this->setPosition(df::Vector(this->getPosition().getX() - 0.5, this->getPosition().getY()));
						break;
					case 3:
						this->setPosition(df::Vector(this->getPosition().getX(), this->getPosition().getY() - 0.2));
						break;

					}
				}
				//otherwise bounce back and forth
				else if(m_dmode==2){
					
					if (this->getPosition().getX()+2 > (80-5)) {
						this->m_dmode = 3;
					}
					else {
						this->setPosition(df::Vector(this->getPosition().getX() + 2, this->getPosition().getY()));
					}
						
					
				}
				else {
					
					if (this->getPosition().getX()-2 < 0) {
						
						this->m_dmode = 2;
					}
					else {
						this->setPosition(df::Vector(this->getPosition().getX() - 2, this->getPosition().getY()));
					}
					
				}
				//update the direction change count
				m_count0++;
				m_count1 = 0;
				//change direction every 9 steps
				if (m_count0 > 8) {
					m_dir++;
					m_count0 = 0;
				}
				//reset direction after 4 chagnes
				if (m_dir > 3) {

					m_dir = 0;
				}
			}
			
			if (m_frameCounter == 300) {
				AnimationTestObject* test1 = new AnimationTestObject();
				AnimationTestObject* test2 = new AnimationTestObject();
				AnimationTestObject* test3 = new AnimationTestObject();
				m_testAnimObjects.insert(test1);
				m_testAnimObjects.insert(test2);
				m_testAnimObjects.insert(test3);

			}
			else if (m_frameCounter == 330) {
				df::LogManager& lm = df::LogManager::getInstance();
				if (m_testAnimObjects.getCount() == 3) {
					m_testAnimObjects[0]->setPosition(df::Vector(10, 10));
					m_testAnimObjects[1]->setPosition(df::Vector(70, 10));
					m_testAnimObjects[2]->setPosition(df::Vector(40, 16));
					df::ResourceManager& rm = df::ResourceManager::getInstance();
					
					int test = 0;
					test = rm.loadSprite("demo_sprite_0.txt", "demo0");
					if (test == -1) {
						lm.writeLog("animation test error! could not load sprite demo_sprite_0.txt");
					}
					else {
						lm.writeLog("animation load test 0 successful!");
					}
					test = rm.loadSprite("demo_sprite_1.txt", "demo1");
					if (test == -1) {
						lm.writeLog("animation test error! could not load sprite demo_sprite_1.txt");
					}
					else {
						lm.writeLog("animation load test 0 successful!");
					}
					test = rm.loadSprite("demo_sprite_2.txt", "demo2");
					if (test == -1) {
						lm.writeLog("animation test error! could not load sprite demo_sprite_2.txt");
					}
					else {
						lm.writeLog("animation load test 2 successful!");
					}

					test = m_testAnimObjects[0]->setSprite("demo2");
					if (test == -1) {
						lm.writeLog("animation test error! could not set animation \"demo2\"!");
					}
					else {
						lm.writeLog("animation set test 0 successful!");
					}
					test = m_testAnimObjects[1]->setSprite("demo1");
					if (test == -1) {
						lm.writeLog("animation test error! could not set animation \"demo1\"!");
					}
					else {
						lm.writeLog("animation set test 1 successful!");
					}
					test = m_testAnimObjects[2]->setSprite("demo0");
					if (test == -1) {
						lm.writeLog("animation test error! could not set animation \"demo0\"!");
					}
					else {
						lm.writeLog("animation set test 2 successful!");
					}
				}else{
					lm.writeLog("%s %d", "animation test error! invalid test object count! intended count: 3, found count: ", m_testAnimObjects.getCount());
				}
				
				
			}
			else if (m_frameCounter == 360) {
				df::LogManager& lm = df::LogManager::getInstance();
				if (m_testAnimObjects.getCount() == 3) {
					m_testAnimObjects[0]->getAnimation().setSlowdownCount(-1);
					m_testAnimObjects[0]->getAnimation().setIndex(0);
					m_testAnimObjects[1]->getAnimation().getSprite()->setSlowdown(m_testAnimObjects[1]->getAnimation().getSprite()->getSlowdown() + 5);
					
				}
				else {
					lm.writeLog("%s %d", "animation test error! invalid test object count! intended count: 3, found count: ", m_testAnimObjects.getCount());
				}
			}
			else if (m_frameCounter == 390) {
				df::LogManager& lm = df::LogManager::getInstance();
				if (m_testAnimObjects.getCount() == 3) {
					
					m_testAnimObjects[0]->getAnimation().setIndex(1);

				}
				else {
					lm.writeLog("%s %d", "animation test error! invalid test object count! intended count: 3, found count: ", m_testAnimObjects.getCount());
				}
			}
			else if (m_frameCounter == 420) {
				df::LogManager& lm = df::LogManager::getInstance();
				if (m_testAnimObjects.getCount() == 3) {
					m_testAnimObjects[0]->getAnimation().setSlowdownCount(0);
					df::Animation temp0 = m_testAnimObjects[0]->getAnimation();
					df::Animation temp1 = m_testAnimObjects[1]->getAnimation();
					m_testAnimObjects[0]->setAnimation(temp1);
					m_testAnimObjects[1]->setAnimation(temp0);
				}
				else {
					lm.writeLog("%s %d", "animation test error! invalid test object count! intended count: 3, found count: ", m_testAnimObjects.getCount());
				}
			}
			else if (m_frameCounter == 480) {
				df::LogManager& lm = df::LogManager::getInstance();
				if(m_testAnimObjects.getCount() == 3) {
					df::WorldManager& wm = df::WorldManager::getInstance();
					wm.markForDelete(m_testAnimObjects[0]);
					wm.markForDelete(m_testAnimObjects[1]);
					wm.markForDelete(m_testAnimObjects[2]);
					m_testAnimObjects.clear();
				}
				else {
					lm.writeLog("%s %d", "animation test error! invalid test object count! intended count: 3, found count: ", m_testAnimObjects.getCount());
				}
			}

			else if (m_frameCounter == 482) {
				df::LogManager& lm = df::LogManager::getInstance();
				df::ResourceManager& rm = df::ResourceManager::getInstance();
				int error=rm.unloadSprite("demo0");
				if (error == -1) {
					lm.writeLog("live resource manager sprite deletetion test 0 failed!");
				}
				else {
					lm.writeLog("live resource manager sprite deletetion test 0 success!");
				}
				error = rm.unloadSprite("demo1");
				if (error == -1) {
					lm.writeLog("live resource manager sprite deletetion test 0 failed!");
				}
				else {
					lm.writeLog("live resource manager sprite deletetion test 0 success!");
				}
				error = rm.unloadSprite("demo2");
				if (error == -1) {
					lm.writeLog("live resource manager sprite deletetion test 0 failed!");
				}
				else {
					lm.writeLog("live resource manager sprite deletetion test 0 success!");
				}
			}
			//change movement mode after 4 seconds 
			if ((m_frameCounter > (30 * 4)) && (m_frameCounter < (30 * 4) + 2)) {
				m_dmode = 1;
			}
			//change movement mode after 4 seconds 
			else if ((m_frameCounter > (30 * 8)) && (m_frameCounter < (30 * 9) + 2)) {
				m_dmode = 2;
				this->setPosition(df::Vector(30, 10));
			}
			//after (m_runtime / 30) seconds, shut down the engine
			if (m_frameCounter == (30 * m_runtime) - 2) {
				df::WorldManager::getInstance().markForDelete(m_testObjects[0]);
				df::WorldManager::getInstance().markForDelete(m_testObjects[1]);
				m_testObjects.remove(m_testObjects[0]);
				m_testObjects.remove(m_testObjects[0]);
			}
			if (m_frameCounter > (30 * m_runtime)) {
				
				df::LogManager::getInstance().writeLog("%d %s",m_runtime / 30, "second test shutdown triggered");
				df::GameManager::getInstance().setGameOver(true);
			}
			//if the engine didnt shut down, log it
			else if (m_frameCounter > ((30 * m_runtime) + 1)) {
				df::LogManager::getInstance().writeLog("%s", "auto delayed shutdown test failed!");
			}
			return 1;
		}

		return 0;
		//obvoiusly this is extremely messy test code and very inneficent, but i have a 13th gen i9 so i dont care. it works and thats all that matters 
	}
}