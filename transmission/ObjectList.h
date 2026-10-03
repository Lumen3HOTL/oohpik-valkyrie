#pragma once
#include <vector>

#include "Object.h"

namespace df {
	class ObjectList {
	
		private:
			int m_count; // Count o f o b j e c t s i n l i s t .
			std::vector<Object *> m_p_obj; // Array o f p o i n t e r s t o o b j e c t s .
		
	public:
			// D e f a u l t c o n s t r u c t o r .
			ObjectList();
		
			// I n s e r t o b j e c t p o i n t e r i n l i s t .
			// Return 0 i f ok , e l s e −1.
			int insert(Object* p_o);
		
			// Remove o b j e c t p o i n t e r from l i s t .
			// Return 0 i f found , e l s e −1.
			int remove(Object* p_o);
		
			// C l e a r l i s t ( s e t t i n g c o u n t t o 0 ) .
			void clear();
		
			// Return c o u n t o f number o f o b j e c t s i n l i s t .
			int getCount() const;
			// Return t r u e i f l i s t i s empty , e l s e f a l s e .
			bool isEmpty() const;
			
			// Return t r u e i f l i s t i s f u l l , e l s e f a l s e .
			bool isFull() const;
			
			// I n d e x i n t o l i s t .
			 Object* operator[](int index);
			 // I n d e x i n t o l i s t for const requiring vars.
			 Object* operator[](int index) const;
			 //backported from world manager
			 int objectsOfTypeCount(std::string type) const;
			 //backported from world manager
			 ObjectList objectsOfType(std::string type);
			 //useful function
			 int objectsOfAltitudeCount(unsigned int altitude) const;
			 //useful function
			 ObjectList objectsOfAltitude(unsigned int altitude);
			 //very useful function
			 std::vector<ObjectList> objectsByAltitude();
	};
}