#pragma once
// System i n c l u d e s .
#include <string>

// Engine i n c l u d e s .
 #include "vector.h"
#include "Event.h"
#include "Animation.h"
#include "Box.h"
namespace df {
	enum Solidness {
		HARD, // O b j e c t c a u s e s c o l l i s i o n s and i m p e d e s .
		SOFT, // O b j e c t c a u s e s c o l l i s i o n s , b u t d o e s n ’ t impede .
		SPECTRAL, // O b j e c t d o e s n ’ t c a u s e c o l l i s i o n s .
	
	};
	const std::string UNDEFINED_OBJECT = "object";
	class Object {

	private:
		unsigned long long m_id; // Unique game e n g i n e d e f i n e d i d e n t i f i e r .
		std::string m_type; // Game programmer d e f i n e d t y p e .
		Vector m_position; // P o s i t i o n i n game w o r l d .
		bool m_visible;
		unsigned int m_altitude;
		Vector m_direction; // D i r e c t i o n v e c t o r .
		float m_speed; // O b j e c t s p e e d i n d i r e c t i o n .
		Solidness m_solidness;
		bool m_no_soft; // True i f won ’ t move o n to s o f t o b j e c t s .
		Animation m_animation;
		Box m_box;
		bool m_camera_affected;

	public:
		// C o n s t r u c t O b j e c t . S e t d e f a u l t p a r a m e t e r s and
		// add t o game w o r l d ( WorldManager ) .
		Object();
	
		// D e s t r o y O b j e c t .
		// Remove from game w o r l d ( WorldManager ) .
		virtual ~Object();
		
		// S e t O b j e c t i d .
		void setId(unsigned long long new_id);
		
		// Get O b j e c t i d .
		unsigned long long getId() const;
		
		// S e t t y p e i d e n t i f i e r o f O b j e c t .
		void setType(std::string new_type);
		
		 // Get t y p e i d e n t i f i e r o f O b j e c t .
		std::string getType() const;
		
		// S e t p o s i t i o n o f O b j e c t .
		void setPosition(Vector new_pos);
		
		// Get p o s i t i o n o f O b j e c t .
		Vector getPosition() const;
		
		//the default event handler
		virtual int eventHandler(const Event* p_e);


		// Draw O b j e c t Animation .
		// Return 0 i f ok , e l s e −1
		virtual int draw();

		void setVisible(const bool newVisibility);
		bool getVisible() const;

		// S e t a l t i t u d e o f O b j e c t , w i t h c h e c k s f o r r a n g e [ 0 , MAX ALTITUDE] .
		// Return 0 i f ok , e l s e −1.
		int setAltitude(unsigned int new_altitude);
		
		// Return a l t i t u d e o f O b j e c t .
		int getAltitude() const;

		// S e t s p e e d o f O b j e c t .
		void setSpeed(float speed);

		// Get s p e e d o f O b j e c t .
		float getSpeed() const;

		// S e t d i r e c t i o n o f O b j e c t .
		void setDirection(Vector new_direction);

		// Get d i r e c t i o n o f O b j e c t .
		Vector getDirection() const;

		// S e t d i r e c t i o n and s p e e d o f O b j e c t .
		void setVelocity(Vector new_velocity);

		// Get v e l o c i t y o f O b j e c t b a s e d on d i r e c t i o n and s p e e d .
		Vector getVelocity() const;
		
		// P r e d i c t O b j e c t p o s i t i o n b a s e d on s p e e d and d i r e c t i o n .
		// Return p r e d i c t e d p o s i t i o n .
		Vector predictPosition();


		bool isSolid() const; // True i f HARD o r SOFT, e l s e f a l s e .
		
		// S e t o b j e c t s o l i d n e s s , w i t h c h e c k s f o r c o n s i s t e n c y .
		// Return 0 i f ok , e l s e −1.
		int setSolidness(Solidness new_solid);
		
		// Return o b j e c t s o l i d n e s s .
		Solidness getSolidness() const;

		// S e t ‘ no s o f t ’ s e t t i n g ( t r u e − c a n n o t move o n to SOFT O b j e c t s ) .
		void setNoSoft(bool new_no_soft = true);
		
		// Get ‘ no s o f t ’ s e t t i n g ( t r u e − c a n n o t move o n to SOFT O b j e c t s ) .
		bool getNoSoft() const;


		// S e t S p r i t e f o r t h i s O b j e c t t o a n i m a te .
		// Return 0 i f ok , e l s e −1.
		int setSprite(std::string sprite_label);
		
		// S e t Animation f o r t h i s O b j e c t t o new one .
		// S e t b o u n d i n g b o x t o s i z e o f a s s o c i a t e d S p r i t e .
		void setAnimation(Animation new_animation);
		
		// Get Animation f o r t h i s O b j e c t .
		Animation getAnimation() const;
		

		// S e t O b j e c t ’ s b o u n d i n g b o x .
		void setBox(Box new_box);
		
		// Get O b j e c t ’ s b o u n d i n g b o x .
		Box getBox() const;

		bool getCameraAffected() const;

		void setCameraAffected(bool new_Camera_Lock_state = true);
		
	};
}
 