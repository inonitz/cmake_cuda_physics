#include <scenetest/input.hpp>
#include <array>


void AppInputManager::create()
{
    resetOnNewFrame();
    return;
}


void AppInputManager::destroy()
{
    resetOnNewFrame();
    return;
}


void AppInputManager::onUpdate(void* appstate, SDL_Event& event)
{
	static constexpr std::array<u32, 5> validEventTypes = {
		SDL_EVENT_MOUSE_WHEEL,
		SDL_EVENT_MOUSE_MOTION,
		SDL_EVENT_MOUSE_BUTTON_DOWN,
		SDL_EVENT_KEY_DOWN,
		SDL_EVENT_KEY_UP
	};
	static constexpr std::array<std::pair<SDL_Keycode, KeyboardKeyType>, 15> validKeyboardKeys = {
		std::pair<SDL_Keycode, KeyboardKeyType>({ SDLK_W, KeyboardKeyType::KEY_W }),
		std::pair<SDL_Keycode, KeyboardKeyType>({ SDLK_A, KeyboardKeyType::KEY_A }),
		std::pair<SDL_Keycode, KeyboardKeyType>({ SDLK_S, KeyboardKeyType::KEY_S }),
		std::pair<SDL_Keycode, KeyboardKeyType>({ SDLK_D, KeyboardKeyType::KEY_D }),
		std::pair<SDL_Keycode, KeyboardKeyType>({ SDLK_SPACE, KeyboardKeyType::KEY_SPACE }),
		std::pair<SDL_Keycode, KeyboardKeyType>({ SDLK_0, KeyboardKeyType::KEY_NUM0 }),
		std::pair<SDL_Keycode, KeyboardKeyType>({ SDLK_1, KeyboardKeyType::KEY_NUM1 }),
		std::pair<SDL_Keycode, KeyboardKeyType>({ SDLK_2, KeyboardKeyType::KEY_NUM2 }),
		std::pair<SDL_Keycode, KeyboardKeyType>({ SDLK_0, KeyboardKeyType::KEY_NUM3 }),
		std::pair<SDL_Keycode, KeyboardKeyType>({ SDLK_1, KeyboardKeyType::KEY_NUM4 }),
		std::pair<SDL_Keycode, KeyboardKeyType>({ SDLK_2, KeyboardKeyType::KEY_NUM5 }),
		std::pair<SDL_Keycode, KeyboardKeyType>({ SDLK_0, KeyboardKeyType::KEY_NUM6 }),
		std::pair<SDL_Keycode, KeyboardKeyType>({ SDLK_1, KeyboardKeyType::KEY_NUM7 }),
		std::pair<SDL_Keycode, KeyboardKeyType>({ SDLK_2, KeyboardKeyType::KEY_NUM8 }),
		std::pair<SDL_Keycode, KeyboardKeyType>({ SDLK_2, KeyboardKeyType::KEY_NUM9 })
	};
	constexpr SDL_MouseButtonFlags leftMouseButton   = SDL_BUTTON_LEFT;
	constexpr SDL_MouseButtonFlags middleMouseButton = SDL_BUTTON_MIDDLE;
	constexpr SDL_MouseButtonFlags rightMouseButton  = SDL_BUTTON_RIGHT;
	bool 	   			 eventIsRelevant = true;
	SDL_MouseButtonFlags currentMouseButton{};
    SDL_FPoint cursor = { 
		event.motion.x,
		event.motion.y
	};
	SDL_Point scrollTicks = {
		event.wheel.integer_x,
		event.wheel.integer_y
	};
	SDL_FPoint scrollPixels = {
		event.wheel.x,
		event.wheel.y
	};


	/* regardless of event type, this should be cleared to avoid data from previous frames propagating */
	resetOnNewFrame();
	
	for(auto& eventType : validEventTypes) {
		eventIsRelevant = (eventType == event.type) ? true : eventIsRelevant;
	}
	if(!eventIsRelevant) {
		return;
	}
	
	
	m_mouseScrollMoved = (event.type == validEventTypes[0]); /* SDL_EVENT_MOUSE_WHEEL		*/
	m_mouseMoved 	   = (event.type == validEventTypes[1]); /* SDL_EVENT_MOUSE_MOTION 		*/
	m_mousePressed 	   = (event.type == validEventTypes[2]); /* SDL_EVENT_MOUSE_BUTTON_DOWN */
	m_keyboardPressed  = (event.type == validEventTypes[3]); /* SDL_EVENT_KEY_DOWN 			*/
	m_keyboardReleased = (event.type == validEventTypes[4]); /* SDL_EVENT_KEY_UP 			*/
	switch(event.type) {
	case validEventTypes[0]: /* SDL_EVENT_MOUSE_WHEEL */
		m_mouseKeyEvent  = MouseKeyType::SCROLL;
		m_mouseScroll 	   = scrollTicks;
		m_mouseScrollPixel = scrollPixels;
	break;
	case validEventTypes[1]: /* SDL_EVENT_MOUSE_MOTION */
		m_mouseKeyEvent = MouseKeyType::MOVE;
		m_mouseCursor   = cursor;
	break;
	case validEventTypes[2]: /* SDL_EVENT_MOUSE_BUTTON_DOWN */
		currentMouseButton = SDL_GetMouseState(nullptr, nullptr);
		m_mouseKeyEvent = (currentMouseButton == leftMouseButton  ) ? MouseKeyType::LEFT   : m_mouseKeyEvent;
		m_mouseKeyEvent = (currentMouseButton == middleMouseButton) ? MouseKeyType::MIDDLE : m_mouseKeyEvent;
		m_mouseKeyEvent = (currentMouseButton == rightMouseButton ) ? MouseKeyType::RIGHT  : m_mouseKeyEvent;
	break;
	case validEventTypes[3]: /* SDL_EVENT_KEY_DOWN */
    case validEventTypes[4]: /* SDL_EVENT_KEY_UP */
        KeyboardKeyType keyMatch = KeyboardKeyType::KEY_UNKNOWN;
        for(auto& validKey : validKeyboardKeys) {
            keyMatch = (validKey.first == event.key.key) ? validKey.second : keyMatch;
        }
        m_keyboardKeyEvent = keyMatch;
	break;
	}


    return;
}


bool AppInputManager::isMousePressed()  const {
    return m_mousePressed;
}

bool AppInputManager::isMouseMoving() const {
    return m_mouseMoved;
}

bool AppInputManager::isMouseScrollMoving() const {
    return m_mouseScrollMoved;
}

bool AppInputManager::isKeyboardPressed() const {
    return m_keyboardPressed;
}

bool AppInputManager::isKeyboardReleased() const {
    return m_keyboardReleased;
}


bool AppInputManager::isKeyPressed(KeyboardKeyType key) const {
    return (m_keyboardKeyEvent == key) && m_keyboardPressed;
}

bool AppInputManager::isKeyReleased(KeyboardKeyType key) const {
    return (m_keyboardKeyEvent == key) && m_keyboardReleased;
}



void AppInputManager::resetOnNewFrame()
{
	/* hope this gets optimized to a memset/memcpy(0) */
	m_mousePressed	   = false;
	m_mouseMoved	   = false;
	m_mouseScrollMoved = false;
	m_keyboardPressed  = false;
	m_keyboardReleased = false;
	m_mouseKeyEvent    = MouseKeyType::UNKNOWN;
	m_keyboardKeyEvent = KeyboardKeyType::KEY_UNKNOWN;
	m_mouseCursor	   = { 0, 0 };
	m_mouseScroll	   = { 0, 0 };
	m_mouseScrollPixel = { 0, 0 };
	return;
}