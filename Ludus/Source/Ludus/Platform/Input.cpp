#include <Ludus/Platform/Input.h>

namespace Ludus::Platform
{
	Input::Input()
		:
		m_PressedKeys(),
		m_RepeatedKeys(),
		m_JustPressedKeys(),
		m_JustReleasedKeys(),
		m_PressedMouseButtons(),
		m_RepeatedMouseButtons(),
		m_JustPressedMouseButtons(),
		m_JustReleasedMouseButtons(),
		m_MouseXPosition(),
		m_MouseYPosition(),
		m_MouseScrollXOffset(),
		m_MouseScrollYOffset()
	{ }

	void Input::Clear()
	{
		m_RepeatedKeys.clear();
		m_JustPressedKeys.clear();
		m_JustReleasedKeys.clear();

		m_RepeatedMouseButtons.clear();
		m_JustPressedMouseButtons.clear();
		m_JustReleasedMouseButtons.clear();

		m_MouseScrollXOffset = 0.0f;
		m_MouseScrollYOffset = 0.0f;
	}

#pragma region Callback listeners

	void Input::OnKey(int key, int scancode, int action, int mods)
	{
		const auto mappedKey = MapKey(key);

		switch (action)
		{
			case GLFW_PRESS:
				m_PressedKeys.insert(mappedKey);
				m_JustPressedKeys.insert(mappedKey);
				break;
			case GLFW_REPEAT:
				if (m_PressedKeys.contains(mappedKey))
				{
					m_RepeatedKeys.insert(mappedKey);
				}
				break;
			case GLFW_RELEASE:
				if (auto iterPressed = m_PressedKeys.find(mappedKey); iterPressed != m_PressedKeys.end())
				{
					m_PressedKeys.erase(iterPressed);
				}
				m_JustReleasedKeys.insert(mappedKey);

				if (auto iterRepeated = m_RepeatedKeys.find(mappedKey); iterRepeated != m_RepeatedKeys.end())
				{
					m_RepeatedKeys.erase(iterRepeated);
				}
				break;
		}
	}

	void Input::OnText(unsigned int codepoint)
	{
		// Not yet implemented. 
	}

	void Input::OnMouseButton(int button, int action, int mods)
	{
		const auto mappedMouseButton = MapMouseButton(button);

		switch (action)
		{
			case GLFW_PRESS:
				m_PressedMouseButtons.insert(mappedMouseButton);
				m_JustPressedMouseButtons.insert(mappedMouseButton);
				break;
			case GLFW_REPEAT:
				if (m_PressedMouseButtons.contains(mappedMouseButton))
				{
					m_RepeatedMouseButtons.insert(mappedMouseButton);
				}
				break;
			case GLFW_RELEASE:
				if (auto iterPressed = m_PressedMouseButtons.find(mappedMouseButton); iterPressed != m_PressedMouseButtons.end())
				{
					m_PressedMouseButtons.erase(iterPressed);
				}
				m_JustReleasedMouseButtons.insert(mappedMouseButton);

				if (auto iterRepeated = m_RepeatedMouseButtons.find(mappedMouseButton); iterRepeated != m_RepeatedMouseButtons.end())
				{
					m_RepeatedMouseButtons.erase(iterRepeated);
				}
				break;
		}
	}

	void Input::OnMouseMove(double xPosition, double yPosition)
	{
		m_MouseXPosition = static_cast<float>(xPosition);
		m_MouseYPosition = static_cast<float>(yPosition);
	}

	void Input::OnMouseScroll(double xOffset, double yOffset)
	{
		m_MouseScrollXOffset = static_cast<float>(xOffset);
		m_MouseScrollYOffset = static_cast<float>(yOffset);
	}

	void Input::OnMouseEnter(bool entered)
	{
		// Not yet implemented. 
	}

