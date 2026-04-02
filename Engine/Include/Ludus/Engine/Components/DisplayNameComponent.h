#pragma once

#include <string>
#include <utility>

#include <Ludus/Engine/Core/Entity.h>

namespace Ludus::Engine::Components
{
	struct DisplayNameComponent
	{
	public:
		Ludus::Engine::Core::EntityHandle OwnerHandle { };
		std::string Name = "";

		DisplayNameComponent() = default;

		explicit DisplayNameComponent(
			std::string name
		) :
			Name(std::move(name))
		{ }

		DisplayNameComponent(
			Ludus::Engine::Core::EntityHandle owner,
			std::string name = ""
		) :
			OwnerHandle(owner),
			Name(std::move(name))
		{ }

		~DisplayNameComponent() = default;

		bool operator==(const DisplayNameComponent& other) const { return OwnerHandle == other.OwnerHandle; }
	};
}
