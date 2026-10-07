#include "MapExit.h"

namespace ookpik {
	MapExit::MapExit() {
		this->setType("mapExit");
		this->setSprite("exit");
		m_used = false;
	}

	bool MapExit::use() {
		if (m_used) {
			return false;
		}
		m_used = true;
		return true;
	}
}