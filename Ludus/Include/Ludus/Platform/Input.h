#pragma once

#include <GLFW/glfw3.h>
#include <set>

#include <Ludus/Math/Vector2D.h>
#include <Ludus/Platform/Key.h>

namespace Ludus::Platform
{
	class Input
	{
	private:
		GLFWwindow* m_Handle;
		std::set<Key> m_PressedKeys;
		std::set<MouseButton> m_PressedButtons;

		int MapKey(Key key);
		int MapMouseButton(MouseButton mouseButton);

	public:
		Input();
		~Input();

		void SetHandle(GLFWwindow* handle);

		bool GetKey(Key key);
		bool GetKeyDown(Key key);

		bool GetMouseButton(MouseButton mouseButton);
		bool GetMouseButtonDown(MouseButton mouseButton);

		const Ludus::Math::Vector2D GetMousePosition();
	};
}
