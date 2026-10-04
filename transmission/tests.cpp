#include "LogManager.h"
#include "Clock.h"
#include "GameManager.h"
#include "windows.h"
#include "Vector.h"
#include "Object.h"
#include "ObjectList.h"
#include "TestObject.h"
#include <cstdio>
#include "WorldManager.h"
#include "EventStep.h"
#include <iostream> // for std::cout
#include <SFML/Graphics.hpp>
#include <math.h>
#include "DisplayManager.h"
#include "InputTestDisplayObject.h"
#include "InputManager.h"
#include <Windows.h>
#include "KeyMoveTestObject.h"
#include "VelocityTestObject.h"
#include "Frame.h"
#include "Sprite.h"
#include "ResourceManager.h"
#include "Color.h"
#include "Music.h"
#include "Sound.h"

using namespace df;
namespace test {
	int runLogManagerTests() {
		int failures = 0;

		std::printf("suite: LogManager\n");

		LogManager& logkeeper = LogManager::getInstance();

		int startupResult = logkeeper.startUp();

		std::printf(
			"test: LogManager startup result: %d result: %s\n",
			startupResult,
			startupResult >= 0 ? "passed" : "failed"
		);

		if (startupResult < 0) {
			failures++;
			std::printf(
				"suite failed: LogManager failures: %d\n",
				failures
			);
			return failures;
		}

		int duplicateStartupResult = logkeeper.startUp();

		std::printf(
			"test: LogManager duplicate startup result: %d result: %s\n",
			duplicateStartupResult,
			duplicateStartupResult < 0 ? "passed" : "failed"
		);

		if (duplicateStartupResult >= 0) {
			failures++;
		}

		bool typePassed =
			logkeeper.getType().compare("LogManager") == 0;

		std::printf(
			"test: LogManager type result: %s expected: LogManager actual: %s\n",
			typePassed ? "passed" : "failed",
			logkeeper.getType().c_str()
		);

		if (!typePassed) {
			failures++;
		}

		/*
		 * The logger has now passed its basic startup and identification tests.
		 * It may be used to log the remaining LogManager tests.
		 */
		logkeeper.setFlush(true);

		logkeeper.writeLog("");
		logkeeper.writeLog("%d %i", 1, 2);
		logkeeper.writeLog("%s %% %f %u", "test", 13.0F, 99u);
		logkeeper.writeLog("+%A", 65535.0f);
		logkeeper.writeLog("%s", "this is a test");

		logkeeper.writeLog(
			"test: LogManager formatted logging result: passed"
		);

		logkeeper.shutDown();

		int writeAfterShutdownResult =
			logkeeper.writeLog("%s", "this should fail");

		/*
		 * LogManager is shut down, so this final test must be logged without
		 * using LogManager.
		 */
		bool writeAfterShutdownPassed =
			writeAfterShutdownResult == -1;

		std::printf(
			"test: LogManager write after shutdown result: %d result: %s\n",
			writeAfterShutdownResult,
			writeAfterShutdownPassed ? "passed" : "failed"
		);

		if (!writeAfterShutdownPassed) {
			failures++;
		}

		if (failures > 0) {
			std::printf(
				"suite failed: LogManager failures: %d\n",
				failures
			);
		}
		else {
			std::printf("suite passed: LogManager\n");
		}

		return failures;
	}

	int logTestResult(LogManager& logkeeper, const char* testName, bool passed) {
		logkeeper.writeLog(
			"test: %s result: %s",
			testName,
			passed ? "passed" : "failed"
		);

		return passed ? 0 : 1;
	}

	int logSuiteResult(LogManager& logkeeper, const char* suiteName, int failures) {
		if (failures > 0) {
			logkeeper.writeLog(
				"suite failed: %s failures: %d",
				suiteName,
				failures
			);
		}
		else {
			logkeeper.writeLog(
				"suite passed: %s",
				suiteName
			);
		}

		return failures;
	}

	

	int runClockTests() {
		LogManager& logkeeper = LogManager::getInstance();
		int failures = 0;

		logkeeper.startUp(true);
		logkeeper.setFlush(true);

		timeBeginPeriod(1);

		Clock testClock = Clock();

		logkeeper.writeLog("%s", "Clock delta test starting");
		testClock.delta();

		Sleep(2000);

		int firstDelta = testClock.split() / 1000;
		logkeeper.writeLog(
			"Clock split result: %d seconds",
			firstDelta
		);

		failures += logTestResult(
			logkeeper,
			"Clock split approximately two seconds",
			firstDelta >= 1
		);

		logkeeper.writeLog("%s", "Clock second delta test starting");
		testClock.delta();

		Sleep(2000);

		int secondDelta = testClock.delta() / 1000;
		logkeeper.writeLog(
			"Clock delta result: %d seconds",
			secondDelta
		);

		failures += logTestResult(
			logkeeper,
			"Clock delta approximately two seconds",
			secondDelta >= 1
		);

		logkeeper.shutDown();
		timeEndPeriod(1);

		return logSuiteResult(logkeeper, "Clock", failures);
	}

	int runGameManagerTests() {
		LogManager& logkeeper = LogManager::getInstance();
		int failures = 0;

		
		

		GameManager& testMan = GameManager::getInstance();
		
		int firstStartup = testMan.startUp();
		logkeeper.setFlush(true);
		logkeeper.writeLog(
			"GameManager startup test 0 result: %d",
			firstStartup
		);

		failures += logTestResult(
			logkeeper,
			"GameManager first startup",
			firstStartup == 0
		);

		int secondStartup = testMan.startUp();

		logkeeper.writeLog(
			"GameManager startup test 1 result: %d",
			secondStartup
		);

		failures += logTestResult(
			logkeeper,
			"GameManager duplicate startup",
			secondStartup == -1
		);

		testMan.shutDown();
		if (!logkeeper.isStarted()) {
			logkeeper.startUp(true);
		}
		logSuiteResult(logkeeper, "GameManager", failures);
		logkeeper.shutDown();

		return failures;
	}

	int runVectorTests() {
		LogManager& logkeeper = LogManager::getInstance();
		int failures = 0;

		logkeeper.startUp(true);
		logkeeper.setFlush(true);

		Vector test;

		const float testval0 = 10.5F;
		const float testval1 = 11.3F;

		logkeeper.writeLog(
			"Vector setX/setY challenge x: %f challenge y: %f",
			testval0,
			testval1
		);

		test.setX(testval0);
		test.setY(testval1);

		logkeeper.writeLog(
			"Vector setX/setY response x: %f response y: %f",
			test.getX(),
			test.getY()
		);

		failures += logTestResult(
			logkeeper,
			"Vector setX/setY",
			test.getX() == testval0 && test.getY() == testval1
		);

		const float testVal2 = 102.8F;
		const float testVal3 = 158.9F;

		logkeeper.writeLog(
			"Vector setXY challenge x: %f challenge y: %f",
			testVal2,
			testVal3
		);

		test.setXY(testVal2, testVal3);

		logkeeper.writeLog(
			"Vector setXY response x: %f response y: %f",
			test.getX(),
			test.getY()
		);

		failures += logTestResult(
			logkeeper,
			"Vector setXY",
			test.getX() == testVal2 && test.getY() == testVal3
		);

		const float testval4 = 1;
		const float testval5 = 1;

		test.setXY(testval4, testval5);
		test.normalize();

		logkeeper.writeLog(
			"Vector normalize challenge x: %f challenge y: %f",
			testval4,
			testval5
		);

		logkeeper.writeLog(
			"Vector normalize response x: %f response y: %f magnitude: %f",
			test.getX(),
			test.getY(),
			test.getMagnitude()
		);

		failures += logTestResult(
			logkeeper,
			"Vector normalize",
			test.getMagnitude() > 0.99F &&
			test.getMagnitude() < 1.01F
		);

		Vector test2;
		test2.setXY(testval4, testval5);

		Vector test3 = test + test2;

		logkeeper.writeLog(
			"Vector addition response x: %f response y: %f",
			test3.getX(),
			test3.getY()
		);

		failures += logTestResult(
			logkeeper,
			"Vector addition",
			test3.getX() == test.getX() + test2.getX() &&
			test3.getY() == test.getY() + test2.getY()
		);

		const float testval10 = 3;
		const float testval11 = 4;

		test3.setXY(testval10, testval11);

		logkeeper.writeLog(
			"Vector subtraction challenge x: %f challenge y: %f",
			test3.getX(),
			test3.getY()
		);

		test3 = test3 - test2;

		logkeeper.writeLog(
			"Vector subtraction response x: %f response y: %f",
			test3.getX(),
			test3.getY()
		);

		failures += logTestResult(
			logkeeper,
			"Vector subtraction",
			test3.getX() == testval10 - test2.getX() &&
			test3.getY() == testval11 - test2.getY()
		);

		const float testval6 = 4;
		const float testval7 = 8;
		const float testval8 = 2;

		test2.setXY(testval6, testval7);
		test3.setXY(testval8, testval4);

		logkeeper.writeLog(
			"Vector division challenge x: %f challenge y: %f",
			test3.getX(),
			test3.getY()
		);

		test3 = test3 / test2;

		logkeeper.writeLog(
			"Vector division response x: %f response y: %f",
			test3.getX(),
			test3.getY()
		);

		failures += logTestResult(
			logkeeper,
			"Vector division",
			test3.getX() == testval8 / testval6 &&
			test3.getY() == testval4 / testval7
		);

		test2.setXY(testval6, testval7);
		test3.setXY(testval8, testval4);

		logkeeper.writeLog(
			"Vector multiplication challenge x: %f challenge y: %f",
			test3.getX(),
			test3.getY()
		);

		test3 = test3 * test2;

		logkeeper.writeLog(
			"Vector multiplication response x: %f response y: %f",
			test3.getX(),
			test3.getY()
		);

		failures += logTestResult(
			logkeeper,
			"Vector multiplication",
			test3.getX() == testval8 * testval6 &&
			test3.getY() == testval4 * testval7
		);

		logSuiteResult(logkeeper, "Vector", failures);
		logkeeper.shutDown();

		return failures;
	}

	int runObjectTests() {
		LogManager& logkeeper = LogManager::getInstance();
		int failures = 0;

		logkeeper.startUp(true);
		logkeeper.setFlush(true);

		Object test0;

		logkeeper.writeLog(
			"object type: %s",
			test0.getType().c_str()
		);

		logkeeper.writeLog(
			"object id: %d",
			test0.getId()
		);

		logkeeper.writeLog(
			"object position x: %f y: %f",
			test0.getPosition().getX(),
			test0.getPosition().getY()
		);

		Object test1;

		logkeeper.writeLog(
			"object id increment test result: %d",
			test1.getId()
		);

		failures += logTestResult(
			logkeeper,
			"Object id increment",
			test1.getId() > test0.getId()
		);

		test0.setId(14);

		logkeeper.writeLog(
			"object id after setId: %d",
			test0.getId()
		);

		failures += logTestResult(
			logkeeper,
			"Object setId",
			test0.getId() == 14
		);

		test0.setType("test");

		logkeeper.writeLog(
			"object type after setType: %s",
			test0.getType().c_str()
		);

		failures += logTestResult(
			logkeeper,
			"Object setType",
			test0.getType().compare("test") == 0
		);

		Vector testPos;
		testPos.setXY(9, 16);
		test0.setPosition(testPos);

		logkeeper.writeLog(
			"object position after setPosition x: %f y: %f",
			test0.getPosition().getX(),
			test0.getPosition().getY()
		);

		failures += logTestResult(
			logkeeper,
			"Object setPosition",
			test0.getPosition().getX() == 9 &&
			test0.getPosition().getY() == 16
		);

		logSuiteResult(logkeeper, "Object", failures);
		logkeeper.shutDown();

		return failures;
	}

