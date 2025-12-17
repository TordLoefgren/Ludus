#pragma once

#include <format>
#include <string>

#include <Ludus/Engine/Core/Enums.h>
#include <Ludus/UI/Widgets/Selection.h>

namespace Ludus::Editor::Core::Utilities
{
	template<typename TEnum>
	inline bool ComboEnum(const char* label, TEnum& currentItem)
	{
		const auto& values = Ludus::Engine::Core::Enums::GetValues<TEnum>::Values;

		auto currentIndex = 0;
		for (int i = 0; i < values.size(); i++)
		{
			if (values[i] == currentItem)
			{
				currentIndex = i;
				break;
			}
		}

		std::vector<std::string> namesBuffer;
		namesBuffer.reserve(values.size());

		std::vector<const char*> names;
		names.reserve(values.size());

		for (auto& value : values)
		{
			namesBuffer.push_back(std::format("{}", value));
			names.push_back(namesBuffer.back().c_str());
		}

		auto hasChanged = Ludus::UI::Widgets::Combo(label, &currentIndex, names);
		if (hasChanged)
		{
			currentItem = values[currentIndex];
		}

		return hasChanged;
	}
}
