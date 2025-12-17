#pragma once

#include <format>
#include <string>

namespace Ludus::UI
{
	template<typename VisibleT, typename HiddenT>
	inline std::string CreateLabel(VisibleT visibleLabel, HiddenT hiddenLabel)
	{
		return std::format("{}##{}", visibleLabel, hiddenLabel);
	}
}