	bool Input::ProcessEvent(const Ludus::Events::Event& event)
	{
		namespace KeyboardEvents = Ludus::Events::KeyboardEvents;
		namespace MouseEvents = Ludus::Events::MouseEvents;

		using EventType = Ludus::Events::EventType;

		switch (event.Type)
		{
			case EventType::KeyEvent:
			{
				const auto& e = static_cast<const KeyboardEvents::KeyEvent&>(event);
				OnKey(e.Key, e.Scancode, e.Action, e.Mods);
				break;
			}

			case EventType::TextInputEvent:
			{
				const auto& e = static_cast<const KeyboardEvents::TextInputEvent&>(event);
				OnText(e.Codepoint);
				break;
			}

			case EventType::MouseButtonEvent:
			{
				const auto& e = static_cast<const MouseEvents::MouseButtonEvent&>(event);
				OnMouseButton(e.Button, e.Action, e.Mods);
				break;
			}

			case EventType::MouseMoveEvent:
			{
				const auto& e = static_cast<const MouseEvents::MouseMoveEvent&>(event);
				OnMouseMove(e.XPosition, e.YPosition);
				break;
			}

			case EventType::MouseScrollEvent:
			{
				const auto& e = static_cast<const MouseEvents::MouseScrollEvent&>(event);
				OnMouseScroll(e.XOffset, e.YOffset);
				break;
			}

			case EventType::MouseEnterEvent:
			{
				const auto& e = static_cast<const MouseEvents::MouseEnterEvent&>(event);
				OnMouseEnter(e.Entered);
				break;
			}

			default: return false;
		}

		return true;
	}

#pragma endregion


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

