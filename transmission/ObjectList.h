#pragma once
#include <vector>

#include "Object.h"

namespace df {
	class ObjectList {
	
		private:
			int m_count; // Count of objects in list.
			std::vector<Object *> m_p_obj; // Array of pointers to objects.
		
	public:
			// Default constructor.
			ObjectList();
		
			// Insert object pointer in list.
			// Return 0 if ok, else -1.
			int insert(Object* p_o);
		
			// Remove object pointer from list.
			// Return 0 if found, else -1.
			int remove(Object* p_o);
		
			// Clear list (setting count to 0).
			void clear();
		
			// Return count of number of objects in list.
			int getCount() const;
			// Return true if list is empty, else false.
			bool isEmpty() const;
			
			// Return true if list is full, else false.
			bool isFull() const;
			
			// Index into list.
			 Object* operator[](int index);
			 // Index into list for const requiring vars.
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