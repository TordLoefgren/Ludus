#include "pch.h"

#include <algorithm>
#include <string>
#include <vector>

#include <Ludus/Editor/Core/Constants.h>
#include <Ludus/Editor/Widgets/Buttons.h>
#include <Ludus/Engine/Core/Strings.h>
#include <Ludus/UI/Context/InputContext.h>
#include <Ludus/UI/Context/LayoutContext.h>
#include <Ludus/UI/Context/ThemeContext.h>
#include <Ludus/UI/Context/WindowContext.h>
#include <Ludus/UI/Types/DrawList.h>
#include <Ludus/UI/Widgets/Buttons.h>

namespace Ludus::Editor::Widgets
{
	namespace
	{
		float MeasureTextWidth(std::string_view text)
		{
			return Ludus::UI::Context::LayoutContext::CalculateTextSize(std::string(text)).X;
		}

		std::vector<float> MeasureCharacterWidths(std::string_view text)
		{
			std::vector<float> characterWidths;
			characterWidths.reserve(text.size());

			for (const char character : text)
			{
				characterWidths.push_back(MeasureTextWidth(std::string_view(&character, 1)));
			}

			return characterWidths;
		}

		std::string TruncateDisplayNameToWidth(std::string_view text, float maxWidth)
		{
			if (text.empty() || MeasureTextWidth(text) <= maxWidth)
			{
				return std::string(text);
			}

			const auto ellipsisWidth = MeasureTextWidth(Ludus::Engine::Core::Strings::DefaultEllipsis);
			if (ellipsisWidth > maxWidth)
			{
				return { };
			}

			const auto characterWidths = MeasureCharacterWidths(text);
			float currentWidth = ellipsisWidth;

			std::size_t leftCount = 0;
			for (; leftCount < characterWidths.size(); ++leftCount)
			{
				if (currentWidth + characterWidths[leftCount] > maxWidth)
				{
					break;
				}

				currentWidth += characterWidths[leftCount];
			}

			return Ludus::Engine::Core::Strings::EllipsizeAt(text, leftCount);
		}

		std::string TruncateProjectPathToWidth(std::string_view path, float maxWidth)
		{
			if (path.empty() || MeasureTextWidth(path) <= maxWidth)
			{
				return std::string(path);
			}

			const auto ellipsisWidth = MeasureTextWidth(Ludus::Engine::Core::Strings::DefaultEllipsis);
			if (ellipsisWidth > maxWidth)
			{
				return { };
			}

			const auto characterWidths = MeasureCharacterWidths(path);
			std::size_t leftCount = path.size() / 2;
			std::size_t rightCount = path.size() - leftCount;

			float leftWidth = 0.0f;
			for (std::size_t index = 0; index < leftCount; ++index)
			{
				leftWidth += characterWidths[index];
			}

			float rightWidth = 0.0f;
			for (std::size_t index = path.size() - rightCount; index < path.size(); ++index)
			{
				rightWidth += characterWidths[index];
			}

			while (leftCount > 0 && rightCount > 0)
			{
				if (leftWidth + ellipsisWidth + rightWidth <= maxWidth)
				{
					return Ludus::Engine::Core::Strings::EllipsizeAt(path, leftCount, rightCount);
				}

				if (rightCount >= leftCount)
				{
					rightWidth -= characterWidths[path.size() - rightCount];
					--rightCount;
				}
				else
				{
					leftWidth -= characterWidths[leftCount - 1];
					--leftCount;
				}
			}

			return std::string(Ludus::Engine::Core::Strings::DefaultEllipsis);
		}
	}

	WidgetState ButtonBase(const char* id, const Ludus::Engine::Math::Vector2D size)
	{
		const auto min = Ludus::UI::Context::WindowContext::GetCursorScreenPosition();
		const auto pressed = Ludus::UI::Widgets::InvisibleButton(id, size);

		return
		{
			.Active = Ludus::UI::Context::InputContext::IsItemActive(),
			.Hovered = Ludus::UI::Context::InputContext::IsItemHovered(),
			.Pressed = pressed,
			.Min = min,
			.Max = { min.X + size.X, min.Y + size.Y }
		};
	}

	bool RecentProjectRow(const RecentProjectRowData& rowData)
	{
		const auto width = Ludus::UI::Context::WindowContext::GetContentRegionAvailable().X;
		const auto height = Ludus::Editor::Core::Constants::Welcome::RecentProjectRow::Height;

		const auto widgetState = ButtonBase(rowData.Id.data(), { width, height });

		const auto background = widgetState.Active
			? Ludus::UI::Context::ThemeContext::ControlActive()
			: widgetState.Hovered
			? Ludus::UI::Context::ThemeContext::RowHover()
			: Ludus::UI::Context::ThemeContext::ControlBackground();

		const auto border = rowData.IsPathMissing
			? Ludus::UI::Context::ThemeContext::Error()
			: widgetState.Hovered
			? Ludus::UI::Context::ThemeContext::BorderStrong()
			: Ludus::UI::Context::ThemeContext::BorderSubtle();

		const std::string missingBadgeText = "Missing";
		const auto badgeWidth = rowData.IsPathMissing
			? Ludus::UI::Context::LayoutContext::CalculateTextSize(missingBadgeText).X
			: 0.0f;

		const auto badgeReservedWidth = rowData.IsPathMissing
			? badgeWidth + Ludus::Editor::Core::Constants::Welcome::RecentProjectRow::BadgeGap
			: 0.0f;

		const auto availableTextWidth = std::max(
			0.0f,
			width
			- Ludus::Editor::Core::Constants::Welcome::RecentProjectRow::TextPaddingX * 2.0f
			- badgeReservedWidth);

		const auto titleText = TruncateDisplayNameToWidth(rowData.DisplayName, availableTextWidth);
		const auto pathText = TruncateProjectPathToWidth(rowData.Path, availableTextWidth);

		auto drawList = Ludus::UI::Types::DrawList();
		drawList.AddRectFilled(
			widgetState.Min,
			widgetState.Max,
			background,
			Ludus::Editor::Core::Constants::Welcome::RecentProjectRow::Rounding);

		drawList.AddRect(
			widgetState.Min,
			widgetState.Max,
			border,
			Ludus::Editor::Core::Constants::Welcome::RecentProjectRow::Rounding);

		drawList.AddText(
			{
				widgetState.Min.X + Ludus::Editor::Core::Constants::Welcome::RecentProjectRow::TextPaddingX,
				widgetState.Min.Y + Ludus::Editor::Core::Constants::Welcome::RecentProjectRow::TitleOffsetY
			},
			Ludus::UI::Context::ThemeContext::TextPrimary(),
			titleText.c_str());

		drawList.AddText(
			{
				widgetState.Min.X + Ludus::Editor::Core::Constants::Welcome::RecentProjectRow::TextPaddingX,
				widgetState.Min.Y + Ludus::Editor::Core::Constants::Welcome::RecentProjectRow::PathOffsetY
			},
			Ludus::UI::Context::ThemeContext::TextSecondary(),
			pathText.c_str());

		if (rowData.IsPathMissing)
		{
			drawList.AddText(
				{
					widgetState.Max.X
					- Ludus::Editor::Core::Constants::Welcome::RecentProjectRow::TextPaddingX
				- badgeWidth,
				widgetState.Min.Y + Ludus::Editor::Core::Constants::Welcome::RecentProjectRow::TitleOffsetY
				},
				Ludus::UI::Context::ThemeContext::Error(),
				missingBadgeText.c_str());
		}

		return widgetState.Pressed;
	}
}
