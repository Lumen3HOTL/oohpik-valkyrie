#include "Sprite.h"
#include "DisplayManager.h"

namespace df {
	Sprite::Sprite() {
		// S p r i t e a l w a y s h a s one arg , t h e frame c o u n t .
		m_color = UNDEFINED_COLOR;
		m_frame=std::vector<Frame>();
		m_frame_count = 0;
		m_height = 0;
		m_label = "";
		m_max_frame_count = 0;
		m_slowdown = 0;
		char m_transparency=NULL; // S p r i t e t r a n s p a r e n t c h a r a c t e r ( 0 i f none ) .
		m_custom_color = 0;
	}


	// D e s t r o y s p r i t e , d e l e t i n g any a l l o c a t e d f r a m e s .
	Sprite::~Sprite() {
		m_color = UNDEFINED_COLOR;
		m_frame.clear();
		m_frame_count = 0;
		m_height = 0;
		m_label = "";
		m_max_frame_count = 0;
		m_slowdown = 0;
	}

	// C r e a t e s p r i t e w i t h i n d i c a t e d maximum number o f f r a m e s .
	Sprite::Sprite(int max_frames) {
		m_max_frame_count = max_frames;
		m_color = UNDEFINED_COLOR;
		m_frame = std::vector<Frame>();
		m_frame_count = 0;
		m_height = 0;
		m_label = "";
	
		m_slowdown = 0;
		char m_transparency = NULL; // S p r i t e t r a n s p a r e n t c h a r a c t e r ( 0 i f none ) .
		m_custom_color = 0;
	}
	// S e t w i d t h o f s p r i t e .
	void Sprite::setWidth(int new_width) {
		m_width = new_width;
	}

	// Get w i d t h o f s p r i t e .
	int Sprite::getWidth() const {
		return m_width;
	}

	// S e t h e i g h t o f s p r i t e .
	void Sprite::setHeight(int new_height) {
		m_height = new_height;

	}

	// Get h e i g h t o f s p r i t e .
	int Sprite::getHeight() const {
		return m_height;
	}

	// S e t s p r i t e c o l o r .
	void Sprite::setColor(Color new_color) {
		m_color = new_color;
	}

	// Get s p r i t e c o l o r .
	Color Sprite::getColor() const {
		return m_color;
	}

	// Get t o t a l c o u n t o f f r a m e s i n s p r i t e .
	int Sprite::getFrameCount() const {
		return m_frame_count;
	}

	// Add frame t o s p r i t e .
	// Return −1 i f frame a r r a y f u l l , e l s e 0 .
	int Sprite::addFrame(Frame new_frame) {
		if (m_frame_count >= m_max_frame_count) {
			return -1;
		}
		m_frame.push_back(new_frame);
		m_frame_count++;
		return 0;
	}

	// Get n e x t s p r i t e frame i n d i c a t e d by number .
	// Return empty frame i f o u t o f r a n g e [ 0 , m f r a m e c o u n t − 1 ] .
	Frame Sprite::getFrame(int frame_number) const {
		if ((frame_number < 0) || (frame_number > m_frame_count - 1)) {
			return Frame();
		}
		return m_frame[frame_number];
	}

	// S e t l a b e l a s s o c i a t e d w i t h s p r i t e .
	void Sprite::setLabel(std::string new_label) {
		m_label = new_label;
	}

	// Get l a b e l a s s o c i a t e d w i t h s p r i t e .
	std::string Sprite::getLabel() const {
		return m_label;
	}

	// S e t a n i m a ti o n slowdown v a l u e .
	// Value i n m u l t i p l e s o f GameManager frame ti m e .
	void Sprite::setSlowdown(int new_sprite_slowdown) {
		m_slowdown = new_sprite_slowdown;
	}

	// Get a n i m a ti o n slowdown v a l u e .
	// Value i n m u l t i p l e s o f GameManager frame ti m e .
	int Sprite::getSlowdown() const {
		return m_slowdown;
	}

	// Draw i n d i c a t e d frame c e n t e r e d a t p o s i t i o n ( x , y ) .
	// Return 0 i f ok , e l s e −1.
	// Note : top − l e f t c o o r d i n a t e i s ( 0 , 0 ) .
	int Sprite::draw(int frame_number, Vector position) const {
		if ((frame_number < 0) || (frame_number > m_frame_count - 1)) {
			return -1;
		}
		if (m_color == CUSTOM_COLOR) {
			DisplayManager& dm = DisplayManager::getInstance();
			uint32_t origonalCustom = dm.getCustomColor();
			dm.setCustomColor(m_custom_color);
			int success = m_frame[frame_number].draw(position, m_color,m_transparency);
			dm.setCustomColor(origonalCustom);
			return success;
		}
		int success = m_frame[frame_number].draw(position, m_color,m_transparency);
		return success;
	}

	// S e t S p r i t e t r a n s p a r e n c y c h a r a c t e r ( 0 means none ) .
	void Sprite::setTransparency(char new_transparency) {
		m_transparency = new_transparency;
	}

	// Get S p r i t e t r a n s p a r e n c y c h a r a c t e r ( 0 means none ) .
	char Sprite::getTransparency() const {
		return m_transparency;
	}

	void Sprite::setCustomColor(uint32_t new_rgba) {
		m_custom_color = new_rgba;
	}

	uint32_t Sprite::getCustomColor() const {
		return m_custom_color;
	}
}