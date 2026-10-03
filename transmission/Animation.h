#pragma once

#include <string>
#include "Sprite.h"
#include "Box.h"
namespace df {


	class Animation {
		
		private:
			Sprite * m_p_sprite; // S p r i t e a s s o c i a t e d w i t h Animation .
			std::string m_name; // S p r i t e name i n ResourceManager .
			int m_index; // C u r r e n t i n d e x frame f o r S p r i t e .
			int m_slowdown_count; // Slowdown c o u n t e r .
			
			
		public:
			// Animation c o n s t r u c t o r
			Animation();
			
			// S e t a s s o c i a t e d S p r i t e t o new one .
			// Note , S p r i t e i s managed by ResourceManager .
			// S e t S p r i t e i n d e x t o 0 ( f i r s t frame ) .
			void setSprite(Sprite * p_new_sprite);
			
			// Return p o i n t e r t o a s s o c i a t e d S p r i t e .
			Sprite* getSprite() const;
			
			// S e t S p r i t e name ( i n ResourceManager ) .
			void setName(std::string new_name);

			// Get S p r i t e name ( i n ResourceManager ) .
			std::string getName() const;
			
			// S e t i n d e x o f c u r r e n t S p r i t e frame t o b e d i s p l a y e d .
			void setIndex(int new_index);
			
			// Get i n d e x o f c u r r e n t S p r i t e frame t o b e d i s p l a y e d .
			int getIndex() const;
			
			// S e t a n i m a ti o n slowdown c o u n t (−1 means s t o p a n i m a ti o n ) .
			void setSlowdownCount(int new_slowdown_count);
			
			// S e t a n i m a ti o n slowdown c o u n t (−1 means s t o p a n i m a ti o n ) .
			int getSlowdownCount() const;
			
			// Draw s i n g l e frame c e n t e r e d a t p o s i t i o n ( x , y ) .
		    // Drawing a c c o u n t s f o r slowdown , and a d v a n c e s S p r i t e frame .
			// Return 0 i f ok , e l s e −1.
			int draw(Vector position);

			Box getBox() const;
			
	};
}