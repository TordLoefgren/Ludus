#pragma once

#include <cstdint>

namespace Ludus::Engine::Core
{
	using EntityHandle = uint32_t;

	struct Entity
	{
	private:
		inline static EntityHandle s_NextHandle = 1;

	public:
		EntityHandle Handle;

		Entity()
			: Handle(s_NextHandle++)
		{ }

		Entity(const Entity&) = delete;
		Entity& operator=(const Entity&) = delete;
		Entity(Entity&&) noexcept = default;
		Entity& operator=(Entity&&) noexcept = default;
		~Entity() = default;

		bool operator==(const Entity& other) const { return Handle == other.Handle; }
	};
}
