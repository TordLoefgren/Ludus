#pragma once

#include <string>
#include <string_view>

#include <Ludus/Engine/Core/Entity.h>

namespace Ludus::Engine::Components
{
	using ScriptHandle = uint64_t;

	struct ScriptComponent
	{
	public:
		Ludus::Engine::Core::EntityHandle OwnerHandle {};
		std::string Name = "";
		ScriptHandle Handle {};

		ScriptComponent() = default;

		explicit ScriptComponent(
			std::string_view name,
			ScriptHandle handle = {}
		) :
			Name(name),
			Handle(handle)
		{ }

		ScriptComponent(
			Ludus::Engine::Core::EntityHandle ownerHandle,
			std::string_view name = "",
			ScriptHandle handle = {}
		) :
			OwnerHandle(ownerHandle),
			Name(name),
			Handle(handle == ScriptHandle {} ? static_cast<ScriptHandle>(ownerHandle) : handle)
		{ }

		~ScriptComponent() = default;

		bool operator==(const ScriptComponent& other) const { return Handle == other.Handle; }
	};
}
