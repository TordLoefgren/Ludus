#include "pch.h"

#include <format>

#include <Ludus/Editor/Core/Constants.h>
#include <Ludus/Editor/Core/EditorSelection.h>
#include <Ludus/Editor/Panels/HierarchyPanel.h>
#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Graphics/Shape.h>
#include <Ludus/UI/Context/InputContext.h>
#include <Ludus/UI/Context/LayoutContext.h>
#include <Ludus/UI/Context/SelectionContext.h>
#include <Ludus/UI/Context/WindowContext.h>
#include <Ludus/UI/Flags/Flags.h>
#include <Ludus/UI/Icons/FontAwesome.h>
#include <Ludus/UI/Labels.h>
#include <Ludus/UI/Scope/ComboScope.h>
#include <Ludus/UI/Scope/MenuScope.h>
#include <Ludus/UI/Scope/PopupScope.h>
#include <Ludus/UI/Scope/TreeNodeScope.h>
#include <Ludus/UI/Scope/WindowScope.h>
#include <Ludus/UI/Widgets/Buttons.h>
#include <Ludus/UI/Widgets/Menu.h>
#include <Ludus/UI/Widgets/Selection.h>

namespace Ludus::Editor::Panels
{

#pragma region Scene Tool Bar

	struct SceneToolBarActions
	{
		std::optional<Ludus::Engine::Core::SceneHandle> SelectedSceneHandle = std::nullopt;
		bool IsSceneAdded = false;
		bool IsSceneRemoved = false;
		bool IsSceneSetActive = false;
	};

	const SceneToolBarActions HierarchyPanel::DrawSceneToolBar(
		std::span<const Ludus::Engine::Core::Scene> scenes,
		std::optional<Ludus::Engine::Core::SceneHandle> sceneHandle
	)
	{
		SceneToolBarActions actions;

		actions.SelectedSceneHandle = sceneHandle;

		const auto comboLabel = Ludus::UI::CreateLabel("", "ActiveScene");
		const auto preview = sceneHandle.has_value() ? std::format("Scene {}", sceneHandle.value()) : "None";

		if (Ludus::UI::Scope::ComboScope combo(comboLabel.c_str(), preview.c_str()); combo)
		{
			if (scenes.empty())
			{
				const auto _ = Ludus::UI::Widgets::Selectable("None", false);
			}
			else
			{
				for (const auto& scene : scenes)
				{
					const bool isSelected = sceneHandle == scene.Handle;
					const auto itemLabel = std::format("Scene {}", scene.Handle);
					if (Ludus::UI::Widgets::Selectable(itemLabel.c_str(), isSelected))
					{
						actions.SelectedSceneHandle = scene.Handle;
					}

					if (isSelected)
					{
						Ludus::UI::Context::SelectionContext::SetItemDefaultFocus();
					}
				}
			}
		}

		const auto spacing = 6.0f;

		Ludus::UI::Context::LayoutContext::SameLine(0.0f, spacing);

		auto plusLabel = Ludus::UI::CreateLabel(ICON_PLUS, "Plus");
		if (Ludus::UI::Widgets::Button(plusLabel.c_str()))
		{
			actions.IsSceneAdded = true;
		}

		Ludus::UI::Context::LayoutContext::SameLine(0.0f, spacing);

		const char* popupId = "SceneOptionsPopup";

		const auto ellipsisLabel = Ludus::UI::CreateLabel(ICON_ELLIPSIS_V, "Ellipsis");
		if (Ludus::UI::Widgets::Button(ellipsisLabel.c_str()))
		{
			Ludus::UI::Scope::OpenPopup(popupId);
		}

		if (Ludus::UI::Scope::PopupScope popupScope(popupId); popupScope)
		{
			if (Ludus::UI::Widgets::MenuItem("Set active scene"))
			{
				actions.IsSceneSetActive = true;
			}

			if (Ludus::UI::Widgets::MenuItem("Remove scene"))
			{
				actions.IsSceneRemoved = true;
			}
		}

		return actions;
	}

	std::optional<Ludus::Engine::Core::SceneHandle> HierarchyPanel::ApplySceneToolBarActions(const SceneToolBarActions& actions, Ludus::Editor::Panels::PanelContext& context)
	{
		auto& sceneManager = context.SystemContext.SceneManager;
		auto& selection = context.EditorContext.State.Selection;

		if (actions.IsSceneAdded)
		{
			const auto sceneHandle = sceneManager.AddScene();
			selection.SelectScene(sceneHandle);

			return sceneHandle;
		}

		if (actions.SelectedSceneHandle.has_value())
		{
			const auto sceneHandle = actions.SelectedSceneHandle.value();
			if (actions.IsSceneRemoved)
			{
				sceneManager.RemoveScene(sceneHandle);
				selection.DeselectScene();

				return sceneManager.GetActiveSceneHandle();
			}

			if (actions.IsSceneSetActive)
			{
				sceneManager.SetActiveScene(sceneHandle);
			}
		}

		return actions.SelectedSceneHandle;
	}

#pragma endregion

#pragma region Scene Context Menu

