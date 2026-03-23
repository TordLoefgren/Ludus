#pragma once

#include <cstdint>
#include <functional>
#include <variant>

#include <Ludus/Engine/Core/Entity.h>

namespace Ludus::Editor::Commands
{
	struct TempReference
	{
		std::uint64_t Value { };

		bool operator==(const TempReference&) const = default;
	};

	struct EntityReference
	{
		struct New { TempReference Temp; };
		std::variant<Ludus::Engine::Core::EntityHandle, New> Value;

		EntityReference() = default;
		EntityReference(Ludus::Engine::Core::EntityHandle handle)
			: Value(handle)
		{ }

		static EntityReference Temporary(TempReference temp)
		{
			return EntityReference(New { temp });
		}

	private:
		explicit EntityReference(New created)
			: Value(created)
		{ }
	};
}

template<>
struct std::hash<Ludus::Editor::Commands::TempReference>
{
	std::size_t operator()(const Ludus::Editor::Commands::TempReference& value) const noexcept
	{
		return std::hash<std::uint64_t> {}(value.Value);
	}
};
