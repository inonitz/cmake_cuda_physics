#ifndef __APP_INPUT_MANAGER_DEFINITION__
#define __APP_INPUT_MANAGER_DEFINITION__
#include <SDL3/SDL.h>
#include <util2/C/base_type.h>


enum class MouseKeyType : u8 {
	LEFT,
	RIGHT,
	MIDDLE,
	SCROLL,
	MOVE,
	UNKNOWN,
	MAX
};


enum class KeyboardKeyType : u8 {
	KEY_W,
	KEY_A,
	KEY_S,
	KEY_D,
	KEY_SPACE,
	KEY_NUM0,
	KEY_NUM1,
	KEY_NUM2,
	KEY_NUM3,
	KEY_NUM4,
	KEY_NUM5,
	KEY_NUM6,
	KEY_NUM7,
	KEY_NUM8,
	KEY_NUM9,
	KEY_UNKNOWN,
	KEY_MAX
};


class AppInputManager {
public:
	void create();
	void destroy();
	void onUpdate(void* appstate, SDL_Event& event);
	bool isMousePressed() 	   const;
	bool isMouseMoving() 	   const;
	bool isMouseScrollMoving() const;
	bool isKeyboardPressed()   const;
	bool isKeyboardReleased()  const;
	bool isKeyPressed(KeyboardKeyType key)  const;
	bool isKeyReleased(KeyboardKeyType key) const;
	[[nodiscard]] SDL_FPoint getMouseCursorPosition() const  {
		return m_mouseCursor;
	}
	[[nodiscard]] SDL_Point getMouseScrollOffset() const  {
		return m_mouseScroll;
	}


private:
	void resetOnNewFrame();

private:
	u8 				  m_mouseScrollMoved;
	u8 				  m_mouseMoved;
	u8 				  m_mousePressed;
	u8 				  m_keyboardPressed;
	u8 				  m_keyboardReleased;
	/* try to re-order data based on usage patterns in the hot path */
	MouseKeyType 	  m_mouseKeyEvent;
	KeyboardKeyType   m_keyboardKeyEvent;
	u8 				  m_reserved[1];
	SDL_FPoint        m_mouseCursor;
	SDL_Point         m_mouseScroll;
	SDL_FPoint        m_mouseScrollPixel;
};


#endif /* __APP_INPUT_MANAGER_DEFINITION__ */