#pragma once

#include <set>

#include <Ludus/Engine/Events/EventHandler.h>
#include <Ludus/Engine/Math/Vector2D.h>
#include <Ludus/Engine/Windowing/Key.h>
#include <Ludus/Engine/Windowing/MouseButton.h>

namespace Ludus::Engine::Events
{
	struct Event;
}

namespace Ludus::Engine::Windowing
{
	class Input : public Ludus::Engine::Events::EventHandler
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

		float m_MouseXDelta;
		float m_MouseYDelta;
		float m_MouseXPosition;
		float m_MouseYPosition;
		float m_MouseXScrollOffset;
		float m_MouseYScrollOffset;

		bool m_HasMouseDelta;

		Key MapKey(int key);
		int MapKey(Key key);

		MouseButton MapMouseButton(int mouseButton);
		int MapMouseButton(MouseButton mouseButton);

	public:
		Input();
		~Input() = default;

		void Clear();

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

		const Ludus::Engine::Math::Vector2D GetMouseDelta() { return { m_MouseXDelta, m_MouseYDelta }; }
		const Ludus::Engine::Math::Vector2D GetMousePosition() { return { m_MouseXPosition, m_MouseYPosition }; }
		const Ludus::Engine::Math::Vector2D GetMouseScrollOffset() { return { m_MouseXScrollOffset, m_MouseYScrollOffset }; }

		virtual bool ProcessEvent(const Ludus::Engine::Events::Event& event) override;
	};
}
