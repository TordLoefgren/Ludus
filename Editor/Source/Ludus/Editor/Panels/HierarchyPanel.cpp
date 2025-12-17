#include "pch.h"

#include <format>

#include <Ludus/Editor/Core/Constants.h>
#include <Ludus/Editor/Panels/HierarchyPanel.h>
#include <Ludus/Engine/Graphics/Shape.h>
#include <Ludus/UI/Context/InputContext.h>
#include <Ludus/UI/Flags/Flags.h>
#include <Ludus/UI/Labels.h>
#include <Ludus/UI/Scope/MenuScope.h>
#include <Ludus/UI/Scope/PopupScope.h>
#include <Ludus/UI/Scope/WindowScope.h>
#include <Ludus/UI/Widgets/Menu.h>
#include <Ludus/UI/Widgets/Selection.h>

namespace Ludus::Editor::Panels
{
	void HierarchyPanel::UpdateImpl(Ludus::Editor::Panels::PanelContext& context)
	{
		auto windowTitle = CreateWindowTitle("Hierarchy");
		if (Ludus::UI::Scope::WindowScope window(windowTitle.c_str(), &m_Open, Ludus::Editor::Core::Constants::PanelFlags); window)
		{
			auto& ecs = context.SystemContext.EntityComponentSystem;
			const auto entities = ecs.View();

			static auto selectedIndex = -1;
			for (int i = 0; i < static_cast<int>(entities.size()); i++)
			{
				const auto handle = entities[i].Handle;

				const auto label = Ludus::UI::CreateLabel(std::format("Entity {}", handle), handle);

				if (Ludus::UI::Widgets::Selectable(label.c_str(), selectedIndex == i))
				{
					selectedIndex = i;
				}

				if (Ludus::UI::Scope::PopupContextItemScope contextItem; contextItem)
				{
					if (Ludus::UI::Widgets::MenuItem("Select"))
					{
						selectedIndex = i;
					}

					if (Ludus::UI::Widgets::MenuItem("Remove"))
					{
						ecs.DestroyEntity(handle);
						if (selectedIndex == i)
						{
							selectedIndex = -1;
						}
					}
				}
			}

			context.EditorContext.State.SelectedEntity = selectedIndex < 0 ? -1 : entities[selectedIndex].Handle;

			auto flags = Ludus::UI::Flags::Popup::MouseButtonRight | Ludus::UI::Flags::Popup::NoOpenOverItems;
			if (Ludus::UI::Scope::PopupContextWindowScope contextWindow("HierarchyWindowContext", flags); contextWindow)
			{
				if (Ludus::UI::Widgets::MenuItem("Create empty"))
				{
					auto handle = ecs.AddEntity();
					ecs.AttachTransform(handle);

					context.EditorContext.State.SelectedEntity = handle;
					selectedIndex = static_cast<int>(ecs.IndexOf(handle));
				}

				auto hasSprite = false;

				if (Ludus::UI::Scope::MenuScope menu("2D Sprite"); menu)
				{
					Ludus::Engine::Graphics::Shape shape;

					if (Ludus::UI::Widgets::MenuItem("Quad"))
					{
						shape = Ludus::Engine::Graphics::Shape::Rect;
						hasSprite = true;
					}

					if (Ludus::UI::Widgets::MenuItem("Circle"))
					{
						shape = Ludus::Engine::Graphics::Shape::Circle;
						hasSprite = true;
					}

					if (hasSprite)
					{
						auto handle = ecs.AddEntity();
						ecs.AttachTransform(handle);
						ecs.AttachSprite(handle, shape);

						context.EditorContext.State.SelectedEntity = handle;
						selectedIndex = static_cast<int>(ecs.IndexOf(handle));
					}
				}
			}

			if (Ludus::UI::Context::InputContext::IsMouseClicked(Ludus::Engine::Platform::MouseButton::Left) &&
				Ludus::UI::Context::InputContext::IsWindowHovered(Ludus::UI::Flags::Hovered::AllowWhenBlockedByActiveItem)
				&& !Ludus::UI::Context::InputContext::IsAnyItemHovered())
			{
				selectedIndex = -1;
			}
		}
	}
}
