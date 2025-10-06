#pragma once

#include <assert.h>
#include <bit>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace Ludus::Engine
{
	inline std::unordered_map<std::string, uint8_t> s_NameToIndex = { {"Default", 0} };
	inline std::unordered_map<uint8_t, std::string> s_IndexToName = { {0, "Default"} };

	using LayerIndex = uint8_t;
	using LayerBits = uint32_t;

	struct LayerMask
	{
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
		static LayerMask GetDefault() { return LayerMask(1u << 0u); }

		static LayerMask FromIndex(LayerIndex index)
		{
			assert(index < 32u);

			if (index >= 32u)
			{
				return GetEmpty();
			}

			return LayerMask(1u << index);
		}

		static void AddLayer(const std::string& layerName, const LayerIndex layerIndex)
		{
			assert(layerIndex > 0u || layerIndex < 32u);

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
			auto it = s_NameToIndex.find(layerName);
			if (it != s_NameToIndex.end())
			{
				return static_cast<int>(it->second);
			}

			return -1;
		}

		static LayerMask NameToLayer(const std::string& layerName)
		{
			auto it = s_NameToIndex.find(layerName);
			if (it != s_NameToIndex.end())
			{
				return LayerMask(1u << it->second);
			}
			else
			{
				return GetEmpty();
			}
		}

		static std::string LayerIndexToName(const LayerIndex layerIndex)
		{
			auto it = s_IndexToName.find(layerIndex);
			if (it != s_IndexToName.end())
			{
				return it->second;
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
			if (layerName != "" && layerName != "Default")
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

		static LayerMask GetMask(const std::string& layerName)
		{
			return NameToLayer(layerName);
		}

		static LayerMask GetMask(const std::vector<std::string>& layerNames)
		{
			auto mask = GetEmpty();

			for (auto& layerName : layerNames)
			{
				mask = mask | GetMask(layerName);
			}

			return mask;
		}
	};
}
