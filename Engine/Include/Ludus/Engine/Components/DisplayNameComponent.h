#pragma once

#include <string>

#include <Ludus/Engine/Core/Entity.h>

namespace Ludus::Engine::Components
{
	struct DisplayNameComponent
	{
	public:
		Ludus::Engine::Core::EntityHandle OwnerHandle {};
		std::string Value = "";

		DisplayNameComponent() = default;

		explicit DisplayNameComponent(
			std::string value
		) :
			Value(std::move(value))
		{ }

		DisplayNameComponent(
			Ludus::Engine::Core::EntityHandle owner,
			std::string value = ""
		) :
			OwnerHandle(owner),
			Value(std::move(value))
		{ }

		~DisplayNameComponent() = default;

		bool operator==(const DisplayNameComponent& other) const { return OwnerHandle == other.OwnerHandle; }
	};
}
