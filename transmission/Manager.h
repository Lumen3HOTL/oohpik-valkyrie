#pragma once
#include <string>
#include "Event.h"
namespace df {
	
	class Manager {
	
		 private:
			 std::string m_type; // Manager t y p e i d e n t i f i e r .
			 bool m_is_started; // True when s t a r t e d s u c c e s s f u l l y .
			
		 protected:
			 // S e t t y p e i d e n t i f i e r o f Manager .
				 void setType(std::string type);
			
		 public:
			 Manager();
			 virtual ~Manager();
			
			// Get t y p e i d e n t i f i e r o f Manager .
			std::string getType() const;
			
			// S t a r t u p Manager .
			// Return 0 i f ok , e l s e n e g a t i v e number .
			virtual int startUp();
			
			// Shutdown Manager .
			virtual void shutDown();
			
			// Return t r u e when s t a r t U p ( ) was e x e c u t e d ok , e l s e f a l s e .
			bool isStarted() const;
			
			// Send e v e n t t o a l l O b j e c t s .
			// Return c o u n t o f number o f O b j e c t s t h a t h a n d l e d t h e e v e n t .
			int onEvent(const Event* p_event) const;

		};
	
		
} // end o f namespace d f