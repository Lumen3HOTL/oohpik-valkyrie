#pragma once
#include "Manager.h"
#include "ObjectList.h"
#include "Vector.h"
#include "Box.h"
namespace df {
	const unsigned int MAX_ALTITUDE = 4;
	const int WORLD_HORIZONTAL_CHARS_DEFAULT = 500;
	const int WORLD_VERTICAL_CHARS_DEFAULT = 500;
	class WorldManager : public Manager {
		private:
			WorldManager(); // P r i v a t e ( a s i n g l e t o n ) .
			WorldManager(WorldManager const&); // Don ’ t a l l o w copy .
			void operator =(WorldManager const&); // Don ’ t a l l o w a s s i g n m e n t .
			
			ObjectList m_updates; // A l l O b j e c t s i n w o r l d t o u p d a t e .
			ObjectList m_deletions; // A l l O b j e c t s i n w o r l d t o d e l e t e .
			
			Box m_world_box;
		 public:

			 int setWorldBox(Box new_world_space);

			 Box getWorldSpaceBox()const;
			 
			 int massFrustromCheck();
			 bool frustromCheck(Object* p_o);
			 

		// Get t h e one and o n l y i n s t a n c e o f t h e WorldManager .
			static WorldManager &getInstance();
		
			// S t a r t u p game w o r l d ( i n i t i a l i z e e v e r y t h i n g t o empty ) .
			// Return 0 .
			int startUp();
	
			// Shutdown game w o r l d ( d e l e t e a l l game w o r l d O b j e c t s ) .
			void shutDown();
		
			// I n s e r t O b j e c t i n t o w o r l d . Return 0 i f ok , e l s e −1.
			int insertObject(Object * p_o);
		
			// Remove O b j e c t from w o r l d . Return 0 i f ok , e l s e −1.
			int removeObject(Object * p_o);
		
			// Return l i s t o f a l l O b j e c t s i n w o r l d .
			ObjectList getAllObjects() const;
		
			// Return l i s t o f a l l O b j e c t s i n w o r l d m a tc h i n g t y p e .
			ObjectList objectsOfType(std::string type);
		
			// Update w o r l d .
			// D e l e t e O b j e c t s marked f o r d e l e t i o n .
			void update();

			int AllObjectsCount() const;

			int objectsOfTypeCount(std::string type) const;
		
			// I n d i c a t e O b j e c t i s t o b e d e l e t e d a t end o f c u r r e n t game l o o p .
			// Return 0 i f ok , e l s e −1.
			int markForDelete(Object * p_o);
		
			void draw();

			// Return l i s t o f O b j e c t s c o l l i d e d w i t h a t p o s i t i o n ‘ where ’ .
			// C o l l i s i o n s o n l y w i t h s o l i d O b j e c t s .
			// Does n o t c o n s i d e r i f p o i s s o l i d o r n o t .
			ObjectList getCollisions(const Object * p_o, Vector where);
			
			// Move O b j e c t .
			// I f c o l l i s i o n w i t h s o l i d , s e n d c o l l i s i o n e v e n t s .
			// I f no c o l l i s i o n w i t h s o l i d , move ok e l s e don ’ t move O b j e c t .
			// I f O b j e c t i s S p e c t r a l , move ok .
			// Return 0 i f move ok , e l s e −1 i f c o l l i s i o n w i t h s o l i d .
			int moveObject(Object * p_o, Vector where);
	};

}