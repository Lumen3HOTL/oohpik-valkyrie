#pragma once
#include "Frame.h"
#include <vector>
#include <string>
#include "Clock.h"
namespace df {
	class Sprite {

		private:
			int m_width; // S p r i t e w i d t h .
			int m_height; // S p r i t e h e i g h t .
			int m_max_frame_count; // Max number f r a m e s s p r i t e can h a v e .
			int m_frame_count; // A c t u a l number f r a m e s s p r i t e h a s .
			Color m_color; // O p t i o n a l c o l o r f o r e n t i r e s p r i t e .
			int m_slowdown; // Animation slowdown (1=no slowdown , 0= s t o p ) .
			std::vector<Frame> m_frame; // Array o f f r a m e s .
			std::string m_label; // Text l a b e l t o i d e n t i f y s p r i t e .
			char m_transparency; // S p r i t e t r a n s p a r e n t c h a r a c t e r ( 0 i f none ) .
			Sprite(); // S p r i t e a l w a y s h a s one arg , t h e frame c o u n t .
			uint32_t m_custom_color;

		public:
			// D e s t r o y s p r i t e , d e l e t i n g any a l l o c a t e d f r a m e s .
			~Sprite();

			// C r e a t e s p r i t e w i t h i n d i c a t e d maximum number o f f r a m e s .
			Sprite(int max_frames);
			// S e t w i d t h o f s p r i t e .
			void setWidth(int new_width);

			// Get w i d t h o f s p r i t e .
			int getWidth() const;

			// S e t h e i g h t o f s p r i t e .
			void setHeight(int new_height);

			// Get h e i g h t o f s p r i t e .
			int getHeight() const;

			// S e t s p r i t e c o l o r .
			void setColor(Color new_color);

			// Get s p r i t e c o l o r .
			Color getColor() const;

			// Get t o t a l c o u n t o f f r a m e s i n s p r i t e .
			int getFrameCount() const;

			// Add frame t o s p r i t e .
			// Return −1 i f frame a r r a y f u l l , e l s e 0 .
			int addFrame(Frame new_frame);

			// Get n e x t s p r i t e frame i n d i c a t e d by number .
			// Return empty frame i f o u t o f r a n g e [ 0 , m f r a m e c o u n t − 1 ] .
			Frame getFrame(int frame_number) const;

			// S e t l a b e l a s s o c i a t e d w i t h s p r i t e .
			void setLabel(std::string new_label);

			// Get l a b e l a s s o c i a t e d w i t h s p r i t e .
			std::string getLabel() const;

			// S e t a n i m a ti o n slowdown v a l u e .
			// Value i n m u l t i p l e s o f GameManager frame ti m e .
			void setSlowdown(int new_sprite_slowdown);

			// Get a n i m a ti o n slowdown v a l u e .
			// Value i n m u l t i p l e s o f GameManager frame ti m e .
			int getSlowdown() const;

			// Draw i n d i c a t e d frame c e n t e r e d a t p o s i t i o n ( x , y ) .
			// Return 0 i f ok , e l s e −1.
			// Note : top − l e f t c o o r d i n a t e i s ( 0 , 0 ) .
			int draw(int frame_number, Vector position) const;

			void setCustomColor(uint32_t new_rgba);

			uint32_t getCustomColor() const;

			// S e t S p r i t e t r a n s p a r e n c y c h a r a c t e r ( 0 means none ) .
			void setTransparency(char new_transparency);
			
			// Get S p r i t e t r a n s p a r e n c y c h a r a c t e r ( 0 means none ) .
			char getTransparency() const;

	};
}

