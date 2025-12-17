#include "pch.h"

#include <Ludus/UI/Context/ScrollContext.h>

namespace Ludus::UI::Context::ScrollContext
{
	void ScrollToNewest()
	{
		const float scrollY = ImGui::GetScrollY();
		const float scrollMaxY = ImGui::GetScrollMaxY();

		if (scrollY >= scrollMaxY - 1.0f)
		{
			ImGui::SetScrollHereY(1.0f);
		}
	}
}
