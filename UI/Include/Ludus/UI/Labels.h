#pragma once

#include <format>
#include <string>
#include <string_view>

namespace Ludus::UI
{
	inline std::string CreateLabel(std::string_view visibleLabel, std::string_view hiddenLabel)
	{
		return std::format("{}##{}", visibleLabel, hiddenLabel);
	}

	inline std::string CreateLabelWithIcon(std::string_view icon, std::string_view visibleLabel, std::string_view hiddenLabel)
	{
		return std::format("{} {}##{}", icon, visibleLabel, hiddenLabel);
	}
}