	int runObjectListTests() {
		LogManager& logkeeper = LogManager::getInstance();
		int failures = 0;

		logkeeper.startUp(true);
		logkeeper.setFlush(true);

		Object testObject0;
		Object testObject1;
		Object testObject2;
		Object testObject3;

		ObjectList testList;

		logkeeper.writeLog(
			"starting size: %d empty: %d full: %d",
			testList.getCount(),
			testList.isEmpty(),
			testList.isFull()
		);

		int removeEmptyResult = testList.remove(&testObject0);

		logkeeper.writeLog(
			"remove on empty test result: %d",
			removeEmptyResult
		);

		failures += logTestResult(
			logkeeper,
			"ObjectList remove on empty list",
			removeEmptyResult < 0
		);

		int addResult = testList.insert(&testObject0);

		logkeeper.writeLog(
			"add on empty test result: %d new size: %d",
			addResult,
			testList.getCount()
		);

		failures += logTestResult(
			logkeeper,
			"ObjectList add to empty list",
			addResult >= 0 && testList.getCount() == 1
		);

		int duplicateResult = testList.insert(&testObject0);

		logkeeper.writeLog(
			"add duplicate test result: %d new size: %d",
			duplicateResult,
			testList.getCount()
		);
		
		failures += logTestResult(
			logkeeper,
			"ObjectList duplicate add",
			duplicateResult == -1
		);

		int removePresentResult = testList.remove(&testObject0);

		logkeeper.writeLog(
			"remove on present result: %d new size: %d",
			removePresentResult,
			testList.getCount()
		);

		failures += logTestResult(
			logkeeper,
			"ObjectList remove present object",
			removePresentResult >= 0 && testList.getCount() == 0
		);

		testList.insert(&testObject0);
		testList.insert(&testObject1);
		testList.insert(&testObject2);
		testList.insert(&testObject3);

		logkeeper.writeLog(
			"multiple add size: %d",
			testList.getCount()
		);

		failures += logTestResult(
			logkeeper,
			"ObjectList multiple add",
			testList.getCount() == 4
		);

		try {
			testList[-1];
			logkeeper.writeLog("invalid access test 0 failed");
			failures++;
		}
		catch (std::exception&) {
			logkeeper.writeLog("invalid access test 0 passed");
		}

		try {
			testList[99];
			logkeeper.writeLog("invalid access test 1 failed");
			failures++;
		}
		catch (std::exception&) {
			logkeeper.writeLog("invalid access test 1 passed");
		}

		try {
			testList[2];
			logkeeper.writeLog("valid access test 0 passed");
		}
		catch (std::exception&) {
			logkeeper.writeLog("valid access test 0 failed");
			failures++;
		}

		logkeeper.writeLog(
			"added size: %d empty: %d full: %d",
			testList.getCount(),
			testList.isEmpty(),
			testList.isFull()
		);

		testList.clear();

		logkeeper.writeLog(
			"cleared size: %d empty: %d full: %d",
			testList.getCount(),
			testList.isEmpty(),
			testList.isFull()
		);

		failures += logTestResult(
			logkeeper,
			"ObjectList clear",
			testList.getCount() == 0 && testList.isEmpty()
		);

		try {
			testList[2];
			logkeeper.writeLog("invalid access test 2 failed");
			failures++;
		}
		catch (std::exception&) {
			logkeeper.writeLog("invalid access test 2 passed");
		}

		logSuiteResult(logkeeper, "ObjectList", failures);
		logkeeper.shutDown();

		return failures;
	}


	int runEventTests() {
		LogManager& logkeeper = LogManager::getInstance();
		int failures = 0;

		logkeeper.startUp(true);
		logkeeper.setFlush(true);

		std::unique_ptr<Event> testEvent = std::make_unique<Event>();

		logkeeper.writeLog(
			"starting base event type: %s",
			testEvent->getType().c_str()
		);

		failures += logTestResult(
			logkeeper,
			"Event starting type",
			testEvent->getType().compare(df::UNDEFINED_EVENT) == 0
		);

		const std::string newType = "test change";
		testEvent->setType(newType);

		logkeeper.writeLog(
			"changed base event type: %s",
			testEvent->getType().c_str()
		);

		failures += logTestResult(
			logkeeper,
			"Event setType",
			testEvent->getType().compare(newType) == 0
		);

		std::unique_ptr<EventStep> testStep =
			std::make_unique<EventStep>();

		logkeeper.writeLog(
			"starting step event type: %s",
			testStep->getType().c_str()
		);

		failures += logTestResult(
			logkeeper,
			"EventStep starting type",
			testStep->getType().compare(df::STEP_EVENT) == 0
		);

		logkeeper.writeLog(
			"starting step event count: %d",
			testStep->getStepCount()
		);

		failures += logTestResult(
			logkeeper,
			"EventStep starting count",
			testStep->getStepCount() == 0
		);

		int randomTestVal = rand() % 100;
		testStep->setStepCount(randomTestVal);

		logkeeper.writeLog(
			"changed step event count: %d",
			testStep->getStepCount()
		);

		failures += logTestResult(
			logkeeper,
			"EventStep setStepCount",
			testStep->getStepCount() == randomTestVal
		);

		logSuiteResult(logkeeper, "Event", failures);
		logkeeper.shutDown();

		return failures;
	}

	int runLifecycleTests() {
		LogManager& logkeeper = LogManager::getInstance();
		int failures = 0;

		GameManager& gameMaster = GameManager::getInstance();

		int startupResult = gameMaster.startUp(true);
		logkeeper.setFlush(true);

		ResourceManager& resourceMaster = ResourceManager::getInstance();
		

		failures += logTestResult(
			logkeeper,
			"Lifecycle GameManager startup",
			startupResult == 0
		);

		startupResult=resourceMaster.startUp();

		failures += logTestResult(
			logkeeper,
			"Lifecycle resourceManager startup",
			startupResult == 0
		);

		TestObject* tester = new TestObject();
		
		InputTestDisplayObject* tester3 = new InputTestDisplayObject();
		tester3->setPosition(Vector(40, 1));
		tester3->setInEngine(true);

		Object* tester2 = new Object();
		
		KeyMoveTestObject* tester4 = new KeyMoveTestObject();


		VelocityTestObject* vtest0 = new VelocityTestObject();
		vtest0->setPosition(Vector(vtest0->getPosition().getX(), vtest0->getPosition().getY()+1));
		

		VelocityTestObject* vtest1 = new VelocityTestObject();
		vtest1->setPosition(Vector(0, 0));

		VelocityTestObject* vtest2 = new VelocityTestObject();
		vtest2->setPosition(Vector(80, 15));
		gameMaster.run();

		logkeeper.writeLog(
			"%s",
			"lifecycle tests completed successfully!"
		);

		gameMaster.shutDown();
		resourceMaster.shutDown();
		if (!logkeeper.isStarted()) {
			logkeeper.startUp(true);
		}

		logSuiteResult(logkeeper, "Lifecycle", failures);
		logkeeper.shutDown();

		return failures;
	}

