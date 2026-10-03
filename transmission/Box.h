#pragma once
#include "Vector.h"
namespace df {


	
	class Box {
		
		private:
			Vector m_corner; // Upper l e f t c o r n e r o f b o x .
			float m_horizontal; // H o r i z o n t a l d i m e n s i o n .
			float m_vertical; // V e r t i c a l d i m e n s i o n .
			
		public:
			// C r e a t e b o x w i t h ( 0 , 0 ) f o r t h e c o r n e r , and 0 f o r h o r i z and v e r t .
			Box();
		
			// C r e a t e b o x w i t h an upper − l e f t c o r n e r , h o r i z and v e r t s i z e s .
			Box(Vector init_corner, float init_horizontal, float init_vertical);
			
			// S e t u p p e r l e f t c o r n e r o f b o x .
			void setCorner(Vector new_corner);
			
			// Get u p p e r l e f t c o r n e r o f b o x .
			Vector getCorner() const;
			
			// S e t h o r i z o n t a l s i z e o f b o x .
			void setHorizontal(float new_horizontal);
			
			// Get h o r i z o n t a l s i z e o f b o x .
			float getHorizontal() const;
			
				// S e t v e r t i c a l s i z e o f b o x .

			void setVertical(float new_vertical);
	
			// Get v e r t i c a l s i z e o f b o x .
			float getVertical() const;
	
	};
}