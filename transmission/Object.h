#pragma once
// System includes.
#include <string>

// Engine includes.
 #include "vector.h"
#include "Event.h"
#include "Animation.h"
#include "Box.h"
namespace df {
	enum Solidness {
		HARD, // Object causes collisions and impedes.
		SOFT, // Object causes collisions, but doesn't impede.
		SPECTRAL, // Object doesn't cause collisions.
	
	};
	const std::string UNDEFINED_OBJECT = "object";
	class Object {

	private:
		unsigned long long m_id; // Unique game engine defined identifier.
		std::string m_type; // Game programmer defined type.
		Vector m_position; // Position in game world.
		bool m_visible;
		unsigned int m_altitude;
		Vector m_direction; // Direction vector.
		float m_speed; // Object speed in direction.
		Solidness m_solidness;
		bool m_no_soft; // True if won't move on to soft objects.
		Animation m_animation;
		Box m_box;
		bool m_camera_affected;

	public:
		// Construct Object. Set default parameters and
		// add to game world (WorldManager).
		Object();
	
		// Destroy Object.
		// Remove from game world (WorldManager).
		virtual ~Object();
		
		// Set Object id.
		void setId(unsigned long long new_id);
		
		// Get Object id.
		unsigned long long getId() const;
		
		// Set type identifier of Object.
		void setType(std::string new_type);
		
		 // Get type identifier of Object.
		std::string getType() const;
		
		// Set position of Object.
		void setPosition(Vector new_pos);
		
		// Get position of Object.
		Vector getPosition() const;
		
		//the default event handler
		virtual int eventHandler(const Event* p_e);


		// Draw Object Animation.
		// Return 0 if ok, else -1
		virtual int draw();

		void setVisible(const bool newVisibility);
		bool getVisible() const;

		// Set altitude of Object, with checks for range [0, MAX_ALTITUDE].
		// Return 0 if ok, else -1.
		int setAltitude(unsigned int new_altitude);
		
		// Return altitude of Object.
		int getAltitude() const;

		// Set speed of Object.
		void setSpeed(float speed);

		// Get speed of Object.
		float getSpeed() const;

		// Set direction of Object.
		void setDirection(Vector new_direction);

		// Get direction of Object.
		Vector getDirection() const;

		// Set direction and speed of Object.
		void setVelocity(Vector new_velocity);

		// Get velocity of Object based on direction and speed.
		Vector getVelocity() const;
		
		// Predict Object position based on speed and direction.
		// Return predicted position.
		Vector predictPosition();


		bool isSolid() const; // True if HARD or SOFT, else false.
		
		// Set object solidness, with checks for consistency.
		// Return 0 if ok, else -1.
		int setSolidness(Solidness new_solid);
		
		// Return object solidness.
		Solidness getSolidness() const;

		// Set 'no_soft' setting (true - cannot move on to SOFT Objects).
		void setNoSoft(bool new_no_soft = true);
		
		// Get 'no_soft' setting (true - cannot move on to SOFT Objects).
		bool getNoSoft() const;


		// Set Sprite for this Object to animate.
		// Return 0 if ok, else -1.
		int setSprite(std::string sprite_label);
		
		// Set Animation for this Object to new one.
		// Set bounding box to size of associated Sprite.
		void setAnimation(Animation new_animation);
		
		// Get Animation for this Object.
		Animation getAnimation() const;
		

		// Set Object's bounding box.
		void setBox(Box new_box);
		
		// Get Object's bounding box.
		Box getBox() const;

		bool getCameraAffected() const;

		void setCameraAffected(bool new_Camera_Lock_state = true);
		
	};
}
 