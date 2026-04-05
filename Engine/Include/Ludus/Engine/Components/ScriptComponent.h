#pragma once

#include <Ludus/Engine/Core/Id.h>

namespace Ludus::Engine::Components
{
	struct ScriptComponent
	{
	public:
		Ludus::Engine::Core::EntityId OwnerId { Ludus::Engine::Core::EntityId::Invalid() };
		Ludus::Engine::Core::ScriptId Id { Ludus::Engine::Core::ScriptId::Invalid() };

		ScriptComponent() = default;

		explicit ScriptComponent(Ludus::Engine::Core::ScriptId id)
			: Id(id)
		{ }

		ScriptComponent(
			Ludus::Engine::Core::EntityId ownerId,
			Ludus::Engine::Core::ScriptId id = Ludus::Engine::Core::ScriptId::Invalid()
		) :
			OwnerId(ownerId),
			Id(id)
		{ }

		~ScriptComponent() = default;

		bool operator==(const ScriptComponent& other) const { return Id == other.Id; }
	};
}