	struct SceneContextMenuActions
	{
		bool IsEntityAdded = false;
		bool IsCamera = false;
		bool IsSprite = false;

		std::string DisplayName;
		Ludus::Engine::Graphics::Shape SpriteShape;
	};

	const SceneContextMenuActions HierarchyPanel::DrawSceneContextMenu(Ludus::Editor::Panels::PanelContext& context, Ludus::Engine::Core::Scene& scene)
	{
		SceneContextMenuActions actions;

		auto& selection = context.EditorContext.State.Selection;
		auto& ecs = scene.EntityComponentSystem;

		auto flags = Ludus::UI::Flags::Popup::MouseButtonRight | Ludus::UI::Flags::Popup::NoOpenOverItems;
		if (Ludus::UI::Scope::PopupContextWindowScope contextWindow("HierarchyWindowContext", flags); contextWindow)
		{
			if (Ludus::UI::Widgets::MenuItem("Add empty"))
			{
				actions.IsEntityAdded = true;
				actions.DisplayName = "Empty";
			}

			if (Ludus::UI::Widgets::MenuItem("Add camera"))
			{
				actions.IsEntityAdded = true;
				actions.DisplayName = "Camera";
				actions.IsCamera = true;
			}

			if (Ludus::UI::Scope::MenuScope menu("Add sprite"); menu)
			{
				auto isSprite = false;

				if (Ludus::UI::Widgets::MenuItem("Circle"))
				{
					actions.DisplayName = "Circle";
					actions.SpriteShape = Ludus::Engine::Graphics::Shape::Circle;

					isSprite = true;
				}

				if (Ludus::UI::Widgets::MenuItem("Quad"))
				{
					actions.DisplayName = "Quad";
					actions.SpriteShape = Ludus::Engine::Graphics::Shape::Quad;

					isSprite = true;
				}

				if (isSprite)
				{
					actions.IsEntityAdded = true;
					actions.IsSprite = true;
				}
			}
		}

		return actions;
	}

	void HierarchyPanel::ApplySceneContextMenuActions(const SceneContextMenuActions& actions, Ludus::Editor::Panels::PanelContext& context, Ludus::Engine::Core::Scene& scene)
	{
		auto& selection = context.EditorContext.State.Selection;
		auto& ecs = scene.EntityComponentSystem;

		if (!actions.IsEntityAdded)
		{
			return;
		}

		const auto handle = ecs.AddEntity();
		ecs.AttachTransform(handle);
		ecs.AttachDisplayName(handle, actions.DisplayName);

		if (actions.IsCamera)
		{
			ecs.AttachCamera(handle);
		}
		else if (actions.IsSprite)
		{
			ecs.AttachSprite(handle, actions.SpriteShape);
		}

		selection.SelectEntity(handle, scene.Handle);
	}

#pragma endregion

#pragma region Entity Rows

	enum class ComponentKind { Camera2D, Collider2D, RigidBody2D, Sprite2D, Text2D };

	struct EntityRowActions
	{
		bool IsEntityRemoved = false;
		bool IsEntitySelected = false;
		std::optional<ComponentKind> AddedComponent = std::nullopt;
	};

	struct EntityRowCommands
	{
		std::vector<Ludus::Engine::Core::EntityHandle> EntitiesToRemove;
		std::optional<Ludus::Engine::Core::EntityHandle> EntityToSelect = std::nullopt;
		std::vector<std::pair<ComponentKind, Ludus::Engine::Core::EntityHandle>> ComponentsToAdd;
	};

	const EntityRowActions HierarchyPanel::DrawEntityRow(Ludus::Editor::Panels::PanelContext& context, Ludus::Engine::Core::Scene& scene, Ludus::Engine::Core::EntityHandle entityHandle)
	{
		EntityRowActions actions;

		auto& selection = context.EditorContext.State.Selection;
		auto& ecs = scene.EntityComponentSystem;
		const auto sceneHandle = scene.Handle;

		const auto* displayNamePtr = ecs.DisplayNames.TryGetByOwner(entityHandle);
		const auto entityLabel = displayNamePtr != nullptr
			? Ludus::UI::CreateLabel(displayNamePtr->Value, entityHandle)
			: Ludus::UI::CreateLabel(std::format("Entity {}", entityHandle), entityHandle);

		if (Ludus::UI::Widgets::Selectable(entityLabel.c_str(), selection.IsSelected(entityHandle, scene.Handle)))
		{
			actions.IsEntitySelected = true;
		}

		if (Ludus::UI::Scope::PopupContextItemScope contextItem; contextItem)
		{
			if (Ludus::UI::Widgets::MenuItem("Select"))
			{
				actions.IsEntitySelected = true;
			}

			Ludus::UI::Context::LayoutContext::Separator();

			if (Ludus::UI::Widgets::MenuItem("Remove"))
			{
				actions.IsEntityRemoved = true;
			}

			Ludus::UI::Context::LayoutContext::Separator();

			if (Ludus::UI::Scope::MenuScope componentMenu("Add component"); componentMenu)
			{
				if (!ecs.Cameras.ContainsOwner(entityHandle) && Ludus::UI::Widgets::MenuItem("Camera 2D"))
				{
					actions.AddedComponent = ComponentKind::Camera2D;
				}

				if (!ecs.Colliders.ContainsOwner(entityHandle) && Ludus::UI::Widgets::MenuItem("Collider 2D"))
				{
					actions.AddedComponent = ComponentKind::Collider2D;
				}

				if (!ecs.RigidBodies.ContainsOwner(entityHandle) && Ludus::UI::Widgets::MenuItem("Rigid Body 2D"))
				{
					actions.AddedComponent = ComponentKind::RigidBody2D;

				}

				if (!ecs.Sprites.ContainsOwner(entityHandle) && Ludus::UI::Widgets::MenuItem("Sprite 2D"))
				{
					actions.AddedComponent = ComponentKind::Sprite2D;

				}

				if (!ecs.Texts.ContainsOwner(entityHandle) && Ludus::UI::Widgets::MenuItem("Text 2D"))
				{
					actions.AddedComponent = ComponentKind::Text2D;

				}

				if (actions.AddedComponent.has_value())
				{
					actions.IsEntitySelected = true;
				}
			}
		}

		return actions;
	}

