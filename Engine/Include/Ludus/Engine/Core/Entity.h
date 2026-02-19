#pragma once

#include <cstdint>

namespace Ludus::Engine::Core
{
	using EntityHandle = uint64_t;

	struct Entity
	{
	public:
		EntityHandle Handle;

		Entity(EntityHandle handle)
			: Handle(handle)
		{ }

		Entity(const Entity&) = default;
		Entity& operator=(const Entity&) = default;
		Entity(Entity&&) noexcept = default;
		Entity& operator=(Entity&&) noexcept = default;
		~Entity() = default;

		bool operator==(const Entity& other) const { return Handle == other.Handle; }
	};
}
