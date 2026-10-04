#include "ObjectList.h"
#include <stdexcept>
#include <cstdio>
#include "WorldManager.h"
namespace df {
	ObjectList::ObjectList() {
		m_count = 0;
		m_p_obj = std::vector<Object *>();
	}

	int ObjectList::insert(Object* p_o) {
		if (p_o == nullptr) {
			return -1;
		}
		//check if the object is already in the list and if so dont add it
		for (int i = 0; i < m_p_obj.size(); i++) {
			if (((Object*)m_p_obj[i]) == ((Object*)p_o)) {
				return -1;
			}
		}
		//otherwise add it and up the count
		m_p_obj.push_back(p_o);
		m_count++;
		return 0;
	}

	int ObjectList::remove(Object* p_o) {
		//if empty obvoisly error out
		if (m_count < 1) {
			return -1;
		}
		if (p_o == nullptr) {
			return -1;
		}
		
		//speed i,provment variable for storing if we found something
		int found = -1;
		//loop through the list
		for (int i = 0; i < m_count; i++) {
			//if we find the pointer
			if (p_o == m_p_obj[i]) {
				//perform a truncated swap delete
				m_p_obj[i]= m_p_obj[m_count - 1];
				m_p_obj.pop_back();
				//update count
				m_count--;
				//set that we found the pointer
				found = 0;
			}
		}
		return found;
	}

	void ObjectList::clear() {
		m_p_obj.clear();
		m_count = 0;

	}

	int ObjectList::getCount() const {
		return m_count;
	}

	bool ObjectList::isEmpty() const {
		return (m_count == 0);
	}

	bool ObjectList::isFull() const {
		return false;
	}

	// Index into list.
	Object* ObjectList::operator[](int index) {
		//safety check
		
		if ((index < 0) || (index >= m_count)) {
			throw std::out_of_range("Invalid index!");
		}
		return ((Object*) m_p_obj[index]);
	}

	// Index into list with const.
	Object* ObjectList::operator[](int index) const{
		//safety check
		
		if ((index < 0) || (index >= m_count)) {
			throw std::out_of_range("Invalid index!");
		}
		return ((Object*)m_p_obj[index]);
	}


	int ObjectList::objectsOfTypeCount(std::string type) const {
		int count = 0;
		for (int i = 0; i < m_count; i++) {
			if (m_p_obj[i]->getType().compare(type) == 0) {
				count++;
			}
		}
		return count;
	}
	ObjectList ObjectList::objectsOfType(std::string type) {
		//efficently find and assemble a new list of all the ovbjects of a type
		ObjectList found = ObjectList();
		Object* check = nullptr;
		for (int i = 0; i < m_count; i++) {
			check = m_p_obj[i];
			if (check->getType().compare(type) == 0) {
				found.insert(check);
			}
		}
		return found;
	}

	//useful function
	int ObjectList::objectsOfAltitudeCount(unsigned int altitude) const {
		if (altitude <= MAX_ALTITUDE) {
			unsigned int altCount = 0;
			for (int index = 0; index < m_count; index++) {
				if (m_p_obj[index]->getAltitude() == altitude) {
					altCount++;
				}
			}
			return altCount;
		}
		return -1;
	}
	//useful function
	ObjectList ObjectList::objectsOfAltitude(unsigned int altitude) {
		if (altitude <= MAX_ALTITUDE) {
			ObjectList altObjs = ObjectList();
			for (int index = 0; index < m_count; index++) {
				if (m_p_obj[index]->getAltitude() == altitude) {
					altObjs.insert(m_p_obj[index]);
				}
			}
			return altObjs;
		}
		return ObjectList();
	}

	//very useful function
	std::vector<ObjectList> ObjectList::objectsByAltitude() {
		std::vector<ObjectList> altVect = std::vector<ObjectList>(MAX_ALTITUDE+1);
		
		int alt = 0;
		for (int i = 0; i < m_count; i++) {
			alt = m_p_obj[i]->getAltitude();
			altVect[alt].insert(m_p_obj[i]);
		}
		return altVect;
	}
}

