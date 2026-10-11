#include "Enemy.h"
#include "WorldManager.h"
#include "DisplayManager.h"
#include "EventManager.h"
#include "EventStep.h"
#include <random>
#include <vector>

namespace ookpik {
	namespace {
		// One tile in each direction, the same hops the owl makes: up, right, down, left
		const int STEP_X[] = { 0, 2, 0, -2 };
		const int STEP_Y[] = { -1, 0, 1, 0 };

		std::mt19937& randomEngine() {
			static std::mt19937 engine(std::random_device{}());
			return engine;
		}
	}

	Enemy::Enemy() {
		setType("Enemy");
		setSprite("enemy");
		setSolidness(df::HARD); // the owl can't pass through it: touching it is fatal
		m_move_steps = 0;
		m_move_interval = ENEMY_MOVE_STEPS;
		df::EventManager::getInstance().registerEvent(this, df::STEP_EVENT);
	}

	void Enemy::setMoveInterval(int steps) {
		if (steps >= 1) {
			m_move_interval = steps;
		}
	}

	int Enemy::getMoveInterval() const {
		return m_move_interval;
	}

	int Enemy::eventHandler(const df::Event* p_e) {
		if (p_e->getType() == df::STEP_EVENT) {
			m_move_steps++;
			if (m_move_steps >= m_move_interval) {
				m_move_steps = 0;
				moveOnce();
			}
			return 1;
		}
		return 0;
	}

	bool Enemy::moveOnce() {
		df::DisplayManager& display = df::DisplayManager::getInstance();
		std::vector<df::Vector> options;
		for (int dir = 0; dir < 4; dir++) {
			df::Vector target(getPosition().getX() + STEP_X[dir], getPosition().getY() + STEP_Y[dir]);
			if (target.getX() < 0 || target.getY() < 0 ||
				target.getX() >= display.getHorizontal() || target.getY() >= display.getVertical()) {
				continue;
			}
			// Trees and other enemies block it; seeds, the exit and the owl don't
			bool blocked = false;
			df::ObjectList in_the_way = WM.getCollisions(this, target);
			for (int i = 0; i < in_the_way.getCount(); i++) {
				if (in_the_way[i]->getType() == "Tree" || in_the_way[i]->getType() == "Enemy") {
					blocked = true;
				}
			}
			if (!blocked) {
				options.push_back(target);
			}
		}
		if (options.empty()) {
			return false;
		}
		std::uniform_int_distribution<int> pick(0, (int)options.size() - 1);
		// Moving onto the owl sends it a collision event, which kills it
		WM.moveObject(this, options[pick(randomEngine())]);
		return true;
	}
}
