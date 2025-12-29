#pragma once

#include <string>

#include <Ludus/Engine/Core/Entity.h>

namespace Ludus::Engine::Components
{
	struct DisplayNameComponent
	{
	public:
		Ludus::Engine::Core::EntityHandle OwnerHandle;
		std::string Value;

		DisplayNameComponent(
			Ludus::Engine::Core::EntityHandle owner,
			std::string value
		) :
			OwnerHandle(owner),
			Value(value)
		{ }

		~DisplayNameComponent() = default;

		bool operator==(const DisplayNameComponent& other) const { return OwnerHandle == other.OwnerHandle; }
	};
}
