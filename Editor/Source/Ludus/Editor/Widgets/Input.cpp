#include "pch.h"

#include <Ludus/Editor/Core/Axis.h>
#include <Ludus/Editor/Widgets/Input.h>
#include <Ludus/Engine/Math/Vector2D.h>
#include <Ludus/UI/Context/LayoutContext.h>
#include <Ludus/UI/Context/TableContext.h>
#include <Ludus/UI/Context/ThemeContext.h>
#include <Ludus/UI/Scope/IDScope.h>
#include <Ludus/UI/Scope/StyleScope.h>
#include <Ludus/UI/Scope/TableScope.h>
#include <Ludus/UI/Widgets/Input.h>

namespace Ludus::Editor::Widgets
{
	namespace
	{
		constexpr float AxisFloatFieldSpacing = 1.0f;

		const Ludus::Engine::Graphics::Color& GetAxisColor(const Ludus::Editor::Core::Axis axis)
		{
			switch (axis)
			{
				case Ludus::Editor::Core::Axis::X: return Ludus::UI::Context::ThemeContext::AxisX();
				case Ludus::Editor::Core::Axis::Y: return Ludus::UI::Context::ThemeContext::AxisY();
				case Ludus::Editor::Core::Axis::Z: return Ludus::UI::Context::ThemeContext::AxisZ();
				default:                           return Ludus::UI::Context::ThemeContext::TextPrimary();
			}
		}
	}

	bool DragFloatAxisButton(
		const char* label,
		float* value,
		const Ludus::Editor::Core::Axis axis,
		const Ludus::Engine::Math::Vector2D& size,
		const float speed
	)
	{
		return Ludus::UI::Widgets::DragFloatColoredButton(label, value, GetAxisColor(axis), size, speed);
	}

	bool DrawAxisFloatField(const char* id, float* value, const Ludus::Editor::Core::Axis axis)
	{
		bool changed = false;

		Ludus::UI::Scope::IDScope _(id);

		changed |= DragFloatAxisButton(Ludus::Engine::Core::Enums::GetDisplayName(axis).c_str(), value, axis, { Ludus::UI::Context::LayoutContext::GetFrameHeight(), 0.0f });

		Ludus::UI::Context::LayoutContext::SameLine(0.0f, AxisFloatFieldSpacing);
		Ludus::UI::Context::LayoutContext::SetNextItemWidthFill();

		changed |= Ludus::UI::Widgets::InputFloat("##Value", value);

		return changed;
	}

	bool DrawVector2Field(const char* id, float* x, float* y)
	{
		bool changed = false;

		const auto cellPaddingX = Ludus::UI::Context::ThemeContext::GetActiveMetrics().CellPadding.X;
		Ludus::UI::Scope::StyleVarScope cellPaddingScope(
			Ludus::UI::Scope::StyleVar::Vector(Ludus::UI::Scope::Variable::CellPadding, { cellPaddingX, 0.0f })
		);

		if (Ludus::UI::Scope::TableScope table(id, 2); table)
		{
			Ludus::UI::Context::TableContext::TableSetupColumn("X", Ludus::UI::Flags::TableColumn::WidthStretch);
			Ludus::UI::Context::TableContext::TableSetupColumn("Y", Ludus::UI::Flags::TableColumn::WidthStretch);

			Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
			changed |= DrawAxisFloatField("X", x, Ludus::Editor::Core::Axis::X);

			Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
			changed |= DrawAxisFloatField("Y", y, Ludus::Editor::Core::Axis::Y);
		}

		return changed;
	}
}
