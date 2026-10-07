#include "Seed.h"
#include "WorldManager.h"

namespace ookpik {
	Seed::Seed() {
		this->setType("Seed");
		this->setSprite("seed");
		m_collected = false;
	}

	bool Seed::collect() {
		if (m_collected) {
			return false;
		}
		m_collected = true;
		WM.markForDelete(this); // removed at the end of this frame
		return true;
	}
}