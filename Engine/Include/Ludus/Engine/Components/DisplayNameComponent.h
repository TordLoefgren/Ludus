#pragma once

#include <string>
#include <utility>

#include <Ludus/Engine/Core/Id.h>

namespace Ludus::Engine::Components
{
	struct DisplayNameComponent
	{
	public:
		Ludus::Engine::Core::EntityId OwnerId { Ludus::Engine::Core::EntityId::Invalid() };
		std::string Name = "";

		DisplayNameComponent() = default;

		explicit DisplayNameComponent(
			std::string name
		) :
			Name(std::move(name))
		{ }

		DisplayNameComponent(
			Ludus::Engine::Core::EntityId owner,
			std::string name = ""
		) :
			OwnerId(owner),
			Name(std::move(name))
		{ }

		~DisplayNameComponent() = default;

		bool operator==(const DisplayNameComponent& other) const { return OwnerId == other.OwnerId; }
	};
}
