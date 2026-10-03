#include "Manager.h"
#include "ObjectList.h"
#include "Object.h"
#include "WorldManager.h"
#include "EventManager.h"
namespace df {

	void Manager::setType(std::string type) {
		m_type = type;
	}

	Manager::Manager() {
		//prevent undefined unitialized memory behavoir
		m_type = "";
		m_is_started = false;
	}

	Manager::~Manager() {

	}

	std::string Manager::getType() const{
		return m_type;
	}

	int Manager::startUp() {
		if (m_is_started) {
			return -1;
		}
		m_is_started = true;
		return 0;
	}

	void Manager::shutDown() {

		m_is_started = false;
	}

	bool Manager::isStarted() const{
		return m_is_started;
	}

	
	int Manager::onEvent(const Event* p_event) const {
		//this is inneficent but whatever
		return EventManager::getInstance().sendEvent(p_event);
		
		
	}
}