	Key Input::MapKey(int key)
	{
		switch (key)
		{
			case GLFW_KEY_UNKNOWN:        return Key::Unknown;

			case GLFW_KEY_SPACE:          return Key::Space;
			case GLFW_KEY_APOSTROPHE:     return Key::Apostrophe;
			case GLFW_KEY_COMMA:          return Key::Comma;
			case GLFW_KEY_MINUS:          return Key::Minus;
			case GLFW_KEY_PERIOD:         return Key::Period;
			case GLFW_KEY_SLASH:          return Key::Slash;

			case GLFW_KEY_0:              return Key::D0;
			case GLFW_KEY_1:              return Key::D1;
			case GLFW_KEY_2:              return Key::D2;
			case GLFW_KEY_3:              return Key::D3;
			case GLFW_KEY_4:              return Key::D4;
			case GLFW_KEY_5:              return Key::D5;
			case GLFW_KEY_6:              return Key::D6;
			case GLFW_KEY_7:              return Key::D7;
			case GLFW_KEY_8:              return Key::D8;
			case GLFW_KEY_9:              return Key::D9;

			case GLFW_KEY_SEMICOLON:      return Key::Semicolon;
			case GLFW_KEY_EQUAL:          return Key::Equal;

			case GLFW_KEY_A:              return Key::A;
			case GLFW_KEY_B:              return Key::B;
			case GLFW_KEY_C:              return Key::C;
			case GLFW_KEY_D:              return Key::D;
			case GLFW_KEY_E:              return Key::E;
			case GLFW_KEY_F:              return Key::F;
			case GLFW_KEY_G:              return Key::G;
			case GLFW_KEY_H:              return Key::H;
			case GLFW_KEY_I:              return Key::I;
			case GLFW_KEY_J:              return Key::J;
			case GLFW_KEY_K:              return Key::K;
			case GLFW_KEY_L:              return Key::L;
			case GLFW_KEY_M:              return Key::M;
			case GLFW_KEY_N:              return Key::N;
			case GLFW_KEY_O:              return Key::O;
			case GLFW_KEY_P:              return Key::P;
			case GLFW_KEY_Q:              return Key::Q;
			case GLFW_KEY_R:              return Key::R;
			case GLFW_KEY_S:              return Key::S;
			case GLFW_KEY_T:              return Key::T;
			case GLFW_KEY_U:              return Key::U;
			case GLFW_KEY_V:              return Key::V;
			case GLFW_KEY_W:              return Key::W;
			case GLFW_KEY_X:              return Key::X;
			case GLFW_KEY_Y:              return Key::Y;
			case GLFW_KEY_Z:              return Key::Z;

			case GLFW_KEY_LEFT_BRACKET:   return Key::LeftBracket;
			case GLFW_KEY_BACKSLASH:      return Key::Backslash;
			case GLFW_KEY_RIGHT_BRACKET:  return Key::RightBracket;
			case GLFW_KEY_GRAVE_ACCENT:   return Key::GraveAccent;

			case GLFW_KEY_ESCAPE:         return Key::Escape;
			case GLFW_KEY_ENTER:          return Key::Enter;
			case GLFW_KEY_TAB:            return Key::Tab;
			case GLFW_KEY_BACKSPACE:      return Key::Backspace;
			case GLFW_KEY_INSERT:         return Key::Insert;
			case GLFW_KEY_DELETE:         return Key::DeleteKey;
			case GLFW_KEY_RIGHT:          return Key::Right;
			case GLFW_KEY_LEFT:           return Key::Left;
			case GLFW_KEY_DOWN:           return Key::Down;
			case GLFW_KEY_UP:             return Key::Up;
			case GLFW_KEY_PAGE_UP:        return Key::PageUp;
			case GLFW_KEY_PAGE_DOWN:      return Key::PageDown;
			case GLFW_KEY_HOME:           return Key::Home;
			case GLFW_KEY_END:            return Key::End;
			case GLFW_KEY_CAPS_LOCK:      return Key::CapsLock;
			case GLFW_KEY_SCROLL_LOCK:    return Key::ScrollLock;
			case GLFW_KEY_NUM_LOCK:       return Key::NumLock;
			case GLFW_KEY_PRINT_SCREEN:   return Key::PrintScreen;
			case GLFW_KEY_PAUSE:          return Key::Pause;

			case GLFW_KEY_F1:             return Key::F1;
			case GLFW_KEY_F2:             return Key::F2;
			case GLFW_KEY_F3:             return Key::F3;
			case GLFW_KEY_F4:             return Key::F4;
			case GLFW_KEY_F5:             return Key::F5;
			case GLFW_KEY_F6:             return Key::F6;
			case GLFW_KEY_F7:             return Key::F7;
			case GLFW_KEY_F8:             return Key::F8;
			case GLFW_KEY_F9:             return Key::F9;
			case GLFW_KEY_F10:            return Key::F10;
			case GLFW_KEY_F11:            return Key::F11;
			case GLFW_KEY_F12:            return Key::F12;

			case GLFW_KEY_KP_0:           return Key::KP_0;
			case GLFW_KEY_KP_1:           return Key::KP_1;
			case GLFW_KEY_KP_2:           return Key::KP_2;
			case GLFW_KEY_KP_3:           return Key::KP_3;
			case GLFW_KEY_KP_4:           return Key::KP_4;
			case GLFW_KEY_KP_5:           return Key::KP_5;
			case GLFW_KEY_KP_6:           return Key::KP_6;
			case GLFW_KEY_KP_7:           return Key::KP_7;
			case GLFW_KEY_KP_8:           return Key::KP_8;
			case GLFW_KEY_KP_9:           return Key::KP_9;
			case GLFW_KEY_KP_DECIMAL:     return Key::KP_Decimal;
			case GLFW_KEY_KP_DIVIDE:      return Key::KP_Divide;
			case GLFW_KEY_KP_MULTIPLY:    return Key::KP_Multiply;
			case GLFW_KEY_KP_SUBTRACT:    return Key::KP_Subtract;
			case GLFW_KEY_KP_ADD:         return Key::KP_Add;
			case GLFW_KEY_KP_ENTER:       return Key::KP_Enter;
			case GLFW_KEY_KP_EQUAL:       return Key::KP_Equal;

			case GLFW_KEY_LEFT_SHIFT:     return Key::LeftShift;
			case GLFW_KEY_LEFT_CONTROL:   return Key::LeftControl;
			case GLFW_KEY_LEFT_ALT:       return Key::LeftAlt;
			case GLFW_KEY_LEFT_SUPER:     return Key::LeftSuper;
			case GLFW_KEY_RIGHT_SHIFT:    return Key::RightShift;
			case GLFW_KEY_RIGHT_CONTROL:  return Key::RightControl;
			case GLFW_KEY_RIGHT_ALT:      return Key::RightAlt;
			case GLFW_KEY_RIGHT_SUPER:    return Key::RightSuper;
			case GLFW_KEY_MENU:           return Key::Menu;

			default:                      return Key::Unknown;
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

	MouseButton Input::MapMouseButton(int mouseButton)
	{
		switch (mouseButton)
		{
			case GLFW_MOUSE_BUTTON_LEFT: return MouseButton::Left;
			case GLFW_MOUSE_BUTTON_RIGHT: return MouseButton::Right;
			case GLFW_MOUSE_BUTTON_MIDDLE: return MouseButton::Middle;
			default: return MouseButton::Left;
		}
	}

#pragma endregion

}