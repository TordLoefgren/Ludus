#pragma once

#include <array>
#include <cstdint>
#include <string_view>

#include <Ludus/Engine/Physics/Core/LayerMask.h>

namespace Ludus::Pong::Core::Layers
{
	enum class Layer : std::uint8_t
	{
		Ball = 1,
		BoundaryHorizontal = 2,
		BoundaryVertical = 3,
		Player1 = 4,
		Player2 = 5
	};

	struct LayerInfo
	{
		Layer Layer;
		Ludus::Engine::Physics::Core::LayerIndex Index;
		std::string_view Name;
	};

	inline constexpr std::array<LayerInfo, 5> LayerLookupTable {
		{
			{ Layer::Ball,               static_cast<Ludus::Engine::Physics::Core::LayerIndex>(1), "Ball" },
			{ Layer::BoundaryHorizontal, static_cast<Ludus::Engine::Physics::Core::LayerIndex>(2), "BoundaryHorizontal" },
			{ Layer::BoundaryVertical,   static_cast<Ludus::Engine::Physics::Core::LayerIndex>(3), "BoundaryVertical" },
			{ Layer::Player1,            static_cast<Ludus::Engine::Physics::Core::LayerIndex>(4), "Player1" },
			{ Layer::Player2,            static_cast<Ludus::Engine::Physics::Core::LayerIndex>(5), "Player2" },
		}
	};

	inline constexpr Ludus::Engine::Physics::Core::LayerIndex ToIndex(Layer layer)
	{
		for (auto&& info : LayerLookupTable)
		{
			if (info.Layer == layer)
			{
				return info.Index;
			}
		}

		return 0;
	}

	inline constexpr std::string_view ToName(Layer layer)
	{
		for (auto&& info : LayerLookupTable)
		{
			if (info.Layer == layer)
			{
				return info.Name;
			}
		}

		return "";
	}

	inline constexpr Layer FromIndex(Ludus::Engine::Physics::Core::LayerIndex index)
	{
		for (auto&& info : LayerLookupTable)
		{
			if (info.Index == index)
			{
				return info.Layer;
			}
		}

		return static_cast<Layer>(0);
	}

	inline constexpr Layer FromName(std::string_view name)
	{
		for (auto&& info : LayerLookupTable)
		{
			if (info.Name == name)
			{
				return info.Layer;
			}
		}

		return static_cast<Layer>(0);
	}

	inline void RegisterAllLayers()
	{
		for (auto&& info : LayerLookupTable)
		{
			Ludus::Engine::Physics::Core::LayerMask::AddLayer(std::string { info.Name }, info.Index);
		}
	}
}
