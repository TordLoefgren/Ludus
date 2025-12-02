#pragma once

#include <array>
#include <cstdint>
#include <string_view>

#include <Ludus/Engine/LayerMask.h>

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
		Ludus::Engine::LayerIndex Index;
		std::string_view Name;
	};

	inline constexpr std::array<LayerInfo, 5> LayerLookupTable {
		{
			{ Layer::Ball,               static_cast<Ludus::Engine::LayerIndex>(1), "Ball" },
			{ Layer::BoundaryHorizontal, static_cast<Ludus::Engine::LayerIndex>(2), "BoundaryHorizontal" },
			{ Layer::BoundaryVertical,   static_cast<Ludus::Engine::LayerIndex>(3), "BoundaryVertical" },
			{ Layer::Player1,            static_cast<Ludus::Engine::LayerIndex>(4), "Player1" },
			{ Layer::Player2,            static_cast<Ludus::Engine::LayerIndex>(5), "Player2" },
		}
	};

	inline constexpr Ludus::Engine::LayerIndex ToIndex(Layer layer)
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

	inline constexpr Layer FromIndex(Ludus::Engine::LayerIndex index)
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
			Ludus::Engine::LayerMask::AddLayer(std::string { info.Name }, info.Index);
		}
	}
}
