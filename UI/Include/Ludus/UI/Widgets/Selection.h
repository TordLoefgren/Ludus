#pragma once

#include <concepts>
#include <cstddef>
#include <format>
#include <functional>
#include <iterator>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include <Ludus/Engine/Core/Enums/EnumTraits.h>

namespace Ludus::UI::Widgets
{
	namespace Detail
	{
		template<typename TEnum>
		struct EnumComboCache
		{
			static const std::vector<const char*>& Names()
			{
				// Immediately-invoked function expression (IIFE) to ensure one-time build.
				static const std::vector<const char*> names = []
				{
					const auto& namesBuffer = Ludus::Engine::Core::Enums::GetDisplayNames<TEnum>();
					std::vector<const char*> items;
					items.reserve(namesBuffer.size());

					for (const auto& name : namesBuffer)
					{
						items.push_back(name.c_str());
					}

					return items;
				}();

				return names;
			}
		};
	}

	bool Selectable(const char* label, bool selected = false);

	bool Combo(const std::string& label, int* currentItem, const std::vector<const char*>& items);

	template<typename T, typename TSelector>
	std::vector<const char*> GetCStringItems(const std::vector<T>& items, TSelector&& selector)
	{
		std::vector<const char*> out;
		out.reserve(items.size());

		for (const auto& item : items)
		{
			out.push_back(selector(item));
		}

		return out;
	}

	template<typename TEnum>
		requires Ludus::Engine::Core::Enums::HasValues<TEnum>
	inline bool ComboEnum(const char* label, TEnum& currentItem)
	{
		const auto& values = Ludus::Engine::Core::Enums::GetValues<TEnum>::Values;
		if (values.empty())
		{
			return false;
		}

		auto currentIndex = Ludus::Engine::Core::Enums::IndexOf(currentItem);
		if (currentIndex < 0)
		{
			currentIndex = 0;
		}

		if (!Ludus::UI::Widgets::Combo(label, &currentIndex, Detail::EnumComboCache<TEnum>::Names()))
		{
			return false;
		}

		currentItem = values[static_cast<std::size_t>(currentIndex)];
		return true;
	}
}
