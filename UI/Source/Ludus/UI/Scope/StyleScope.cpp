#include "pch.h"

#include <initializer_list>
#include <variant>

#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/UI/Scope/StyleScope.h>

namespace Ludus::UI::Scope
{
	StyleVarScope::StyleVarScope(std::initializer_list<StyleVar> styles)
	{
		for (auto& style : styles)
		{
			std::visit([&](const auto& value)
				{
					using T = std::decay_t<decltype(value)>;

					if constexpr (std::is_same_v<T, float>)
					{
						ImGui::PushStyleVar(static_cast<int>(style.Var), value);
					}
					else
					{
						ImGui::PushStyleVar(static_cast<int>(style.Var), ImVec2 { value.X, value.Y });
					}
				}, style.Data);

			m_StyleCount++;
		}
	}

	StyleVarScope::~StyleVarScope()
	{
		ImGui::PopStyleVar(m_StyleCount);
	}

	StyleColorScope::StyleColorScope(std::initializer_list<StyleColor> styles)
	{
		for (auto& style : styles)
		{
			ImGui::PushStyleColor(static_cast<int>(style.Col), ImVec4(style.Value.R, style.Value.G, style.Value.B, style.Value.A));
			m_StyleCount++;
		}
	}

	StyleColorScope::~StyleColorScope()
	{
		ImGui::PopStyleColor(m_StyleCount);
	}
}
