#include <glad/glad.h>

#include <Ludus/Platform/Input.h>

namespace Ludus::Platform
{
	Input::Input()
		: m_Handle(nullptr), m_PressedKeys(), m_PressedButtons()
	{ }

	Input::~Input()
	{ }

	void Input::SetHandle(GLFWwindow* handle)
	{
		m_Handle = handle;
	}

	bool Input::GetKey(Key key)
	{
		return glfwGetKey(m_Handle, MapKey(key)) == GLFW_PRESS;
	}

	bool Input::GetKeyDown(Key key)
	{
		if (GetKey(key) && !m_PressedKeys.contains(key))
		{
			m_PressedKeys.insert(key);
			return true;
		}

		if (glfwGetKey(m_Handle, MapKey(key)) == GLFW_RELEASE)
		{
			m_PressedKeys.erase(key);
		}

		return false;
	}

	bool Input::GetMouseButton(MouseButton button)
	{
		return glfwGetMouseButton(m_Handle, MapMouseButton(button)) == GLFW_PRESS;
	}

	bool Input::GetMouseButtonDown(MouseButton button)
	{
		if (GetMouseButton(button) && !m_PressedButtons.contains(button))
		{
			m_PressedButtons.insert(button);
			return true;
		}

		if (glfwGetMouseButton(m_Handle, MapMouseButton(button)) == GLFW_RELEASE)
		{
			m_PressedButtons.erase(button);
		}

		return false;
	}

	const Ludus::Math::Vector2D Input::GetMousePosition()
	{
		double xPosition, yPosition;
		glfwGetCursorPos(m_Handle, &xPosition, &yPosition);

		return { (float)xPosition, (float)yPosition };
	}

#pragma region Mapping

