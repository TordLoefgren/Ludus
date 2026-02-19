#include "pch.h"

#include <cmath>

#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Windowing/Input.h>

namespace Ludus::Engine::Windowing
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
		m_MouseXDelta(),
		m_MouseYDelta(),
		m_MouseXPosition(),
		m_MouseYPosition(),
		m_MouseXScrollOffset(),
		m_MouseYScrollOffset(),
		m_HasMouseDelta(false)
	{ }

	void Input::Clear()
	{
		m_RepeatedKeys.clear();
		m_JustPressedKeys.clear();
		m_JustReleasedKeys.clear();

		m_RepeatedMouseButtons.clear();
		m_JustPressedMouseButtons.clear();
		m_JustReleasedMouseButtons.clear();

		m_MouseXDelta = 0.0f;
		m_MouseYDelta = 0.0f;
		m_MouseXScrollOffset = 0.0f;
		m_MouseYScrollOffset = 0.0f;
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
		const auto x = static_cast<float>(xPosition);
		const auto y = static_cast<float>(yPosition);

		// Avoid large delta on first capture.
		if (m_HasMouseDelta)
		{
			m_MouseXDelta += m_MouseXPosition - x;
			m_MouseYDelta += m_MouseYPosition - y;
		}
		else
		{
			m_HasMouseDelta = true;
		}

		m_MouseXPosition = x;
		m_MouseYPosition = y;
	}

	void Input::OnMouseScroll(double xOffset, double yOffset)
	{
		m_MouseXScrollOffset += static_cast<float>(xOffset);
		m_MouseYScrollOffset += static_cast<float>(yOffset);
	}

	void Input::OnMouseEnter(bool entered)
	{
		// Not yet implemented. 
	}

	bool Input::ProcessEvent(const Ludus::Engine::Events::Event& event)
	{
		namespace KeyboardEvents = Ludus::Engine::Events::KeyboardEvents;
		namespace MouseEvents = Ludus::Engine::Events::MouseEvents;

		using EventType = Ludus::Engine::Events::EventType;

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
		return static_cast<int>(key);
	}

	Key Input::MapKey(int key)
	{
		if (key == GLFW_KEY_UNKNOWN)
		{
			return Key::Unknown;
		}

		if (key >= GLFW_KEY_SPACE && key <= GLFW_KEY_LAST)
		{
			return static_cast<Key>(key);
		}

		return Key::Unknown;
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