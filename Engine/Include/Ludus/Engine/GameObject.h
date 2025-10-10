#pragma once

#include <cstdint>

namespace Ludus::Engine
{
	using GameObjectHandle = uint32_t;

	struct GameObject
	{
	private:
		inline static GameObjectHandle s_NextHandle = 1;

	public:
		GameObjectHandle Handle;

		GameObject()
			: Handle(s_NextHandle++)
		{ }

		GameObject(const GameObject&) = delete;
		GameObject& operator=(const GameObject&) = delete;
		GameObject(GameObject&&) noexcept = default;
		GameObject& operator=(GameObject&&) noexcept = default;
		~GameObject() = default;

		bool operator==(const GameObject& other) const { return Handle == other.Handle; }
	};
}