	void HierarchyPanel::AddToEntityRowCommands(EntityRowCommands& commands, const EntityRowActions& actions, Ludus::Engine::Core::EntityHandle entityHandle)
	{
		if (actions.IsEntityRemoved)
		{
			commands.EntitiesToRemove.push_back(entityHandle);
		}

		if (actions.IsEntitySelected)
		{
			commands.EntityToSelect = entityHandle;
		}

		if (actions.AddedComponent.has_value())
		{
			commands.ComponentsToAdd.push_back({ *actions.AddedComponent, entityHandle });
		}
	}

	void HierarchyPanel::ApplyEntityRowCommands(const EntityRowCommands& commands, Ludus::Editor::Panels::PanelContext& context, Ludus::Engine::Core::Scene& scene)
	{
		auto& ecs = scene.EntityComponentSystem;
		auto& selection = context.EditorContext.State.Selection;

		for (auto [kind, handle] : commands.ComponentsToAdd)
		{
			switch (kind)
			{
			case ComponentKind::Camera2D:		ecs.AttachCamera(handle);		break;
			case ComponentKind::Collider2D:		ecs.AttachCollider(handle);		break;
			case ComponentKind::RigidBody2D:	ecs.AttachRigidBody(handle);	break;
			case ComponentKind::Sprite2D:		ecs.AttachSprite(handle);		break;
			case ComponentKind::Text2D:			ecs.AttachText(handle, "");		break;
			}
		}

		for (auto handle : commands.EntitiesToRemove)
		{
			ecs.DestroyEntity(handle);
			if (selection.IsSelected(handle, scene.Handle))
			{
				selection.DeselectEntity();
			}
		}

		if (commands.EntityToSelect.has_value())
		{
			selection.SelectEntity(commands.EntityToSelect.value(), scene.Handle);
		}
	}

#pragma endregion

	bool HierarchyPanel::UpdateImpl(Ludus::Editor::Panels::PanelContext& context)
	{
		auto windowTitle = CreateWindowTitle("Hierarchy");
		if (Ludus::UI::Scope::WindowScope window(windowTitle.c_str(), &m_Open, Ludus::Editor::Core::Constants::PanelFlags); window)
		{
			auto& sceneManager = context.SystemContext.SceneManager;
			auto& selection = context.EditorContext.State.Selection;

			if (!m_SelectedSceneHandle.has_value() || !sceneManager.Contains(m_SelectedSceneHandle.value()))
			{
				m_SelectedSceneHandle = sceneManager.GetActiveSceneHandle();
			}

			const auto& toolBarActions = DrawSceneToolBar(sceneManager.View(), m_SelectedSceneHandle);
			m_SelectedSceneHandle = ApplySceneToolBarActions(toolBarActions, context);
			if (!m_SelectedSceneHandle.has_value())
			{
				return true;
			}

			const auto sceneHandle = m_SelectedSceneHandle.value();
			auto* scene = context.SystemContext.SceneManager.TryGetScene(sceneHandle);

			if (!scene)
			{
				return true;
			}

			Ludus::UI::Context::LayoutContext::Separator();

			const auto& contextMenuActions = DrawSceneContextMenu(context, *scene);
			ApplySceneContextMenuActions(contextMenuActions, context, *scene);

			EntityRowCommands commands;

			for (const auto& entity : scene->EntityComponentSystem.View())
			{
				const auto entityRowActions = DrawEntityRow(context, *scene, entity.Handle);
				AddToEntityRowCommands(commands, entityRowActions, entity.Handle);
			}

			ApplyEntityRowCommands(commands, context, *scene);

			if (Ludus::UI::Context::InputContext::IsMouseClicked(Ludus::Engine::Platform::MouseButton::Left) &&
				Ludus::UI::Context::InputContext::IsWindowHovered(Ludus::UI::Flags::Hovered::AllowWhenBlockedByActiveItem) &&
				!Ludus::UI::Context::InputContext::IsAnyItemHovered())
			{
				selection.DeselectEntity();
			}
		}

		return true;
	}
}
