#pragma once

#include <algorithm>
#include <bit>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <Ludus/Engine/Debug/Debug.h>

namespace Ludus::Engine::Physics::Core
{
	using LayerIndex = uint8_t;
	using LayerBits = uint32_t;

	inline LayerIndex s_MaxLayers = 32u;

	inline std::unordered_map<std::string, uint8_t> s_NameToIndex = { { "Default", 0 }, { "UI", 1 } };
	inline std::unordered_map<uint8_t, std::string> s_IndexToName = { { 0, "Default" }, { 1, "UI" } };

	struct LayerMask
	{
		struct LayerEntry
		{
			LayerIndex Index;
			std::string_view Name;
		};

		uint32_t Value { 0u };

		bool Any()  const { return Value != 0; }
		bool None() const { return Value == 0; }
		bool Contains(LayerIndex index) const { return (Value & (1u << index)) != 0; }
		bool Contains(LayerMask other) const { return (Value & other.Value) == other.Value; }

#pragma region Operators

		operator LayerBits() const { return Value; }

		bool operator==(const LayerMask& other) const
		{
			return Value == other.Value;
		}

		LayerMask operator&(const LayerMask& other) const
		{
			return LayerMask(Value & other.Value);
		}

		LayerMask operator|(const LayerMask& other) const
		{
			return LayerMask(Value | other.Value);
		}

		LayerMask operator^(const LayerMask& other) const
		{
			return LayerMask(Value ^ other.Value);
		}

		LayerMask operator~() const
		{
			return LayerMask(~Value);
		}

		LayerMask& operator&=(const LayerMask& other)
		{
			Value &= other.Value;
			return *this;
		}

		LayerMask& operator|=(const LayerMask& other)
		{
			Value |= other.Value;
			return *this;
		}

		LayerMask& operator^=(const LayerMask& other)
		{
			Value ^= other.Value;
			return *this;
		}

#pragma endregion

		static LayerMask GetEmpty() { return LayerMask(0u); }

		static LayerMask FromIndex(LayerIndex index)
		{
			LUDUS_ASSERT(index < s_MaxLayers, "Index must be smaller than 32.");

			if (index >= s_MaxLayers)
			{
				return GetEmpty();
			}

			return LayerMask(1u << index);
		}

		static LayerMask FromIndices(std::initializer_list<LayerIndex> indices)
		{
			LayerMask mask = GetEmpty();
			for (auto index : indices)
			{
				mask |= FromIndex(index);
			}

			return mask;
		}

		static void AddLayer(const std::string& layerName, const LayerIndex layerIndex)
		{
			LUDUS_ASSERT(layerIndex > 0u && layerIndex < s_MaxLayers, "Index must be between 1 and 31, inclusive.");

			auto iterName = s_NameToIndex.find(layerName);
			if (iterName != s_NameToIndex.end())
			{
				s_IndexToName.erase(iterName->second);
				s_NameToIndex.erase(iterName);
			}

			auto iterIndex = s_IndexToName.find(layerIndex);
			if (iterIndex != s_IndexToName.end())
			{
				s_NameToIndex.erase(iterIndex->second);
				s_IndexToName.erase(iterIndex);
			}

			s_NameToIndex.insert({ layerName, layerIndex });
			s_IndexToName.insert({ layerIndex, layerName });
		}

		static int NameToLayerIndex(const std::string& layerName)
		{
			auto iter = s_NameToIndex.find(layerName);
			if (iter != s_NameToIndex.end())
			{
				return static_cast<int>(iter->second);
			}

			return -1;
		}

		static const LayerMask NameToLayer(const std::string& layerName)
		{
			auto iter = s_NameToIndex.find(layerName);
			if (iter != s_NameToIndex.end())
			{
				return LayerMask(1u << iter->second);
			}
			else
			{
				return GetEmpty();
			}
		}

		static std::string LayerIndexToName(const LayerIndex layerIndex)
		{
			auto iter = s_IndexToName.find(layerIndex);
			if (iter != s_IndexToName.end())
			{
				return iter->second;
			}

			return "";
		}

		static std::string LayerToName(const LayerMask& layer)
		{
			if (layer.Value != 0 && std::has_single_bit(layer.Value))
			{
				auto layerIndex = (LayerIndex)std::countr_zero(layer.Value);
				return LayerIndexToName(layerIndex);
			}

			return "";
		}

		static void RemoveLayer(const LayerIndex layerIndex)
		{
			auto layerName = LayerIndexToName(layerIndex);
			if (layerName != "" && layerName != "Default" && layerName != "UI")
			{
				s_NameToIndex.erase(layerName);
				s_IndexToName.erase(layerIndex);
			}
		}

		static void RemoveLayer(const std::string& layerName)
		{
			auto layerIndex = NameToLayerIndex(layerName);
			if (layerIndex != -1)
			{
				s_NameToIndex.erase(layerName);
				s_IndexToName.erase(static_cast<LayerIndex>(layerIndex));
			}
		}

		static const LayerMask GetMask(const std::string& layerName)
		{
			return NameToLayer(layerName);
		}

		static const LayerMask GetMask(const std::initializer_list<std::string>& layerNames)
		{
			auto mask = GetEmpty();

			for (auto& layerName : layerNames)
			{
				mask = mask | GetMask(layerName);
			}

			return mask;
		}

		static std::vector<LayerEntry> GetLayersOrdered()
		{
			std::vector<LayerEntry> layers;
			layers.reserve(s_IndexToName.size());

			for (const auto& [index, name] : s_IndexToName)
			{
				layers.push_back({ index, name });
			}

			std::sort(layers.begin(), layers.end(), [](const LayerEntry& left, const LayerEntry& right)
			{
				return left.Index < right.Index;
			});

			return layers;
		}

		static const size_t GetLayerCount() { return s_NameToIndex.size(); }
	};
}
