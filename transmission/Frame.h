#pragma once
#include <string>
#include "Vector.h"
#include "Color.h"
namespace df {

	
	class Frame {
	private:
		int m_width; // Width o f frame .
		int m_height; // H e i g h t o f frame .
		std::string m_frame_str; // A l l frame c h a r a c t e r s s t o r e d a s s t r i n g .
	
	public:
		// C r e a t e empty frame .
		Frame();
		
		// C r e a t e frame o f i n d i c a t e d w i d t h and h e i g h t w i t h s t r i n g .
		Frame(int new_width, int new_height, std::string frame_str);
		
		// S e t w i d t h o f frame .
		void setWidth(int new_width);
		
		// Get w i d t h o f frame .
		int getWidth() const;
	
		// S e t h e i g h t o f frame .
		void setHeight(int new_height);
	
		// Get h e i g h t o f frame .
		int getHeight() const;
		
		// S e t frame c h a r a c t e r s ( s t o r e d a s s t r i n g ) .
		void setString(std::string new_frame_str);
		
		// Get frame c h a r a c t e r s ( s t o r e d a s s t r i n g ) .
		std::string getString() const;
		
		// Draw s e l f , c e n t e r e d a t p o s i t i o n ( x , y ) w i t h c o l o r .
		// Return 0 i f ok , e l s e −1.
		// Note : top − l e f t c o o r d i n a t e i s ( 0 , 0 ) .
		int draw(Vector position, Color color, char transparent = NULL) const;
	};
}