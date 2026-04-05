#pragma once

#include <Ludus/Engine/Core/Id.h>

namespace Ludus::Engine::Core
{
	struct Entity
	{
	public:
		EntityId Id { EntityId::Invalid() };

		Entity() = default;

		Entity(EntityId id) : Id(id)
		{ }

		Entity(const Entity&) = default;
		Entity& operator=(const Entity&) = default;
		Entity(Entity&&) noexcept = default;
		Entity& operator=(Entity&&) noexcept = default;
		~Entity() = default;

		bool operator==(const Entity& other) const { return Id == other.Id; }
	};
}

