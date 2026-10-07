#include "Ground.h"
namespace ookpik {
	Ground::Ground() {
		this->setType("Ground");
		// Open floor: nothing to draw and nothing to collide with
		this->setSolidness(df::SPECTRAL);
		this->setVisible(false);
	}
	//placeholder for game logic
}