	int Input::MapKey(Key key)
	{
		switch (key)
		{
			case Key::Unknown:        return GLFW_KEY_UNKNOWN;

			case Key::Space:          return GLFW_KEY_SPACE;
			case Key::Apostrophe:     return GLFW_KEY_APOSTROPHE;
			case Key::Comma:          return GLFW_KEY_COMMA;
			case Key::Minus:          return GLFW_KEY_MINUS;
			case Key::Period:         return GLFW_KEY_PERIOD;
			case Key::Slash:          return GLFW_KEY_SLASH;

			case Key::D0:             return GLFW_KEY_0;
			case Key::D1:             return GLFW_KEY_1;
			case Key::D2:             return GLFW_KEY_2;
			case Key::D3:             return GLFW_KEY_3;
			case Key::D4:             return GLFW_KEY_4;
			case Key::D5:             return GLFW_KEY_5;
			case Key::D6:             return GLFW_KEY_6;
			case Key::D7:             return GLFW_KEY_7;
			case Key::D8:             return GLFW_KEY_8;
			case Key::D9:             return GLFW_KEY_9;

			case Key::Semicolon:      return GLFW_KEY_SEMICOLON;
			case Key::Equal:          return GLFW_KEY_EQUAL;

			case Key::A:              return GLFW_KEY_A;
			case Key::B:              return GLFW_KEY_B;
			case Key::C:              return GLFW_KEY_C;
			case Key::D:              return GLFW_KEY_D;
			case Key::E:              return GLFW_KEY_E;
			case Key::F:              return GLFW_KEY_F;
			case Key::G:              return GLFW_KEY_G;
			case Key::H:              return GLFW_KEY_H;
			case Key::I:              return GLFW_KEY_I;
			case Key::J:              return GLFW_KEY_J;
			case Key::K:              return GLFW_KEY_K;
			case Key::L:              return GLFW_KEY_L;
			case Key::M:              return GLFW_KEY_M;
			case Key::N:              return GLFW_KEY_N;
			case Key::O:              return GLFW_KEY_O;
			case Key::P:              return GLFW_KEY_P;
			case Key::Q:              return GLFW_KEY_Q;
			case Key::R:              return GLFW_KEY_R;
			case Key::S:              return GLFW_KEY_S;
			case Key::T:              return GLFW_KEY_T;
			case Key::U:              return GLFW_KEY_U;
			case Key::V:              return GLFW_KEY_V;
			case Key::W:              return GLFW_KEY_W;
			case Key::X:              return GLFW_KEY_X;
			case Key::Y:              return GLFW_KEY_Y;
			case Key::Z:              return GLFW_KEY_Z;

			case Key::LeftBracket:    return GLFW_KEY_LEFT_BRACKET;
			case Key::Backslash:      return GLFW_KEY_BACKSLASH;
			case Key::RightBracket:   return GLFW_KEY_RIGHT_BRACKET;
			case Key::GraveAccent:    return GLFW_KEY_GRAVE_ACCENT;

			case Key::Escape:         return GLFW_KEY_ESCAPE;
			case Key::Enter:          return GLFW_KEY_ENTER;
			case Key::Tab:            return GLFW_KEY_TAB;
			case Key::Backspace:      return GLFW_KEY_BACKSPACE;
			case Key::Insert:         return GLFW_KEY_INSERT;
			case Key::DeleteKey:      return GLFW_KEY_DELETE;
			case Key::Right:          return GLFW_KEY_RIGHT;
			case Key::Left:           return GLFW_KEY_LEFT;
			case Key::Down:           return GLFW_KEY_DOWN;
			case Key::Up:             return GLFW_KEY_UP;
			case Key::PageUp:         return GLFW_KEY_PAGE_UP;
			case Key::PageDown:       return GLFW_KEY_PAGE_DOWN;
			case Key::Home:           return GLFW_KEY_HOME;
			case Key::End:            return GLFW_KEY_END;
			case Key::CapsLock:       return GLFW_KEY_CAPS_LOCK;
			case Key::ScrollLock:     return GLFW_KEY_SCROLL_LOCK;
			case Key::NumLock:        return GLFW_KEY_NUM_LOCK;
			case Key::PrintScreen:    return GLFW_KEY_PRINT_SCREEN;
			case Key::Pause:          return GLFW_KEY_PAUSE;

			case Key::F1:             return GLFW_KEY_F1;
			case Key::F2:             return GLFW_KEY_F2;
			case Key::F3:             return GLFW_KEY_F3;
			case Key::F4:             return GLFW_KEY_F4;
			case Key::F5:             return GLFW_KEY_F5;
			case Key::F6:             return GLFW_KEY_F6;
			case Key::F7:             return GLFW_KEY_F7;
			case Key::F8:             return GLFW_KEY_F8;
			case Key::F9:             return GLFW_KEY_F9;
			case Key::F10:            return GLFW_KEY_F10;
			case Key::F11:            return GLFW_KEY_F11;
			case Key::F12:            return GLFW_KEY_F12;

			case Key::KP_0:           return GLFW_KEY_KP_0;
			case Key::KP_1:           return GLFW_KEY_KP_1;
			case Key::KP_2:           return GLFW_KEY_KP_2;
			case Key::KP_3:           return GLFW_KEY_KP_3;
			case Key::KP_4:           return GLFW_KEY_KP_4;
			case Key::KP_5:           return GLFW_KEY_KP_5;
			case Key::KP_6:           return GLFW_KEY_KP_6;
			case Key::KP_7:           return GLFW_KEY_KP_7;
			case Key::KP_8:           return GLFW_KEY_KP_8;
			case Key::KP_9:           return GLFW_KEY_KP_9;
			case Key::KP_Decimal:     return GLFW_KEY_KP_DECIMAL;
			case Key::KP_Divide:      return GLFW_KEY_KP_DIVIDE;
			case Key::KP_Multiply:    return GLFW_KEY_KP_MULTIPLY;
			case Key::KP_Subtract:    return GLFW_KEY_KP_SUBTRACT;
			case Key::KP_Add:         return GLFW_KEY_KP_ADD;
			case Key::KP_Enter:       return GLFW_KEY_KP_ENTER;
			case Key::KP_Equal:       return GLFW_KEY_KP_EQUAL;

			case Key::LeftShift:      return GLFW_KEY_LEFT_SHIFT;
			case Key::LeftControl:    return GLFW_KEY_LEFT_CONTROL;
			case Key::LeftAlt:        return GLFW_KEY_LEFT_ALT;
			case Key::LeftSuper:      return GLFW_KEY_LEFT_SUPER;
			case Key::RightShift:     return GLFW_KEY_RIGHT_SHIFT;
			case Key::RightControl:   return GLFW_KEY_RIGHT_CONTROL;
			case Key::RightAlt:       return GLFW_KEY_RIGHT_ALT;
			case Key::RightSuper:     return GLFW_KEY_RIGHT_SUPER;
			case Key::Menu:           return GLFW_KEY_MENU;

			default:                  return GLFW_KEY_UNKNOWN;
		}
	}

	int Input::MapMouseButton(MouseButton mouseButton)
	{
		switch (mouseButton)
		{
			case MouseButton::Left: return GLFW_MOUSE_BUTTON_LEFT;
			case MouseButton::Right: return GLFW_MOUSE_BUTTON_RIGHT;
			case MouseButton::Middle: return GLFW_MOUSE_BUTTON_MIDDLE;
			default: return GLFW_MOUSE_BUTTON_LEFT;
		}
	}

#pragma endregion
}