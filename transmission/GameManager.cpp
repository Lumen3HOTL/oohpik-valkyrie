#include "GameManager.h"
#include "LogManager.h"
#include "WorldManager.h"
#include "Clock.h"
#include "EventStep.h"
#include <Windows.h>
#include "DisplayManager.h"
#include "InputManager.h"
#include "ResourceManager.h"
#include "CameraManager.h"
#include "TimerManager.h"
#include "EventManager.h"

namespace df {
	//make sure our variables arent unitialized and set our type
	GameManager::GameManager() {
		this->setType("GameManager");
		m_game_over = true;
		m_frame_time = 0;
		m_last_frame_time = 0;
		unsigned long long m_lastDeltaTime;
		Clock m_lastDeltaClock;
	}
	
	//singleton logic
	GameManager& GameManager::getInstance() {
		static GameManager gameWrangler;
		return gameWrangler;
	}

	
	int GameManager::startUp(bool appendRun) {
		//make sure we dont start up twice
		if (Manager::startUp() < 0) {
			return -1;
		}
		//init our vars
		m_game_over = false;
		m_frame_time = FRAME_TIME_DEFAULT;
		//start up the other managers and set our time period
		if (df::LogManager::getInstance().startUp(appendRun) < 0) {
			Manager::shutDown();
			return -1;
		}
		timeBeginPeriod(1);
		if (CameraManager::getInstance().startUp() < 0) {

			df::LogManager::getInstance().shutDown();
			timeEndPeriod(1);
			Manager::shutDown();
			return -1;
		}
		if (df::DisplayManager::getInstance().startUp() < 0) {
			df::DisplayManager::getInstance().shutDown();
			Manager::shutDown();

			df::LogManager::getInstance().shutDown();
			timeEndPeriod(1);
			return -1;
		}
		if (df::WorldManager::getInstance().startUp() < 0) {
			df::DisplayManager::getInstance().shutDown();
			Manager::shutDown();
			df::LogManager::getInstance().shutDown();
			timeEndPeriod(1);
			return - 1;
		}

		if (df::EventManager::getInstance().startUp() < 0) {
			df::WorldManager::getInstance().shutDown();
			df::DisplayManager::getInstance().shutDown();
			Manager::shutDown();
			df::LogManager::getInstance().shutDown();
			timeEndPeriod(1);
			return -1;
		}

		if (df::InputManager::getInstance().startUp() < 0) {
			df::WorldManager::getInstance().shutDown();
			df::DisplayManager::getInstance().shutDown();
			Manager::shutDown();
			df::LogManager::getInstance().shutDown();
			timeEndPeriod(1);
			return -1;

		}

		if (df::ResourceManager::getInstance().startUp() < 0) {
			df::InputManager::getInstance().shutDown();
			df::WorldManager::getInstance().shutDown();
			df::DisplayManager::getInstance().shutDown();
			Manager::shutDown();
			df::LogManager::getInstance().shutDown();
			timeEndPeriod(1);
			return -1;
		}

		if (df::TimerManager::getInstance().startUp() < 0) {

			df::InputManager::getInstance().shutDown();
			df::WorldManager::getInstance().shutDown();
			df::DisplayManager::getInstance().shutDown();
			df::ResourceManager::getInstance().shutDown();
			Manager::shutDown();
			df::LogManager::getInstance().shutDown();
			timeEndPeriod(1);
			return -1;
		}
		
		return 0;
	}


	void GameManager::shutDown() {
		//end the game
		this->setGameOver();

		//shutdown the other managers, then ourself, then the log manager
		InputManager::getInstance().shutDown();
		WorldManager::getInstance().shutDown();
		DisplayManager::getInstance().shutDown();
		CameraManager::getInstance().shutDown();
		EventManager::getInstance().shutDown();
		ResourceManager::getInstance().shutDown();
		TimerManager::getInstance().shutDown();
		Manager::shutDown();
		df::LogManager::getInstance().shutDown();
		//end the time thing
		timeEndPeriod(1);
	}

	void GameManager::run() {
		if (this->isStarted()) {
			//make sure we arent on game over
			this->setGameOver(false);
			//get a clock
			Clock loopClock = Clock();
			m_lastDeltaClock.delta();
			//get our delta vars made
			long long lastTime = 0;
			long long elapsed = 0;
			int sleepTime = 0;
			//get the world manager
			WorldManager& world = WorldManager::getInstance();
			//get the display manager
			DisplayManager& display = DisplayManager::getInstance();
			//get the input manager
			InputManager& input = InputManager::getInstance();

			TimerManager& timers = TimerManager::getInstance();

			unsigned long long loopCount = 0;

			while (!m_game_over) {
				lastTime = loopClock.delta();

				//put step event code here
				EventStep thisEvent = EventStep(loopCount);
				this->onEvent(&thisEvent);
				loopCount++;

				//put logic here


				input.getInput();

				//put timer code here
				timers.update();

				//update the world
				world.update();
				//draw the world
				world.draw();
				display.swapBuffers();
				//calcuate the remaining frame time and if neccesary sleep for a bit
				elapsed = loopClock.delta();
				m_last_frame_time = elapsed;
				sleepTime = m_frame_time - (elapsed / 1000);
				if (sleepTime <= 0) {
					sleepTime = 0;
				}
				else {
					Sleep(sleepTime);
				}
				m_last_delta_time = m_lastDeltaClock.delta();
			}
		}
		
	}


	unsigned int GameManager::getLastFrameTime()const {
		if (this->isStarted()) {
			return m_last_frame_time;
		}
		return MAXINT32;
	}
	unsigned int GameManager::getLastDeltaTime()const {
		if (this->isStarted()) {
			return m_last_delta_time;
		}
		return MAXINT32;
	}

	void GameManager::setGameOver(bool new_game_over) {
		if (this->isStarted()) {
			m_game_over = new_game_over;
		}
		
	}

	bool GameManager::getGameOver() const{
		if (this->isStarted()) {
			return m_game_over;
		}
		return false;
	}

	int GameManager::getFrameTime() const {
		if (this->isStarted()) {
			return m_frame_time;
		}
		return -1;
	}
}