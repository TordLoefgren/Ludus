#include "pch.h"

#include <Ludus/UI/Context/StyleContext.h>

namespace Ludus::UI::Context::StyleContext
{
	void SetFontSizeBase(float size)
	{
		ImGui::GetStyle().FontSizeBase = size;
	}

	float GetFontSizeBase()
	{
		return ImGui::GetStyle().FontSizeBase;
	}

	void SetFontScaleDpi(float scale)
	{
		ImGui::GetStyle().FontScaleDpi = scale;
	}

	float GetFontScaleDpi()
	{
		return ImGui::GetStyle().FontScaleDpi;
	}
}
