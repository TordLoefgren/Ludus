#pragma once

#include <compare>
#include <cstdint>
#include <functional>

namespace Ludus::Engine::Core
{
	template<typename TTag>
	struct Id
	{
		std::uint64_t Value = 0;

		static constexpr Id Invalid()
		{
			return { };
		}

		constexpr bool IsValid() const
		{
			return Value != 0;
		}

		auto operator<=>(const Id&) const = default;
	};

	struct EntityIdTag;
	struct SceneIdTag;
	struct ScriptIdTag;
	struct AssetIdTag;

	using EntityId = Id<EntityIdTag>;
	using SceneId = Id<SceneIdTag>;
	using ScriptId = Id<ScriptIdTag>;
	using AssetId = Id<AssetIdTag>;
}

template<>
struct std::hash<Ludus::Engine::Core::EntityId>
{
	std::size_t operator()(const Ludus::Engine::Core::EntityId& id) const noexcept
	{
		return std::hash<std::uint64_t> {}(id.Value);
	}
};

template<>
struct std::hash<Ludus::Engine::Core::SceneId>
{
	std::size_t operator()(const Ludus::Engine::Core::SceneId& id) const noexcept
	{
		return std::hash<std::uint64_t> {}(id.Value);
	}
};

template<>
struct std::hash<Ludus::Engine::Core::ScriptId>
{
	std::size_t operator()(const Ludus::Engine::Core::ScriptId& id) const noexcept
	{
		return std::hash<std::uint64_t> {}(id.Value);
	}
};

template<>
struct std::hash<Ludus::Engine::Core::AssetId>
{
	std::size_t operator()(const Ludus::Engine::Core::AssetId& id) const noexcept
	{
		return std::hash<std::uint64_t> {}(id.Value);
	}
};