	int runWorldManagerTests() {
		LogManager& logkeeper = LogManager::getInstance();
		int failures = 0;

		logkeeper.startUp(true);
		logkeeper.setFlush(true);

		WorldManager& testWorld = WorldManager::getInstance();

		int startupResult = testWorld.startUp();

		failures += logTestResult(
			logkeeper,
			"WorldManager startup",
			startupResult == 0
		);

		failures += logTestResult(
			logkeeper,
			"WorldManager starting count",
			testWorld.AllObjectsCount() == 0
		);

		TestObject* obj1 = new TestObject();
		TestObject* obj2 = new TestObject();
		TestObject* obj3 = new TestObject();
		Object* obj4 = new Object();
		Object* obj5 = new Object();
		Object* obj6 = new Object();

		testWorld.insertObject(obj1);
		testWorld.insertObject(obj2);
		testWorld.insertObject(obj3);
		testWorld.insertObject(obj4);
		testWorld.insertObject(obj5);
		testWorld.insertObject(obj6);

		Object* objectPointerList[6] =
		{ obj1, obj2, obj3, obj4, obj5, obj6 };

		Object* objectPointerList2[3] =
		{ obj1, obj2, obj3 };

		failures += logTestResult(
			logkeeper,
			"WorldManager object count",
			testWorld.AllObjectsCount() == 6
		);

		failures += logTestResult(
			logkeeper,
			"WorldManager type count",
			testWorld.objectsOfTypeCount("TestObject") == 3
		);

		ObjectList testList = testWorld.getAllObjects();
		int foundCount = 0;

		for (int i = 0; i < 6; i++) {
			for (int j = 0; j < 6; j++) {
				if (objectPointerList[i] == testList[j]) {
					foundCount++;
				}
			}
		}

		failures += logTestResult(
			logkeeper,
			"WorldManager retrieve all objects",
			foundCount == 6
		);

		testList = testWorld.objectsOfType("TestObject");

		failures += logTestResult(
			logkeeper,
			"WorldManager retrieve object type count",
			testList.getCount() == 3
		);

		foundCount = 0;

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				if (objectPointerList2[i] == testList[j]) {
					foundCount++;
				}
			}
		}

		failures += logTestResult(
			logkeeper,
			"WorldManager retrieve object type",
			foundCount == 3
		);

		for (int i = 0; i < 3; i++) {
			testWorld.markForDelete(objectPointerList2[i]);
		}

		failures += logTestResult(
			logkeeper,
			"WorldManager mark for delete",
			testWorld.AllObjectsCount() == 6
		);

		testWorld.update();

		failures += logTestResult(
			logkeeper,
			"WorldManager remove objects",
			testWorld.AllObjectsCount() == 3
		);

		failures += logTestResult(
			logkeeper,
			"WorldManager invalid type count",
			testWorld.objectsOfTypeCount("nonexistant") == 0
		);

		failures += logTestResult(
			logkeeper,
			"WorldManager remaining object count",
			testWorld.objectsOfTypeCount(df::UNDEFINED_OBJECT) == 3
		);

		ObjectList testList2 =
			testWorld.objectsOfType(df::UNDEFINED_OBJECT);

		testList = testWorld.getAllObjects();
		foundCount = 0;

		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				if (testList2[i] == testList[j]) {
					foundCount++;
				}
			}
		}

		failures += logTestResult(
			logkeeper,
			"WorldManager remaining object match",
			foundCount == 3
		);

		testWorld.shutDown();
		testWorld.startUp();

		failures += logTestResult(
			logkeeper,
			"WorldManager restart",
			testWorld.AllObjectsCount() == 0
		);

		testWorld.shutDown();

		logSuiteResult(logkeeper, "WorldManager", failures);
		logkeeper.shutDown();

		return failures;
	}


	//sfml window tests
	int SFMLConfigTest() {
		LogManager& logman = LogManager::getInstance();
		logman.startUp(true);
		logman.setFlush(true);
		
		// Load font.
		sf::Font font;
		if (font.openFromFile("df-font.ttf") == false) {
			logman.writeLog("Error! Unable to load font \"df-font.ttf\" .");
			logman.shutDown();
			return -1;

		}

		// Setup text to display.
		sf::Text text(font);
		text.setString("Hello, world!"); // Set string to display.
		text.setCharacterSize(32); // Set character size (in pixels).
		text.setFillColor(sf::Color::Green); // Set text color 
		text.setStyle(sf::Text::Bold); // Set text style.
		text.setPosition({ 96 ,134 }); // Set text position (in pixels).
		unsigned int window_horizontal = 1024;
		unsigned int window_vertical = 768;
		// Create window to draw on.
		sf::RenderWindow* p_window = new sf::RenderWindow(sf::VideoMode(sf::Vector2u(window_horizontal, window_vertical)), "SFML Hello World!");
		if (!p_window) {
			logman.writeLog("Error! Unable to allocate RenderWindow.");
			logman.shutDown();
			return -1;

		}

		// Turn off mouse cursor for window.
		p_window->setMouseCursorVisible(false);

		// Synchronize refresh rate with monitor.
		p_window->setVerticalSyncEnabled(true);

		// Repeat forever (as long as window is open).
		Clock timer;
		int timeout = (30 * 33);
		timer.delta();
		while (1) {

			// Clear window and draw text.
			p_window->clear();
			p_window->draw(text);
			p_window->display();

			// Loop until no more events (or window closed).
			while (const std::optional < sf::Event > p_event = p_window->pollEvent()) {
				if (p_event->is < sf::Event::Closed >()) {
					p_window->close();
					delete p_window;
					return 0;

				}


			} // End of while (event).

			if ((timer.split() / 1000 )>= timeout) {
				p_window->close();
				delete p_window;
				return 0;
			}
			
		} // End of while (1).

		Sleep(1000);
	 // End o f main ( ) .
		logman.shutDown();
	}

	int objectListTests2() {
		df::LogManager& logkeeper = df::LogManager::getInstance();
		logkeeper.startUp(true);
		logkeeper.setFlush(true);

		int failures = 0;

		logkeeper.writeLog("suite: ObjectList query functions\n");

		/*
		 * Create test objects with different types and altitudes.
		 */
		TestObject player1;
		TestObject player2;
		TestObject enemy1;
		TestObject neutral1;

		player1.setType("player");
		player2.setType("player");
		enemy1.setType("enemy");
		neutral1.setType("neutral");

		player1.setAltitude(1);
		player2.setAltitude(1);
		enemy1.setAltitude(2);
		neutral1.setAltitude(3);

		ObjectList objects;

		objects.insert(&player1);
		objects.insert(&player2);
		objects.insert(&enemy1);
		objects.insert(&neutral1);

		/*
		 * Happy path: objectsOfTypeCount.
		 */
		int playerCount = objects.objectsOfTypeCount("player");

		failures += logTestResult(
			logkeeper,
			"objectsOfTypeCount returns correct count",
			playerCount == 2
		);

		int enemyCount = objects.objectsOfTypeCount("enemy");

		failures += logTestResult(
			logkeeper,
			"objectsOfTypeCount finds one matching object",
			enemyCount == 1
		);

		/*
		 * Edge case: no objects match the requested type.
		 */
		int missingTypeCount =
			objects.objectsOfTypeCount("does-not-exist");

		failures += logTestResult(
			logkeeper,
			"objectsOfTypeCount returns zero for missing type",
			missingTypeCount == 0
		);

		/*
		 * Edge case: empty type string.
		 */
		int emptyTypeCount =
			objects.objectsOfTypeCount("");

		failures += logTestResult(
			logkeeper,
			"objectsOfTypeCount returns zero for empty type",
			emptyTypeCount == 0
		);

		/*
		 * Happy path: objectsOfType.
		 */
		ObjectList players = objects.objectsOfType("player");

		bool playersCorrect =
			players.getCount() == 2 &&
			players.objectsOfTypeCount("player") == 2;

		failures += logTestResult(
			logkeeper,
			"objectsOfType returns all matching objects",
			playersCorrect
		);

		/*
		 * Verify that objectsOfType returns an empty list when
		 * no objects match.
		 */
		ObjectList missingObjects =
			objects.objectsOfType("does-not-exist");

		failures += logTestResult(
			logkeeper,
			"objectsOfType returns empty list for missing type",
			missingObjects.isEmpty() &&
			missingObjects.getCount() == 0
		);

		/*
		 * Verify that the returned list contains the original pointers.
		 */
		bool returnedPointersCorrect =
			(players[0] == &player1 || players[0] == &player2) &&
			(players[1] == &player1 || players[1] == &player2) &&
			(players[0] != players[1]);

		failures += logTestResult(
			logkeeper,
			"objectsOfType preserves matching object pointers",
			returnedPointersCorrect
		);

		/*
		 * Happy path: objectsOfAltitudeCount.
		 */
		int altitudeOneCount =
			objects.objectsOfAltitudeCount(1);

		failures += logTestResult(
			logkeeper,
			"objectsOfAltitudeCount returns correct count",
			altitudeOneCount == 2
		);

		int altitudeTwoCount =
			objects.objectsOfAltitudeCount(2);

		failures += logTestResult(
			logkeeper,
			"objectsOfAltitudeCount finds one object",
			altitudeTwoCount == 1
		);

		/*
		 * Edge case: valid altitude with no matching objects.
		 */
		int unusedAltitude =
			objects.objectsOfAltitudeCount(4);

		failures += logTestResult(
			logkeeper,
			"objectsOfAltitudeCount returns zero for unused altitude",
			unusedAltitude == 0
		);

		/*
		 * Error path: altitude equal to MAX_ALTITUDE is invalid,
		 * because valid altitudes are less than MAX_ALTITUDE.
		 */
		int invalidAltitudeCount =
			objects.objectsOfAltitudeCount(df::MAX_ALTITUDE+1);

		failures += logTestResult(
			logkeeper,
			"objectsOfAltitudeCount rejects MAX_ALTITUDE + 1",
			invalidAltitudeCount == -1
		);

		/*
		 * Error path: an even larger altitude is invalid.
		 */
		int tooLargeAltitudeCount =
			objects.objectsOfAltitudeCount(
				df::MAX_ALTITUDE + 1
			);

		failures += logTestResult(
			logkeeper,
			"objectsOfAltitudeCount rejects altitude above MAX_ALTITUDE",
			tooLargeAltitudeCount == -1
		);

		/*
		 * Happy path: objectsOfAltitude.
		 */
		ObjectList altitudeOneObjects =
			objects.objectsOfAltitude(1);

		bool altitudeObjectsCorrect =
			altitudeOneObjects.getCount() == 2 &&
			altitudeOneObjects.objectsOfAltitudeCount(1) == 2;

		failures += logTestResult(
			logkeeper,
			"objectsOfAltitude returns all matching objects",
			altitudeObjectsCorrect
		);

		/*
		 * Verify that objectsOfAltitude returns an empty list
		 * when no objects match.
		 */
		ObjectList unusedAltitudeObjects =
			objects.objectsOfAltitude(4);

		failures += logTestResult(
			logkeeper,
			"objectsOfAltitude returns empty list for unused altitude",
			unusedAltitudeObjects.isEmpty() &&
			unusedAltitudeObjects.getCount() == 0
		);

		/*
		 * Error path: invalid altitude should return an empty list.
		 */
		ObjectList invalidAltitudeObjects =
			objects.objectsOfAltitude(df::MAX_ALTITUDE);

		failures += logTestResult(
			logkeeper,
			"objectsOfAltitude returns empty list for invalid altitude",
			invalidAltitudeObjects.isEmpty() &&
			invalidAltitudeObjects.getCount() == 0
		);

		/*
		 * Happy path: objectsByAltitude.
		 */
		std::vector<df::ObjectList> objectsByAltitude =
			objects.objectsByAltitude();

		bool altitudeVectorSizeCorrect =
			objectsByAltitude.size() == df::MAX_ALTITUDE+1;

		failures += logTestResult(
			logkeeper,
			"objectsByAltitude returns MAX_ALTITUDE lists",
			altitudeVectorSizeCorrect
		);

		bool groupedObjectsCorrect =
			objectsByAltitude[1].getCount() == 2 &&
			objectsByAltitude[2].getCount() == 1 &&
			objectsByAltitude[3].getCount() == 1;

		failures += logTestResult(
			logkeeper,
			"objectsByAltitude groups objects correctly",
			groupedObjectsCorrect
		);

		bool unusedGroupsAreEmpty =
			objectsByAltitude[0].isEmpty() &&
			objectsByAltitude[4].isEmpty();

		failures += logTestResult(
			logkeeper,
			"objectsByAltitude leaves unused groups empty",
			unusedGroupsAreEmpty
		);

		/*
		 * Empty-list tests.
		 */
		ObjectList emptyObjects;

		failures += logTestResult(
			logkeeper,
			"objectsOfTypeCount works on empty list",
			emptyObjects.objectsOfTypeCount("player") == 0
		);

		failures += logTestResult(
			logkeeper,
			"objectsOfAltitudeCount works on empty list",
			emptyObjects.objectsOfAltitudeCount(1) == 0
		);

		df::ObjectList emptyTypeResult =
			emptyObjects.objectsOfType("player");

		failures += logTestResult(
			logkeeper,
			"objectsOfType returns empty result for empty list",
			emptyTypeResult.isEmpty()
		);

		df::ObjectList emptyAltitudeResult =
			emptyObjects.objectsOfAltitude(1);

		failures += logTestResult(
			logkeeper,
			"objectsOfAltitude returns empty result for empty list",
			emptyAltitudeResult.isEmpty()
		);

		std::vector<df::ObjectList> emptyGroupedResult =
			emptyObjects.objectsByAltitude();

		bool emptyGroupedCorrect =
			emptyGroupedResult.size() == df::MAX_ALTITUDE+1;

		for (int i = 0; i <= df::MAX_ALTITUDE; i++) {
			if (!emptyGroupedResult[i].isEmpty()) {
				emptyGroupedCorrect = false;
				break;
			}
		}

		failures += logTestResult(
			logkeeper,
			"objectsByAltitude works on empty list",
			emptyGroupedCorrect
		);
		logkeeper.shutDown();
		return failures;
		

	}



	int drawTestChars(DisplayManager& testDisplay) {
		int failures = 0;
		
		failures += testDisplay.drawCh(Vector(10, 10), 't', UNDEFINED_COLOR) != 0;
		
		failures += testDisplay.drawCh(Vector(11, 10), 'e', BLACK) != 0;
		
		failures += testDisplay.drawCh(Vector(12, 10), 's', RED) != 0;
		
		failures += testDisplay.drawCh(Vector(13, 10), 't', GREEN) != 0;
		
		failures += testDisplay.drawCh(Vector(14, 10), ' ', YELLOW) != 0;
		
		failures += testDisplay.drawCh(Vector(15, 10), '-', YELLOW) != 0;
		
		failures += testDisplay.drawCh(Vector(16, 10), 'c', BLUE) != 0;
		
		failures += testDisplay.drawCh(Vector(17, 10), 'h', MAGENTA) != 0;
	
		failures += testDisplay.drawCh(Vector(18, 10), 'a', CYAN) != 0;
		
		failures += testDisplay.drawCh(Vector(19, 10), 'r', WHITE) != 0;
		
		failures += testDisplay.drawCh(Vector(20, 10), 's', CUSTOM_COLOR) != 0;
		
		
		return failures;
	}
	int runDisplayManagerTests() {
		LogManager& logkeeper = LogManager::getInstance();
		DisplayManager& testDisplay = DisplayManager::getInstance();

		int failures = 0;

		logkeeper.startUp(true);
		logkeeper.setFlush(true);

		int startupResult = testDisplay.startUp();

		failures += logTestResult(
			logkeeper,
			"DisplayManager startup",
			startupResult == 0
		);

		if (startupResult == -1) {
			logkeeper.shutDown();

			return logSuiteResult(
				logkeeper,
				"DisplayManager",
				failures
			);
		}

		sf::RenderWindow* testWindow = testDisplay.getWindow();

		failures += logTestResult(
			logkeeper,
			"DisplayManager window getter",
			testWindow != nullptr
		);

		failures += logTestResult(
			logkeeper,
			"DisplayManager horizontal character size",
			testDisplay.getHorizontal() == WINDOW_HORIZONTAL_CHARS_DEFAULT
		);

		failures += logTestResult(
			logkeeper,
			"DisplayManager vertical character size",
			testDisplay.getVertical() == WINDOW_VERTICAL_CHARS_DEFAULT
		);

		failures += logTestResult(
			logkeeper,
			"DisplayManager horizontal pixel size",
			testDisplay.getHorizontalPixels() ==
			WINDOW_HORIZONTAL_PIXELS_DEFAULT
		);

		failures += logTestResult(
			logkeeper,
			"DisplayManager vertical pixel size",
			testDisplay.getVerticalPixels() ==
			WINDOW_VERTICAL_PIXELS_DEFAULT
		);

		/*
		 * Test character drawing.
		 */
		int characterFailures = drawTestChars(testDisplay);

		failures += logTestResult(
			logkeeper,
			"DisplayManager draw characters",
			characterFailures == 0
		);

		failures += logTestResult(
			logkeeper,
			"DisplayManager swap character buffer",
			testDisplay.swapBuffers() == 0
		);

		Sleep(300);

		/*
		 * Test custom foreground color.
		 */
		std::uint32_t customColor =
			RGBAToUInt32ColorConverter(180, 12, 180, 255);

		bool customColorResult =
			testDisplay.setCustomColor(customColor);

		failures += logTestResult(
			logkeeper,
			"DisplayManager set custom color",
			customColorResult
		);

		failures += logTestResult(
			logkeeper,
			"DisplayManager get custom color",
			testDisplay.getCustomColor() == customColor
		);

		characterFailures = drawTestChars(testDisplay);

		failures += logTestResult(
			logkeeper,
			"DisplayManager draw with custom color",
			characterFailures == 0
		);

		failures += logTestResult(
			logkeeper,
			"DisplayManager swap custom color buffer",
			testDisplay.swapBuffers() == 0
		);

		Sleep(300);

		/*
		 * Test background colors.
		 */
		failures += logTestResult(
			logkeeper,
			"DisplayManager set undefined background",
			testDisplay.setBackgroundColor(UNDEFINED_COLOR)
		);

		failures += logTestResult(
			logkeeper,
			"DisplayManager draw with undefined background",
			drawTestChars(testDisplay) == 0
		);

		failures += logTestResult(
			logkeeper,
			"DisplayManager swap undefined background buffer",
			testDisplay.swapBuffers() == 0
		);

		Sleep(300);

		failures += logTestResult(
			logkeeper,
			"DisplayManager set black background",
			testDisplay.setBackgroundColor(BLACK)
		);

		failures += logTestResult(
			logkeeper,
			"DisplayManager draw with black background",
			drawTestChars(testDisplay) == 0
		);

		failures += logTestResult(
			logkeeper,
			"DisplayManager swap black background buffer",
			testDisplay.swapBuffers() == 0
		);

		Sleep(300);

		failures += logTestResult(
			logkeeper,
			"DisplayManager set red background",
			testDisplay.setBackgroundColor(RED)
		);

		failures += logTestResult(
			logkeeper,
			"DisplayManager draw with red background",
			drawTestChars(testDisplay) == 0
		);

		failures += logTestResult(
			logkeeper,
			"DisplayManager swap red background buffer",
			testDisplay.swapBuffers() == 0
		);

		Sleep(300);

		failures += logTestResult(
			logkeeper,
			"DisplayManager set green background",
			testDisplay.setBackgroundColor(GREEN)
		);

		failures += logTestResult(
			logkeeper,
			"DisplayManager draw with green background",
			drawTestChars(testDisplay) == 0
		);

		failures += logTestResult(
			logkeeper,
			"DisplayManager swap green background buffer",
			testDisplay.swapBuffers() == 0
		);

		Sleep(300);

		failures += logTestResult(
			logkeeper,
			"DisplayManager set yellow background",
			testDisplay.setBackgroundColor(YELLOW)
		);

		failures += logTestResult(
			logkeeper,
			"DisplayManager draw with yellow background",
			drawTestChars(testDisplay) == 0
		);

		failures += logTestResult(
			logkeeper,
			"DisplayManager swap yellow background buffer",
			testDisplay.swapBuffers() == 0
		);

		Sleep(300);

		failures += logTestResult(
			logkeeper,
			"DisplayManager set blue background",
			testDisplay.setBackgroundColor(BLUE)
		);

		failures += logTestResult(
			logkeeper,
			"DisplayManager draw with blue background",
			drawTestChars(testDisplay) == 0
		);

		failures += logTestResult(
			logkeeper,
			"DisplayManager swap blue background buffer",
			testDisplay.swapBuffers() == 0
		);

		Sleep(300);

		failures += logTestResult(
			logkeeper,
			"DisplayManager set magenta background",
			testDisplay.setBackgroundColor(MAGENTA)
		);

		failures += logTestResult(
			logkeeper,
			"DisplayManager draw with magenta background",
			drawTestChars(testDisplay) == 0
		);

		failures += logTestResult(
			logkeeper,
			"DisplayManager swap magenta background buffer",
			testDisplay.swapBuffers() == 0
		);

		Sleep(300);

		failures += logTestResult(
			logkeeper,
			"DisplayManager set cyan background",
			testDisplay.setBackgroundColor(CYAN)
		);

		failures += logTestResult(
			logkeeper,
			"DisplayManager draw with cyan background",
			drawTestChars(testDisplay) == 0
		);

		failures += logTestResult(
			logkeeper,
			"DisplayManager swap cyan background buffer",
			testDisplay.swapBuffers() == 0
		);

		Sleep(300);

		failures += logTestResult(
			logkeeper,
			"DisplayManager set white background",
			testDisplay.setBackgroundColor(WHITE)
		);

		failures += logTestResult(
			logkeeper,
			"DisplayManager draw with white background",
			drawTestChars(testDisplay) == 0
		);

		failures += logTestResult(
			logkeeper,
			"DisplayManager swap white background buffer",
			testDisplay.swapBuffers() == 0
		);

		Sleep(300);

		/*
		 * Test custom background color.
		 */
		std::uint32_t customBackgroundColor =
			RGBAToUInt32ColorConverter(180, 12, 180, 255);

		failures += logTestResult(
			logkeeper,
			"DisplayManager set custom background color",
			testDisplay.setCustomBackgroundColor(customBackgroundColor)
		);

		failures += logTestResult(
			logkeeper,
			"DisplayManager set custom background selection",
			testDisplay.setBackgroundColor(CUSTOM_COLOR)
		);

		failures += logTestResult(
			logkeeper,
			"DisplayManager get custom background color",
			testDisplay.getCustomWindowBackgroundColor() ==
			customBackgroundColor
		);

		failures += logTestResult(
			logkeeper,
			"DisplayManager draw with custom background",
			drawTestChars(testDisplay) == 0
		);

		failures += logTestResult(
			logkeeper,
			"DisplayManager swap custom background buffer",
			testDisplay.swapBuffers() == 0
		);

		Sleep(300);

		/*
		 * Test string drawing.
		 */
		failures += logTestResult(
			logkeeper,
			"DisplayManager draw empty string",
			testDisplay.drawString(
				Vector(20, 10),
				"",
				LEFT_JUSTIFIED,
				RED
			) == 0
		);

		failures += logTestResult(
			logkeeper,
			"DisplayManager swap empty string buffer",
			testDisplay.swapBuffers() == 0
		);

		Sleep(400);
		failures += logTestResult(
			logkeeper,
			"DisplayManager draw funny string",
			testDisplay.drawString(
				Vector(10, 10),
				"funny",
				LEFT_JUSTIFIED,
				RED
			) == 0
		);

		failures += logTestResult(
			logkeeper,
			"DisplayManager swap funny string buffer",
			testDisplay.swapBuffers() == 0
		);
		Sleep(400);

		failures += logTestResult(
			logkeeper,
			"DisplayManager draw right-justified string",
			testDisplay.drawString(
				Vector(35, 10),
				"https://youtu.be/19y8YTbvri8",
				RIGHT_JUSTIFIED,
				BLUE
			) == 0
		);

		failures += logTestResult(
			logkeeper,
			"DisplayManager swap right-justified string buffer",
			testDisplay.swapBuffers() == 0
		);

		Sleep(400);

		failures += logTestResult(
			logkeeper,
			"DisplayManager draw centered string",
			testDisplay.drawString(
				Vector(20, 15),
				"https://youtu.be/5SZYz7lZRRI",
				CENTER_JUSTIFIED,
				BLUE
			) == 0
		);

		failures += logTestResult(
			logkeeper,
			"DisplayManager swap centered string buffer",
			testDisplay.swapBuffers() == 0
		);

		Sleep(400);

		failures += logTestResult(
			logkeeper,
			"DisplayManager clear",
			testDisplay.clear() == 0
		);

		testDisplay.shutDown();

		logSuiteResult(
			logkeeper,
			"DisplayManager",
			failures
		);

		logkeeper.shutDown();
		return failures;
	}


	int disperateInputManagerTest() {
		LogManager& logman = LogManager::getInstance();
		logman.startUp(true);
		logman.setFlush(true);
		logman.writeLog("input manager test start!");
		WorldManager& worldman = WorldManager::getInstance();
		worldman.startUp();

		DisplayManager& displayman = DisplayManager::getInstance();
		displayman.startUp();

		InputManager& inputman = InputManager::getInstance();
		inputman.startUp();

		InputTestDisplayObject* test = new InputTestDisplayObject();

		int end = 5 * 30;
		int count = 0;
		timeBeginPeriod(1);
		test->setPosition(Vector(40, 10));
		displayman.setBackgroundColor(BROWN);
		Clock frameTime;
		int sleepyTime = 0;
		while (count < end) {
			frameTime.delta();
			EventStep event;
			
			test->eventHandler(&event);
			inputman.getInput();
			worldman.update();
			worldman.draw();
			displayman.swapBuffers();
			count++;
			sleepyTime = 33 - (frameTime.delta() / 1000);
			if (sleepyTime > 0) {
				Sleep(sleepyTime);
			}
			
		}
		timeEndPeriod(1);
		test = nullptr;

		inputman.shutDown();
		displayman.shutDown();
		worldman.shutDown();
		logman.writeLog("input manager test passed!");
		logman.shutDown();
		return 0;
	}
	// ---------------------------------------------------------
	// Frame tests
	// ---------------------------------------------------------

	int testFrameAttributes() {
		LogManager& lm = LogManager::getInstance();

		int failed = 0;

		Frame frame;

		if (frame.getWidth() != 0) {
			lm.writeLog("FAIL: Frame default width should be 0");
			failed++;
		}
		else {
			lm.writeLog("PASS: Frame default width");
		}

		if (frame.getHeight() != 0) {
			lm.writeLog("FAIL: Frame default height should be 0");
			failed++;
		}
		else {
			lm.writeLog("PASS: Frame default height");
		}

		if (frame.getString() != "") {
			lm.writeLog("FAIL: Frame default string should be empty");
			failed++;
		}
		else {
			lm.writeLog("PASS: Frame default string");
		}

		frame.setWidth(5);

		if (frame.getWidth() != 5) {
			lm.writeLog("FAIL: Frame setWidth/getWidth");
			failed++;
		}
		else {
			lm.writeLog("PASS: Frame setWidth/getWidth");
		}

		frame.setHeight(3);

		if (frame.getHeight() != 3) {
			lm.writeLog("FAIL: Frame setHeight/getHeight");
			failed++;
		}
		else {
			lm.writeLog("PASS: Frame setHeight/getHeight");
		}

		frame.setString("hello");

		if (frame.getString() != "hello") {
			lm.writeLog("FAIL: Frame setString/getString");
			failed++;
		}
		else {
			lm.writeLog("PASS: Frame setString/getString");
		}

		Frame other(4, 2, "123456");

		if (other.getWidth() != 4 ||
			other.getHeight() != 2 ||
			other.getString() != "123456") {
			lm.writeLog("FAIL: Frame parameterized constructor");
			failed++;
		}
		else {
			lm.writeLog("PASS: Frame parameterized constructor");
		}

		return failed;
	}


	int testFrameDraw() {
		LogManager& lm = LogManager::getInstance();
		DisplayManager& dm = DisplayManager::getInstance();

		int failed = 0;

		Frame frame(3, 3, "abcdefghi");

		if (frame.draw(Vector(10, 10), WHITE) != 0) {
			lm.writeLog("FAIL: Frame draw at normal position");
			failed++;
		}
		else {
			lm.writeLog("PASS: Frame draw at normal position");
		}
		dm.swapBuffers();
		Sleep(33 * 30);
		if (frame.draw(Vector(0, 0), WHITE) != 0) {
			lm.writeLog("FAIL: Frame draw at top-left position");
			failed++;
		}
		else {
			lm.writeLog("PASS: Frame draw at top-left position");
		}
		dm.swapBuffers();
		Sleep(33 * 30);
		if (frame.draw(Vector(-5, -5), WHITE) != 0) {
			lm.writeLog("FAIL: Frame draw at negative position");
			failed++;
		}
		else {
			lm.writeLog("PASS: Frame draw at negative position");
		}
		dm.swapBuffers();
		Sleep(33 * 30);
		return failed;
	}


	int testFrameDrawEdgeCases() {
		LogManager& lm = LogManager::getInstance();
		DisplayManager& dm = DisplayManager::getInstance();
		int failed = 0;

		Frame empty;

		if (empty.draw(Vector(10, 10), WHITE) != -1) {
			lm.writeLog("FAIL: Empty Frame draw should return -1");
			failed++;
		}
		else {
			lm.writeLog("PASS: Empty Frame draw returns -1");
		}
		dm.swapBuffers();
		Sleep(33 * 30);
		Frame one(1, 1, "X");

		if (one.draw(Vector(0, 0), WHITE) != 0) {
			lm.writeLog("FAIL: 1x1 Frame draw");
			failed++;
		}
		else {
			lm.writeLog("PASS: 1x1 Frame draw");
		}
		dm.swapBuffers();
		Sleep(33 * 30);
		Frame large(5, 5, "1234567890123456789012345");

		if (large.draw(Vector(0, 0), WHITE) != 0) {
			lm.writeLog("FAIL: Large Frame draw");
			failed++;
		}
		else {
			lm.writeLog("PASS: Large Frame draw");
		}
		dm.swapBuffers();
		Sleep(33 * 30);
		return failed;
	}


	// ---------------------------------------------------------
	// Sprite attribute tests
	// ---------------------------------------------------------

	int testSpriteAttributes() {
		LogManager& lm = LogManager::getInstance();
		DisplayManager& dm = DisplayManager::getInstance();
		int failed = 0;

		Sprite sprite(3);

		sprite.setWidth(10);

		if (sprite.getWidth() != 10) {
			lm.writeLog("FAIL: Sprite setWidth/getWidth");
			failed++;
		}
		else {
			lm.writeLog("PASS: Sprite setWidth/getWidth");
		}

		sprite.setHeight(5);

		if (sprite.getHeight() != 5) {
			lm.writeLog("FAIL: Sprite setHeight/getHeight");
			failed++;
		}
		else {
			lm.writeLog("PASS: Sprite setHeight/getHeight");
		}

		sprite.setColor(RED);

		if (sprite.getColor() != RED) {
			lm.writeLog("FAIL: Sprite setColor/getColor");
			failed++;
		}
		else {
			lm.writeLog("PASS: Sprite setColor/getColor");
		}

		sprite.setLabel("test_sprite");

		if (sprite.getLabel() != "test_sprite") {
			lm.writeLog("FAIL: Sprite setLabel/getLabel");
			failed++;
		}
		else {
			lm.writeLog("PASS: Sprite setLabel/getLabel");
		}

		sprite.setSlowdown(4);

		if (sprite.getSlowdown() != 4) {
			lm.writeLog("FAIL: Sprite setSlowdown/getSlowdown");
			failed++;
		}
		else {
			lm.writeLog("PASS: Sprite setSlowdown/getSlowdown");
		}

		sprite.setTransparency('.');

		if (sprite.getTransparency() != '.') {
			lm.writeLog("FAIL: Sprite setTransparency/getTransparency");
			failed++;
		}
		else {
			lm.writeLog("PASS: Sprite setTransparency/getTransparency");
		}

		sprite.setCustomColor(0x12345678);

		if (sprite.getCustomColor() != 0x12345678) {
			lm.writeLog("FAIL: Sprite setCustomColor/getCustomColor");
			failed++;
		}
		else {
			lm.writeLog("PASS: Sprite setCustomColor/getCustomColor");
		}

		return failed;
	}


	// ---------------------------------------------------------
	// Sprite frame tests
	// ---------------------------------------------------------

	int testSpriteFrames() {
		LogManager& lm = LogManager::getInstance();

		int failed = 0;

		Sprite sprite(3);

		if (sprite.getFrameCount() != 0) {
			lm.writeLog("FAIL: New Sprite should contain 0 frames");
			failed++;
		}
		else {
			lm.writeLog("PASS: New Sprite starts with 0 frames");
		}

		Frame frame1(2, 2, "ABCD");
		Frame frame2(2, 2, "EFGH");
		Frame frame3(2, 2, "IJKL");

		if (sprite.addFrame(frame1) != 0) {
			lm.writeLog("FAIL: Adding first Frame");
			failed++;
		}
		else {
			lm.writeLog("PASS: Adding first Frame");
		}

		if (sprite.addFrame(frame2) != 0) {
			lm.writeLog("FAIL: Adding second Frame");
			failed++;
		}
		else {
			lm.writeLog("PASS: Adding second Frame");
		}

		if (sprite.addFrame(frame3) != 0) {
			lm.writeLog("FAIL: Adding third Frame");
			failed++;
		}
		else {
			lm.writeLog("PASS: Adding third Frame");
		}

		if (sprite.getFrameCount() != 3) {
			lm.writeLog("FAIL: Sprite frame count should be 3");
			failed++;
		}
		else {
			lm.writeLog("PASS: Sprite frame count is 3");
		}

		if (sprite.getFrame(0).getString() != "ABCD") {
			lm.writeLog("FAIL: getFrame(0)");
			failed++;
		}
		else {
			lm.writeLog("PASS: getFrame(0)");
		}

		if (sprite.getFrame(1).getString() != "EFGH") {
			lm.writeLog("FAIL: getFrame(1)");
			failed++;
		}
		else {
			lm.writeLog("PASS: getFrame(1)");
		}

		if (sprite.getFrame(2).getString() != "IJKL") {
			lm.writeLog("FAIL: getFrame(2)");
			failed++;
		}
		else {
			lm.writeLog("PASS: getFrame(2)");
		}

		return failed;
	}


	int testSpriteFrameLimit() {
		LogManager& lm = LogManager::getInstance();

		int failed = 0;

		Sprite sprite(2);

		Frame frame1(1, 1, "A");
		Frame frame2(1, 1, "B");
		Frame frame3(1, 1, "C");

		sprite.addFrame(frame1);
		sprite.addFrame(frame2);

		if (sprite.getFrameCount() != 2) {
			lm.writeLog("FAIL: Sprite should contain exactly 2 frames");
			failed++;
		}
		else {
			lm.writeLog("PASS: Sprite frame limit setup");
		}

		if (sprite.addFrame(frame3) != -1) {
			lm.writeLog("FAIL: Adding Frame beyond maximum should return -1");
			failed++;
		}
		else {
			lm.writeLog("PASS: Adding Frame beyond maximum returns -1");
		}

		if (sprite.getFrameCount() != 2) {
			lm.writeLog("FAIL: Frame count changed after rejected Frame");
			failed++;
		}
		else {
			lm.writeLog("PASS: Frame count unchanged after rejected Frame");
		}

		return failed;
	}


	int testSpriteGetFrameErrors() {
		LogManager& lm = LogManager::getInstance();

		int failed = 0;

		Sprite sprite(2);

		Frame frame(1, 1, "A");
		sprite.addFrame(frame);

		Frame negative = sprite.getFrame(-1);

		if (negative.getString() != "") {
			lm.writeLog("FAIL: getFrame(-1) should return empty Frame");
			failed++;
		}
		else {
			lm.writeLog("PASS: getFrame(-1) returns empty Frame");
		}

		Frame tooHigh = sprite.getFrame(1);

		if (tooHigh.getString() != "") {
			lm.writeLog("FAIL: getFrame(1) should return empty Frame");
			failed++;
		}
		else {
			lm.writeLog("PASS: getFrame(1) returns empty Frame");
		}

		Frame wayTooHigh = sprite.getFrame(100);

		if (wayTooHigh.getString() != "") {
			lm.writeLog("FAIL: getFrame(100) should return empty Frame");
			failed++;
		}
		else {
			lm.writeLog("PASS: getFrame(100) returns empty Frame");
		}

		return failed;
	}


	// ---------------------------------------------------------
	// Sprite draw tests
	// ---------------------------------------------------------

	int testSpriteDraw() {
		LogManager& lm = LogManager::getInstance();
		DisplayManager& dm = DisplayManager::getInstance();
		int failed = 0;

		Sprite sprite(2);

		sprite.setWidth(2);
		sprite.setHeight(2);
		sprite.setColor(WHITE);

		Frame frame1(2, 2, "ABCD");
		Frame frame2(2, 2, "EFGH");

		sprite.addFrame(frame1);
		sprite.addFrame(frame2);

		if (sprite.draw(0, Vector(10, 10)) != 0) {
			lm.writeLog("FAIL: Sprite draw frame 0");
			failed++;
		}
		else {
			lm.writeLog("PASS: Sprite draw frame 0");
		}
		dm.swapBuffers();
		Sleep(33 * 30);
		if (sprite.draw(1, Vector(20, 20)) != 0) {
			lm.writeLog("FAIL: Sprite draw frame 1");
			failed++;
		}
		else {
			lm.writeLog("PASS: Sprite draw frame 1");
		}
		dm.swapBuffers();
		Sleep(33 * 30);
		if (sprite.draw(0, Vector(-10, -10)) != 0) {
			lm.writeLog("FAIL: Sprite draw at negative position");
			failed++;
		}
		else {
			lm.writeLog("PASS: Sprite draw at negative position");
		}
		dm.swapBuffers();
		Sleep(33 * 30);
		return failed;
	}


	int testSpriteDrawErrors() {
		LogManager& lm = LogManager::getInstance();

		int failed = 0;

		Sprite sprite(1);

		Frame frame(2, 2, "ABCD");
		sprite.addFrame(frame);

		if (sprite.draw(-1, Vector(10, 10)) != -1) {
			lm.writeLog("FAIL: Sprite draw(-1) should return -1");
			failed++;
		}
		else {
			lm.writeLog("PASS: Sprite draw(-1) returns -1");
		}

		if (sprite.draw(1, Vector(10, 10)) != -1) {
			lm.writeLog("FAIL: Sprite draw(1) should return -1");
			failed++;
		}
		else {
			lm.writeLog("PASS: Sprite draw(1) returns -1");
		}

		if (sprite.draw(100, Vector(10, 10)) != -1) {
			lm.writeLog("FAIL: Sprite draw(100) should return -1");
			failed++;
		}
		else {
			lm.writeLog("PASS: Sprite draw(100) returns -1");
		}

		return failed;
	}


	// ---------------------------------------------------------
	// ResourceManager tests
	// ---------------------------------------------------------

	int testResourceManager() {
		LogManager& lm = LogManager::getInstance();
		ResourceManager& rm = ResourceManager::getInstance();

		int failed = 0;

		if (rm.startUp() != 0) {
			lm.writeLog("FAIL: ResourceManager startUp");
			return 1;
		}

		lm.writeLog("PASS: ResourceManager startUp");

		if (rm.getSprite("sprite1") != nullptr) {
			lm.writeLog("FAIL: sprite1 should not exist before loading");
			failed++;
		}
		else {
			lm.writeLog("PASS: sprite1 does not exist before loading");
		}

		if (rm.loadSprite("test_sprite_1.txt", "sprite1") != 0) {
			lm.writeLog("FAIL: Loading sprite1");
			failed++;
		}
		else {
			lm.writeLog("PASS: Loading sprite1");
		}

		Sprite* sprite = rm.getSprite("sprite1");

		if (sprite == nullptr) {
			lm.writeLog("FAIL: getSprite(sprite1) returned nullptr");
			failed++;
		}
		else {
			lm.writeLog("PASS: getSprite(sprite1) returned Sprite");

			if (sprite->getLabel() != "sprite1") {
				lm.writeLog("FAIL: Loaded Sprite label");
				failed++;
			}
			else {
				lm.writeLog("PASS: Loaded Sprite label");
			}

			if (sprite->getWidth() != 3) {
				lm.writeLog("FAIL: Loaded Sprite width");
				failed++;
			}
			else {
				lm.writeLog("PASS: Loaded Sprite width");
			}

			if (sprite->getHeight() != 2) {
				lm.writeLog("FAIL: Loaded Sprite height");
				failed++;
			}
			else {
				lm.writeLog("PASS: Loaded Sprite height");
			}

			if (sprite->getFrameCount() != 2) {
				lm.writeLog("FAIL: Loaded Sprite frame count");
				failed++;
			}
			else {
				lm.writeLog("PASS: Loaded Sprite frame count");
			}

			if (sprite->getFrame(0).getString() != "ABCDEF") {
				lm.writeLog("FAIL: Loaded Sprite frame 0");
				failed++;
			}
			else {
				lm.writeLog("PASS: Loaded Sprite frame 0");
			}

			if (sprite->getFrame(1).getString() != "GHIJKL") {
				lm.writeLog("FAIL: Loaded Sprite frame 1");
				failed++;
			}
			else {
				lm.writeLog("PASS: Loaded Sprite frame 1");
			}
		}

		if (rm.unloadSprite("sprite1") != 0) {
			lm.writeLog("FAIL: unloadSprite(sprite1)");
			failed++;
		}
		else {
			lm.writeLog("PASS: unloadSprite(sprite1)");
		}

		if (rm.getSprite("sprite1") != nullptr) {
			lm.writeLog("FAIL: sprite1 still exists after unloading");
			failed++;
		}
		else {
			lm.writeLog("PASS: sprite1 removed after unloading");
		}

		rm.shutDown();

		return failed;
	}


	// ---------------------------------------------------------
	// ResourceManager multiple-sprite tests
	// ---------------------------------------------------------

	int testMultipleSprites() {
		LogManager& lm = LogManager::getInstance();
		ResourceManager& rm = ResourceManager::getInstance();

		int failed = 0;

		if (rm.startUp() != 0) {
			lm.writeLog("FAIL: ResourceManager startUp for multiple sprites");
			return 1;
		}

		if (rm.loadSprite("test_sprite_1.txt", "sprite1") != 0) {
			lm.writeLog("FAIL: Loading sprite1");
			failed++;
		}
		else {
			lm.writeLog("PASS: Loading sprite1");
		}

		if (rm.loadSprite("test_sprite_2.txt", "sprite2") != 0) {
			lm.writeLog("FAIL: Loading sprite2");
			failed++;
		}
		else {
			lm.writeLog("PASS: Loading sprite2");
		}

		if (rm.loadSprite("test_sprite_3.txt", "sprite3") != 0) {
			lm.writeLog("FAIL: Loading sprite3");
			failed++;
		}
		else {
			lm.writeLog("PASS: Loading sprite3");
		}

		if (rm.loadSprite("test_sprite_4.txt", "sprite4") != 0) {
			lm.writeLog("FAIL: Loading sprite4");
			failed++;
		}
		else {
			lm.writeLog("PASS: Loading sprite4");
		}
		Sprite* sprite1 = rm.getSprite("sprite1");
		Sprite* sprite2 = rm.getSprite("sprite2");
		Sprite* sprite3 = rm.getSprite("sprite3");
		Sprite* sprite4 = rm.getSprite("sprite4");

		if (sprite1 == nullptr) {
			lm.writeLog("FAIL: getSprite(sprite1)");
			failed++;
		}
		else {
			lm.writeLog("PASS: getSprite(sprite1)");
		}

		if (sprite2 == nullptr) {
			lm.writeLog("FAIL: getSprite(sprite2)");
			failed++;
		}
		else {
			lm.writeLog("PASS: getSprite(sprite2)");
		}

		if (sprite3 == nullptr) {
			lm.writeLog("FAIL: getSprite(sprite3)");
			failed++;
		}
		else {
			lm.writeLog("PASS: getSprite(sprite3)");
		}

		if (sprite4 == nullptr) {
			lm.writeLog("FAIL: getSprite(sprite4)");
			failed++;
		}
		else {
			lm.writeLog("PASS: getSprite(sprite4)");
		}


		if (rm.getSprite("does_not_exist") != nullptr) {
			lm.writeLog("FAIL: getSprite for nonexistent label");
			failed++;
		}
		else {
			lm.writeLog("PASS: getSprite for nonexistent label");
		}

		if (rm.unloadSprite("sprite2") != 0) {
			lm.writeLog("FAIL: unloadSprite(sprite2)");
			failed++;
		}
		else {
			lm.writeLog("PASS: unloadSprite(sprite2)");
		}

		if (rm.getSprite("sprite1") == nullptr) {
			lm.writeLog("FAIL: sprite1 disappeared after unloading sprite2");
			failed++;
		}
		else {
			lm.writeLog("PASS: sprite1 remains after unloading sprite2");
		}

		if (rm.getSprite("sprite2") != nullptr) {
			lm.writeLog("FAIL: sprite2 still exists after unloading");
			failed++;
		}
		else {
			lm.writeLog("PASS: sprite2 removed after unloading");
		}

		if (rm.getSprite("sprite3") == nullptr) {
			lm.writeLog("FAIL: sprite3 disappeared after unloading sprite2");
			failed++;
		}
		else {
			lm.writeLog("PASS: sprite3 remains after unloading sprite2");
		}

		


		if (rm.getSprite("sprite4") == nullptr) {
			lm.writeLog("FAIL: sprite4 disappeared after unloading sprite3");
			failed++;
		}
		else {
			lm.writeLog("PASS: sprite4 remains after unloading sprite3");
		}

		if (rm.unloadSprite("does_not_exist") != -1) {
			lm.writeLog("FAIL: unloadSprite for nonexistent label");
			failed++;
		}
		else {
			lm.writeLog("PASS: unloadSprite for nonexistent label");
		}

		rm.shutDown();

		return failed;
	}


	// ---------------------------------------------------------
	// ResourceManager error tests
	// ---------------------------------------------------------

	int testResourceManagerErrors() {
		LogManager& lm = LogManager::getInstance();
		ResourceManager& rm = ResourceManager::getInstance();

		int failed = 0;

		if (rm.startUp() != 0) {
			lm.writeLog("FAIL: ResourceManager startUp for error tests");
			return 1;
		}

		if (rm.loadSprite("this_file_does_not_exist.txt", "bad_sprite") != -1) {
			lm.writeLog("FAIL: Loading nonexistent sprite file should return -1");
			failed++;
		}
		else {
			lm.writeLog("PASS: Loading nonexistent sprite file returns -1");
		}

		if (rm.getSprite("bad_sprite") != nullptr) {
			lm.writeLog("FAIL: Failed sprite should not exist");
			failed++;
		}
		else {
			lm.writeLog("PASS: Failed sprite does not exist");
		}

		if (rm.loadSprite("test_sprite_1.txt", "sprite1") != 0) {
			lm.writeLog("FAIL: Loading valid sprite");
			failed++;
		}
		else {
			lm.writeLog("PASS: Loading valid sprite");
		}

		if (rm.loadSprite("test_sprite_1.txt", "sprite1") != -1) {
			lm.writeLog("FAIL: Duplicate sprite label should return -1");
			failed++;
		}
		else {
			lm.writeLog("PASS: Duplicate sprite label returns -1");
		}

		if (rm.getSprite("sprite1") == nullptr) {
			lm.writeLog("FAIL: Original sprite disappeared after duplicate load");
			failed++;
		}
		else {
			lm.writeLog("PASS: Original sprite remains after duplicate load");
		}

		rm.shutDown();

		return failed;
	}


	int testResourceManagerUnhappyPaths() {
		LogManager& lm = LogManager::getInstance();
		ResourceManager& rm = ResourceManager::getInstance();

		int failed = 0;

		if (rm.startUp() != 0) {
			lm.writeLog("FAIL: ResourceManager startUp for unhappy path tests");
			return 1;
		}

		// ---------------------------------------------------------
		// Fatal error: missing frame data
		// ---------------------------------------------------------

		if (rm.loadSprite("bad_sprite_missing_data.txt", "missing_data") != -1) {
			lm.writeLog("FAIL: Missing data sprite should not load");
			failed++;
		}
		else {
			lm.writeLog("PASS: Missing data sprite rejected");
		}

		if (rm.getSprite("missing_data") != nullptr) {
			lm.writeLog("FAIL: Missing data sprite exists after failed load");
			failed++;
		}
		else {
			lm.writeLog("PASS: Missing data sprite was not added");
		}


		// ---------------------------------------------------------
		// Fatal error: frame line too short
		// ---------------------------------------------------------

		if (rm.loadSprite("bad_sprite_short_line.txt", "short_line") != -1) {
			lm.writeLog("FAIL: Short-line sprite should not load");
			failed++;
		}
		else {
			lm.writeLog("PASS: Short-line sprite rejected");
		}

		if (rm.getSprite("short_line") != nullptr) {
			lm.writeLog("FAIL: Short-line sprite exists after failed load");
			failed++;
		}
		else {
			lm.writeLog("PASS: Short-line sprite was not added");
		}


		// ---------------------------------------------------------
		// Fatal error: invalid frame count
		// ---------------------------------------------------------

		if (rm.loadSprite("bad_sprite_invalid_frames.txt", "invalid_frames") != -1) {
			lm.writeLog("FAIL: Invalid frame count should be rejected");
			failed++;
		}
		else {
			lm.writeLog("PASS: Invalid frame count rejected");
		}

		if (rm.getSprite("invalid_frames") != nullptr) {
			lm.writeLog("FAIL: Invalid frame count sprite exists");
			failed++;
		}
		else {
			lm.writeLog("PASS: Invalid frame count sprite was not added");
		}


		// ---------------------------------------------------------
		// Fatal error: invalid width
		// ---------------------------------------------------------

		if (rm.loadSprite("bad_sprite_invalid_width.txt", "invalid_width") != -1) {
			lm.writeLog("FAIL: Invalid width should be rejected");
			failed++;
		}
		else {
			lm.writeLog("PASS: Invalid width rejected");
		}

		if (rm.getSprite("invalid_width") != nullptr) {
			lm.writeLog("FAIL: Invalid width sprite exists");
			failed++;
		}
		else {
			lm.writeLog("PASS: Invalid width sprite was not added");
		}


		// ---------------------------------------------------------
		// Fatal error: invalid height
		// ---------------------------------------------------------

		if (rm.loadSprite("bad_sprite_invalid_height.txt", "invalid_height") != -1) {
			lm.writeLog("FAIL: Invalid height should be rejected");
			failed++;
		}
		else {
			lm.writeLog("PASS: Invalid height rejected");
		}

		if (rm.getSprite("invalid_height") != nullptr) {
			lm.writeLog("FAIL: Invalid height sprite exists");
			failed++;
		}
		else {
			lm.writeLog("PASS: Invalid height sprite was not added");
		}


		// ---------------------------------------------------------
		// Fatal error: invalid slowdown
		// ---------------------------------------------------------

		if (rm.loadSprite("bad_sprite_invalid_slowdown.txt", "invalid_slowdown") != -1) {
			lm.writeLog("FAIL: Invalid slowdown should be rejected");
			failed++;
		}
		else {
			lm.writeLog("PASS: Invalid slowdown rejected");
		}

		if (rm.getSprite("invalid_slowdown") != nullptr) {
			lm.writeLog("FAIL: Invalid slowdown sprite exists");
			failed++;
		}
		else {
			lm.writeLog("PASS: Invalid slowdown sprite was not added");
		}


		// ---------------------------------------------------------
		// Fatal error: invalid custom color
		// ---------------------------------------------------------

		if (rm.loadSprite("bad_sprite_invalid_custom_color.txt",
			"invalid_custom_color") != -1) {
			lm.writeLog("FAIL: Invalid custom color should be rejected");
			failed++;
		}
		else {
			lm.writeLog("PASS: Invalid custom color rejected");
		}

		if (rm.getSprite("invalid_custom_color") != nullptr) {
			lm.writeLog("FAIL: Invalid custom color sprite exists");
			failed++;
		}
		else {
			lm.writeLog("PASS: Invalid custom color sprite was not added");
		}


		if (rm.loadSprite("missing_custom_color.txt",
			"invalid_custom_color") != -1) {
			lm.writeLog("FAIL: Invalid custom color missing value should be rejected");
			failed++;
		}
		else {
			lm.writeLog("PASS: Invalid custom color missing value rejected");
		}

		if (rm.getSprite("invalid_custom_color") != nullptr) {
			lm.writeLog("FAIL: Invalid custom color missing value sprite exists");
			failed++;
		}
		else {
			lm.writeLog("PASS: Invalid custom color missing value sprite was not added");
		}

		if (rm.loadSprite("unrecoverable_custom_color_hex_0.txt",
			"invalid_custom_color") != -1) {
			lm.writeLog("FAIL: Invalid custom color hex 0 should be rejected");
			failed++;
		}
		else {
			lm.writeLog("PASS: Invalid custom color hex 0 rejected");
		}

		if (rm.getSprite("invalid_custom_color") != nullptr) {
			lm.writeLog("FAIL: Invalid custom color hex 0 sprite exists");
			failed++;
		}
		else {
			lm.writeLog("PASS: Invalid custom color hex 0 sprite was not added");
		}

		if (rm.loadSprite("unrecoverable_custom_color_hex_1.txt",
			"invalid_custom_color") != -1) {
			lm.writeLog("FAIL: Invalid custom hex 1 color should be rejected");
			failed++;
		}
		else {
			lm.writeLog("PASS: Invalid custom hex color 1 rejected");
		}

		if (rm.getSprite("invalid_custom_color") != nullptr) {
			lm.writeLog("FAIL: Invalid custom color hex 1 sprite exists");
			failed++;
		}
		else {
			lm.writeLog("PASS: Invalid custom color hex 1 sprite was not added");
		}

		// ---------------------------------------------------------
		// Recoverable error: invalid color
		// ---------------------------------------------------------

		if (rm.loadSprite("recoverable_bad_color.txt",
			"recoverable_color") != 0) {
			lm.writeLog("FAIL: Invalid color should be a recoverable error");
			failed++;
		}
		else {
			lm.writeLog("PASS: Invalid color was recovered from");
		}

		Sprite* colorSprite = rm.getSprite("recoverable_color");

		if (colorSprite == nullptr) {
			lm.writeLog("FAIL: Recoverable color sprite was not loaded");
			failed++;
		}
		else {
			lm.writeLog("PASS: Recoverable color sprite was loaded");

			if (colorSprite->getFrameCount() != 1) {
				lm.writeLog("FAIL: Recoverable color sprite has wrong frame count");
				failed++;
			}
			else {
				lm.writeLog("PASS: Recoverable color sprite frame count");
			}
		}


		// ---------------------------------------------------------
		// Recoverable error: lines too long
		// ---------------------------------------------------------

		if (rm.loadSprite("recoverable_long_line.txt",
			"recoverable_long") != 0) {
			lm.writeLog("FAIL: Long frame lines should be recoverable");
			failed++;
		}
		else {
			lm.writeLog("PASS: Long frame lines were recovered from");
		}

		Sprite* longSprite = rm.getSprite("recoverable_long");

		if (longSprite == nullptr) {
			lm.writeLog("FAIL: Recoverable long-line sprite was not loaded");
			failed++;
		}
		else {
			lm.writeLog("PASS: Recoverable long-line sprite was loaded");

			if (longSprite->getFrameCount() != 1) {
				lm.writeLog("FAIL: Recoverable long-line frame count");
				failed++;
			}
			else {
				lm.writeLog("PASS: Recoverable long-line frame count");
			}

			if (longSprite->getFrame(0).getString() != "ABCEFG") {
				lm.writeLog("FAIL: Long-line sprite frame contents");
				failed++;
			}
			else {
				lm.writeLog("PASS: Long-line sprite frame contents");
			}
		}


		// ---------------------------------------------------------
		// Recoverable error: extra lines
		// ---------------------------------------------------------

		if (rm.loadSprite("recoverable_extra_lines.txt",
			"recoverable_extra") != 0) {
			lm.writeLog("FAIL: Extra lines should be a recoverable error");
			failed++;
		}
		else {
			lm.writeLog("PASS: Extra lines were recovered from");
		}

		Sprite* extraSprite = rm.getSprite("recoverable_extra");

		if (extraSprite == nullptr) {
			lm.writeLog("FAIL: Recoverable extra-line sprite was not loaded");
			failed++;
		}
		else {
			lm.writeLog("PASS: Recoverable extra-line sprite was loaded");

			if (extraSprite->getFrameCount() != 1) {
				lm.writeLog("FAIL: Recoverable extra-line frame count");
				failed++;
			}
			else {
				lm.writeLog("PASS: Recoverable extra-line frame count");
			}
		}


		// ---------------------------------------------------------
		// Clean up
		// ---------------------------------------------------------

		rm.shutDown();

		return failed;
	}






	int testMusicAttributes() {
		LogManager& lm = LogManager::getInstance();

		int failed = 0;

		Music music;

		// Happy path - default values
		if (music.getLabel() != "undefinedMusic") {
			lm.writeLog("FAIL: Music default label should be undefinedMusic");
			failed++;
		}
		else {
			lm.writeLog("PASS: Music default label");
		}

		if (music.getMusic() != nullptr) {
			lm.writeLog("FAIL: Music default pointer should be nullptr");
			failed++;
		}
		else {
			lm.writeLog("PASS: Music default pointer");
		}

		// Happy path - set/get label
		music.setLabel("testMusic");

		if (music.getLabel() != "testMusic") {
			lm.writeLog("FAIL: Music setLabel/getLabel");
			failed++;
		}
		else {
			lm.writeLog("PASS: Music setLabel/getLabel");
		}

		// Edge case - empty label
		music.setLabel("");

		if (music.getLabel() != "") {
			lm.writeLog("FAIL: Music empty label");
			failed++;
		}
		else {
			lm.writeLog("PASS: Music empty label");
		}

		// Edge case - play/stop/pause with no music loaded
		music.play();

		if (music.getMusic() != nullptr) {
			lm.writeLog("FAIL: Music play with no music should leave pointer nullptr");
			failed++;
		}
		else {
			lm.writeLog("PASS: Music play with no music");
		}

		music.stop();

		if (music.getMusic() != nullptr) {
			lm.writeLog("FAIL: Music stop with no music should leave pointer nullptr");
			failed++;
		}
		else {
			lm.writeLog("PASS: Music stop with no music");
		}

		music.pause();

		if (music.getMusic() != nullptr) {
			lm.writeLog("FAIL: Music pause with no music should leave pointer nullptr");
			failed++;
		}
		else {
			lm.writeLog("PASS: Music pause with no music");
		}

		// Happy path - load valid music
		if (music.loadMusic("sound_effect-000.wav") != 0) {
			lm.writeLog("FAIL: Music loadMusic valid file");
			failed++;
		}
		else {
			lm.writeLog("PASS: Music loadMusic valid file");
		}

		if (music.getMusic() == nullptr) {
			lm.writeLog("FAIL: Music pointer should not be nullptr after loading");
			failed++;
		}
		else {
			lm.writeLog("PASS: Music pointer after loading");
		}

		// Happy path - play loaded music
		music.play(false);
		lm.writeLog("PASS: Music play loaded music");

		music.pause();
		lm.writeLog("PASS: Music pause loaded music");

		music.stop();
		lm.writeLog("PASS: Music stop loaded music");

		// Unhappy path - invalid file
		if (music.loadMusic("this_file_does_not_exist.wav") != -1) {
			lm.writeLog("FAIL: Music loadMusic invalid file should return -1");
			failed++;
		}
		else {
			lm.writeLog("PASS: Music loadMusic invalid file");
		}

		return failed;
	}


	int testSoundAttributes() {
		LogManager& lm = LogManager::getInstance();

		int failed = 0;

		Sound sound;

		// Happy path - default values
		if (sound.getLabel() != "UndefinedSound") {
			lm.writeLog("FAIL: Sound default label should be UndefinedSound");
			failed++;
		}
		else {
			lm.writeLog("PASS: Sound default label");
		}

		// Happy path - set/get label
		sound.setLabel("testSound");

		if (sound.getLabel() != "testSound") {
			lm.writeLog("FAIL: Sound setLabel/getLabel");
			failed++;
		}
		else {
			lm.writeLog("PASS: Sound setLabel/getLabel");
		}

		// Edge case - empty label
		sound.setLabel("");

		if (sound.getLabel() != "") {
			lm.writeLog("FAIL: Sound empty label");
			failed++;
		}
		else {
			lm.writeLog("PASS: Sound empty label");
		}

		// Edge case - play/stop/pause with no sound loaded
		Sound emptySound;

		emptySound.play(false);
		lm.writeLog("PASS: Sound play with no sound");

		emptySound.stop();
		lm.writeLog("PASS: Sound stop with no sound");

		emptySound.pause();
		lm.writeLog("PASS: Sound pause with no sound");

		// Happy path - load valid sound
		if (sound.loadSound("sound_effect-001.wav") != 0) {
			lm.writeLog("FAIL: Sound loadSound valid file");
			failed++;
		}
		else {
			lm.writeLog("PASS: Sound loadSound valid file");
		}

		// Happy path - play loaded sound
		sound.play(false);
		lm.writeLog("PASS: Sound play loaded sound");

		sound.pause();
		lm.writeLog("PASS: Sound pause loaded sound");

		sound.stop();
		lm.writeLog("PASS: Sound stop loaded sound");

		// Unhappy path - invalid file
		if (sound.loadSound("this_file_does_not_exist.wav") != -1) {
			lm.writeLog("FAIL: Sound loadSound invalid file should return -1");
			failed++;
		}
		else {
			lm.writeLog("PASS: Sound loadSound invalid file");
		}

		return failed;
	}


	int testResourceManagerSound() {
		LogManager& lm = LogManager::getInstance();

		int failed = 0;

		ResourceManager& rm = ResourceManager::getInstance();


		rm.startUp();

		// Happy path - load sound
		if (rm.loadSound("sound_effect-002.wav", "testSound") != 0) {
			lm.writeLog("FAIL: ResourceManager loadSound valid file");
			failed++;
		}
		else {
			lm.writeLog("PASS: ResourceManager loadSound valid file");
		}

		// Happy path - get sound
		if (rm.getSound("testSound") == nullptr) {
			lm.writeLog("FAIL: ResourceManager getSound loaded sound");
			failed++;
		}
		else {
			lm.writeLog("PASS: ResourceManager getSound loaded sound");
		}

		// Happy path - verify label
		if (rm.getSound("testSound") != nullptr &&
			rm.getSound("testSound")->getLabel() != "testSound") {
			lm.writeLog("FAIL: ResourceManager sound label");
			failed++;
		}
		else {
			lm.writeLog("PASS: ResourceManager sound label");
		}

		// Happy path - unload sound
		if (rm.unloadSound("testSound") != 0) {
			lm.writeLog("FAIL: ResourceManager unloadSound loaded sound");
			failed++;
		}
		else {
			lm.writeLog("PASS: ResourceManager unloadSound loaded sound");
		}

		// Edge case - get after unload
		if (rm.getSound("testSound") != nullptr) {
			lm.writeLog("FAIL: ResourceManager getSound after unload should return nullptr");
			failed++;
		}
		else {
			lm.writeLog("PASS: ResourceManager getSound after unload");
		}

		// Unhappy path - invalid file
		if (rm.loadSound("this_file_does_not_exist.wav", "badSound") != -1) {
			lm.writeLog("FAIL: ResourceManager loadSound invalid file should return -1");
			failed++;
		}
		else {
			lm.writeLog("PASS: ResourceManager loadSound invalid file");
		}

		// Unhappy path - missing sound
		if (rm.getSound("missingSound") != nullptr) {
			lm.writeLog("FAIL: ResourceManager getSound missing label should return nullptr");
			failed++;
		}
		else {
			lm.writeLog("PASS: ResourceManager getSound missing label");
		}

		// Unhappy path - unload missing sound
		if (rm.unloadSound("missingSound") != -1) {
			lm.writeLog("FAIL: ResourceManager unloadSound missing label should return -1");
			failed++;
		}
		else {
			lm.writeLog("PASS: ResourceManager unloadSound missing label");
		}

		// Edge case - empty label
		if (rm.loadSound("sound_effect-003.wav", "") != 0) {
			lm.writeLog("FAIL: ResourceManager loadSound empty label");
			failed++;
		}
		else {
			lm.writeLog("PASS: ResourceManager loadSound empty label");
		}

		if (rm.getSound("") == nullptr) {
			lm.writeLog("FAIL: ResourceManager getSound empty label");
			failed++;
		}
		else {
			lm.writeLog("PASS: ResourceManager getSound empty label");
		}

		rm.unloadSound("");

		rm.shutDown();

		return failed;
	}


	int testResourceManagerMusic() {
		LogManager& lm = LogManager::getInstance();

		int failed = 0;

		ResourceManager& rm = ResourceManager::getInstance();


		rm.startUp();

		// Happy path - load music
		if (rm.loadMusic("sound_effect-004.wav", "testMusic") != 0) {
			lm.writeLog("FAIL: ResourceManager loadMusic valid file");
			failed++;
		}
		else {
			lm.writeLog("PASS: ResourceManager loadMusic valid file");
		}

		// Happy path - get music
		if (rm.getMusic("testMusic") == nullptr) {
			lm.writeLog("FAIL: ResourceManager getMusic loaded music");
			failed++;
		}
		else {
			lm.writeLog("PASS: ResourceManager getMusic loaded music");
		}

		// Happy path - verify label
		if (rm.getMusic("testMusic") != nullptr &&
			rm.getMusic("testMusic")->getLabel() != "testMusic") {
			lm.writeLog("FAIL: ResourceManager music label");
			failed++;
		}
		else {
			lm.writeLog("PASS: ResourceManager music label");
		}

		// Happy path - unload music
		if (rm.unloadMusic("testMusic") != 0) {
			lm.writeLog("FAIL: ResourceManager unloadMusic loaded music");
			failed++;
		}
		else {
			lm.writeLog("PASS: ResourceManager unloadMusic loaded music");
		}

		// Edge case - get after unload
		if (rm.getMusic("testMusic") != nullptr) {
			lm.writeLog("FAIL: ResourceManager getMusic after unload should return nullptr");
			failed++;
		}
		else {
			lm.writeLog("PASS: ResourceManager getMusic after unload");
		}

		// Unhappy path - invalid file
		if (rm.loadMusic("this_file_does_not_exist.wav", "badMusic") != -1) {
			lm.writeLog("FAIL: ResourceManager loadMusic invalid file should return -1");
			failed++;
		}
		else {
			lm.writeLog("PASS: ResourceManager loadMusic invalid file");
		}

		// Unhappy path - missing music
		if (rm.getMusic("missingMusic") != nullptr) {
			lm.writeLog("FAIL: ResourceManager getMusic missing label should return nullptr");
			failed++;
		}
		else {
			lm.writeLog("PASS: ResourceManager getMusic missing label");
		}

		// Unhappy path - unload missing music
		if (rm.unloadMusic("missingMusic") != -1) {
			lm.writeLog("FAIL: ResourceManager unloadMusic missing label should return -1");
			failed++;
		}
		else {
			lm.writeLog("PASS: ResourceManager unloadMusic missing label");
		}

		// Edge case - empty label
		if (rm.loadMusic("sound_effect-005.wav", "") != 0) {
			lm.writeLog("FAIL: ResourceManager loadMusic empty label");
			failed++;
		}
		else {
			lm.writeLog("PASS: ResourceManager loadMusic empty label");
		}

		if (rm.getMusic("") == nullptr) {
			lm.writeLog("FAIL: ResourceManager getMusic empty label");
			failed++;
		}
		else {
			lm.writeLog("PASS: ResourceManager getMusic empty label");
		}

		rm.unloadMusic("");

		rm.shutDown();

		return failed;
	}


	int testResourceManagerSoundCapacity() {
		LogManager& lm = LogManager::getInstance();

		int failed = 0;

		ResourceManager& rm = ResourceManager::getInstance();


		rm.startUp();

		// Happy path - load sounds up to the maximum.
		for (int i = 0; i < MAX_SOUNDS; i++) {
			int fileNumber = i % 13;

			std::string filename;

			if (fileNumber < 10) {
				filename = "sound_effect-00" + std::to_string(fileNumber) + ".wav";
			}
			else {
				filename = "sound_effect-0" + std::to_string(fileNumber) + ".wav";
			}

			std::string label = "sound" + std::to_string(i);

			if (rm.loadSound(filename, label) != 0) {
				lm.writeLog("FAIL: ResourceManager loadSound before MAX_SOUNDS");
				failed++;
				break;
			}
		}

		// Edge case - load one more than maximum.
		if (rm.loadSound("sound_effect-000.wav", "tooManySounds") != -1) {
			lm.writeLog("FAIL: ResourceManager loadSound over MAX_SOUNDS should return -1");
			failed++;
		}
		else {
			lm.writeLog("PASS: ResourceManager loadSound over MAX_SOUNDS");
		}

		rm.shutDown();

		return failed;
	}


	int testResourceManagerMusicCapacity() {
		LogManager& lm = LogManager::getInstance();

		int failed = 0;

		ResourceManager& rm = ResourceManager::getInstance();


		rm.startUp();

		// Happy path - load music up to the maximum.
		for (int i = 0; i < MAX_MUSICS; i++) {
			int fileNumber = i % 13;

			std::string filename;

			if (fileNumber < 10) {
				filename = "sound_effect-00" + std::to_string(fileNumber) + ".wav";
			}
			else {
				filename = "sound_effect-0" + std::to_string(fileNumber) + ".wav";
			}
			std::string label = "music" + std::to_string(i);

			if (rm.loadMusic(filename, label) != 0) {
				lm.writeLog("FAIL: ResourceManager loadMusic before MAX_MUSICS");
				failed++;
				break;
			}
		}

		// Edge case - load one more than maximum.
		if (rm.loadMusic("sound_effect-000.wav", "tooManyMusic") != -1) {
			lm.writeLog("FAIL: ResourceManager loadMusic over MAX_MUSICS should return -1");
			failed++;
		}
		else {
			lm.writeLog("PASS: ResourceManager loadMusic over MAX_MUSICS");
		}

		rm.shutDown();

		return failed;
	}











	// ---------------------------------------------------------
	// Run all tests
	// ---------------------------------------------------------

	int projectect2cTests1() {
		LogManager& lm = LogManager::getInstance();
		DisplayManager& dm = DisplayManager::getInstance();
		
		if (lm.startUp(true) != 0) {
			std::cout << "Could not start LogManager." << std::endl;
			return 1;
		}
		if (dm.startUp() != 0) {
			std::cout << "Could not start LogManager." << std::endl;
			return 1;
		}

		lm.writeLog("========================================\nBeginning Frame/Sprite tests\========================================");

		timeBeginPeriod(1);
		int passedTests = 0;
		int failedTests = 0;
		int test = 0;
		test = testFrameAttributes();
		if (test <= 0) {
			lm.writeLog("PASS: testFrameAttributes");
			passedTests++;
		}
		else {
			lm.writeLog("FAIL: testFrameAttributes");
			failedTests += test;
		}

		test = testFrameDraw();
		if (test <= 0) {
			lm.writeLog("PASS: testFrameDraw");
			passedTests++;
		}
		else {
			lm.writeLog("FAIL: testFrameDraw");
			failedTests += test;
		}

		test = testFrameDrawEdgeCases();
		if (test <= 0) {
			lm.writeLog("PASS: testFrameDrawEdgeCases");
			passedTests++;
		}
		else {
			lm.writeLog("FAIL: testFrameDrawEdgeCases");
			failedTests += test;
		}

		test = testSpriteAttributes();

		if (test <= 0) {
			lm.writeLog("PASS: testSpriteAttributes");
			passedTests++;
		}
		else {
			lm.writeLog("FAIL: testSpriteAttributes");
			failedTests += test;
		}


		test = testSpriteFrames();
		if (test <= 0) {
			lm.writeLog("PASS: testSpriteFrames");
			passedTests++;
		}
		else {
			lm.writeLog("FAIL: testSpriteFrames");
			failedTests++;
		}

		test = testSpriteFrameLimit();


		if (test <= 0) {
			lm.writeLog("PASS: testSpriteFrameLimit");
			passedTests++;
		}
		else {
			lm.writeLog("FAIL: testSpriteFrameLimit");
			failedTests += test;
		}

		test = testSpriteGetFrameErrors();

		if (test <= 0) {
			lm.writeLog("PASS: testSpriteGetFrameErrors");
			passedTests++;
		}
		else {
			lm.writeLog("FAIL: testSpriteGetFrameErrors");
			failedTests += test;
		}

		test = testSpriteDraw();

		if (test <= 0) {
			lm.writeLog("PASS: testSpriteDraw");
			passedTests++;
		}
		else {
			lm.writeLog("FAIL: testSpriteDraw");
			failedTests += test;
		}

		test = testSpriteDrawErrors();

		if (test <= 0) {
			lm.writeLog("PASS: testSpriteDrawErrors");
			passedTests++;
		}
		else {
			lm.writeLog("FAIL: testSpriteDrawErrors");
			failedTests += test;
		}
		test = testResourceManager();
		if (test <= 0) {
			lm.writeLog("PASS: testResourceManager");
			passedTests++;
		}
		else {
			lm.writeLog("FAIL: testResourceManager");
			failedTests += test;
		}

		test = testMultipleSprites();

		if (test <= 0) {
			lm.writeLog("PASS: testMultipleSprites");
			passedTests++;
		}
		else {
			lm.writeLog("FAIL: testMultipleSprites");
			failedTests += test;
		}


		test = testResourceManagerErrors();

		if (test <= 0) {
			lm.writeLog("PASS: testResourceManagerErrors");
			passedTests++;
		}
		else {
			lm.writeLog("FAIL: testResourceManagerErrors");
			failedTests += test;
		}

		test = testResourceManagerUnhappyPaths();

		if (test <= 0) {
			lm.writeLog("PASS: testResourceManagerUnhappyPaths");
			passedTests++;
		}
		else {
			lm.writeLog("FAIL: testResourceManagerUnhappyPaths");
			failedTests += test;
		}


		

		
		test = testResourceManagerMusicCapacity();

		if (test <= 0) {
			lm.writeLog("PASS: testResourceManagerMusicCapacity");
			passedTests++;
		}
		else {
			lm.writeLog("FAIL: testResourceManagerMusicCapacity");
			failedTests += test;
		}
		


		test = testResourceManagerSoundCapacity();

		if (test <= 0) {
			lm.writeLog("PASS: testResourceManagerSoundCapacity");
			passedTests++;
		}
		else {
			lm.writeLog("FAIL: testResourceManagerSoundCapacity");
			failedTests += test;
		}

		test = testResourceManagerMusic();

		if (test <= 0) {
			lm.writeLog("PASS: testResourceManagerMusic");
			passedTests++;
		}
		else {
			lm.writeLog("FAIL: testResourceManagerMusic");
			failedTests += test;
		}
			
			

		test = testResourceManagerSound();

		if (test <= 0) {
			lm.writeLog("PASS: testResourceManagerSound");
			passedTests++;
		}
		else {
			lm.writeLog("FAIL: testResourceManagerSound");
			failedTests += test;
		}
			


		test = testSoundAttributes();

		if (test <= 0) {
			lm.writeLog("PASS: testSoundAttributes");
			passedTests++;
		}
		else {
			lm.writeLog("FAIL: testSoundAttributes");
			failedTests += test;
		}
			

		test = testMusicAttributes();

		if (test <= 0) {
			lm.writeLog("PASS: testMusicAttributes");
			passedTests++;
		}
		else {
			lm.writeLog("FAIL: testMusicAttributes");
			failedTests += test;
		}
			

		dm.shutDown();







		timeEndPeriod(1);


		
		lm.shutDown();



		return failedTests;
	}
	int runTests() {
		int totalFailures = 0;

		totalFailures += runLogManagerTests();
		totalFailures += runClockTests();
		totalFailures += runGameManagerTests();
		totalFailures += runVectorTests();
		totalFailures += runObjectTests();
		totalFailures += runObjectListTests();
		totalFailures += runEventTests();
		totalFailures += runWorldManagerTests();
		totalFailures += objectListTests2();
		totalFailures += std::abs(SFMLConfigTest());
		totalFailures += runDisplayManagerTests();
		totalFailures += projectect2cTests1();
		totalFailures += disperateInputManagerTest();
		
		totalFailures += runLifecycleTests();
		

		

		if (!LogManager::getInstance().isStarted()) {
			LogManager::getInstance().startUp(true);
		}
		LogManager::getInstance().writeLog("standard set passed");
		LogManager::getInstance().shutDown();
		std::printf("testing complete!\ntesting log saved to dragonfly.log\n");
		

		return totalFailures;
	}


	
}




namespace test {

	
	


	
	
}


int main() {
	return test::runTests();
	
}
