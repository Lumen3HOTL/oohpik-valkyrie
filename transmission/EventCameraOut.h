#pragma once
#pragma once
#include "Event.h"

namespace df {



	const std::string CAMERA_OUT_EVENT = "df::CameraOut";

	class EventCameraOut : public Event {

	public:
		EventCameraOut();

	};
}