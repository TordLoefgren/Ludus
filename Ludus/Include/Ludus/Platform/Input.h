#pragma once

#include <GLFW/glfw3.h>
#include <set>
#include <vector>

#include <Ludus/Events/Event.h>
#include <Ludus/Events/EventHandler.h>
#include <Ludus/Events/EventType.h>
#include <Ludus/Events/KeyboardEvents.h>
#include <Ludus/Events/MouseEvents.h>
#include <Ludus/Math/Vector2D.h>
#include <Ludus/Platform/Key.h>

namespace Ludus::Platform
{
	class Input : Ludus::Events::Eventhandler
	{
	private:
		std::set<Key> m_PressedKeys;
		std::set<Key> m_RepeatedKeys;
		std::set<Key> m_JustPressedKeys;
		std::set<Key> m_JustReleasedKeys;

		std::set<MouseButton> m_PressedMouseButtons;
		std::set<MouseButton> m_RepeatedMouseButtons;
		std::set<MouseButton> m_JustPressedMouseButtons;
		std::set<MouseButton> m_JustReleasedMouseButtons;

		float m_MouseXPosition;
		float m_MouseYPosition;
		float m_MouseScrollXOffset;
		float m_MouseScrollYOffset;

		Key MapKey(int key);
		int MapKey(Key key);

		MouseButton MapMouseButton(int mouseButton);
		int MapMouseButton(MouseButton mouseButton);

	public:
		Input();
		~Input() = default;

		void Clear();
		void SetHandle(GLFWwindow* handle);

		void OnKey(int key, int scancode, int action, int mods);
		void OnText(unsigned int codepoint);
		void OnMouseButton(int button, int action, int mods);
		void OnMouseMove(double xPosition, double yPosition);
		void OnMouseScroll(double xOffset, double yOffset);
		void OnMouseEnter(bool entered);

		bool GetKey(Key key) { return m_PressedKeys.contains(key); }
		bool GetKeyDown(Key key) { return m_JustPressedKeys.contains(key); }
		bool GetKeyUp(Key key) { return m_JustReleasedKeys.contains(key); }

		bool GetMouseButton(MouseButton mouseButton) { return m_PressedMouseButtons.contains(mouseButton); }
		bool GetMouseButtonDown(MouseButton mouseButton) { return m_JustPressedMouseButtons.contains(mouseButton); }
		bool GetMouseButtonUp(MouseButton mouseButton) { return m_JustReleasedMouseButtons.contains(mouseButton); }

		const Ludus::Math::Vector2D GetMousePosition() { return { m_MouseXPosition, m_MouseYPosition }; }

		virtual bool ProcessEvent(const Ludus::Events::Event& event) override;
	};
}
