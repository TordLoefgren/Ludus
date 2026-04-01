#include "pch.h"

#include <format>
#include <string>
#include <type_traits>
#include <utility>

#include <Ludus/Editor/Commands/Enqueue.h>
#include <Ludus/Editor/Core/Constants.h>
#include <Ludus/Editor/Panels/HierarchyPanel.h>
#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Core/SceneRegistry.h>
#include <Ludus/Engine/Graphics/Shape.h>
#include <Ludus/UI/Context/InputContext.h>
#include <Ludus/UI/Context/LayoutContext.h>
#include <Ludus/UI/Context/SelectionContext.h>
#include <Ludus/UI/Context/WindowContext.h>
#include <Ludus/UI/Flags/Flags.h>
#include <Ludus/UI/Icons/FontAwesome.h>
#include <Ludus/UI/Labels.h>
#include <Ludus/UI/Scope/ComboScope.h>
#include <Ludus/UI/Scope/DisabledScope.h>
#include <Ludus/UI/Scope/MenuScope.h>
#include <Ludus/UI/Scope/PopupScope.h>
#include <Ludus/UI/Scope/TreeNodeScope.h>
#include <Ludus/UI/Scope/WindowScope.h>
#include <Ludus/UI/Widgets/Buttons.h>
#include <Ludus/UI/Widgets/Menu.h>
#include <Ludus/UI/Widgets/Selection.h>

namespace Ludus::Editor::Panels
{
	namespace
	{
		namespace Component = Ludus::Engine::Components;
	}

	void HierarchyPanel::DrawEntityRow(Ludus::Editor::Core::ProjectSessionContext& context, Ludus::Engine::Core::Scene& scene, Ludus::Engine::Core::EntityHandle entityHandle)
	{
		auto& selection = context.ProjectSession.EditorState.Selection;
		auto& ecs = scene.EntityComponentSystem;
		const auto sceneHandle = scene.Handle;

		const auto* displayNamePtr = ecs.DisplayNames.TryGetByOwner(entityHandle);
		const auto entityLabel = displayNamePtr != nullptr
			? Ludus::UI::CreateLabelWithIcon(ICON_CUBE, displayNamePtr->Value, std::to_string(entityHandle))
			: Ludus::UI::CreateLabelWithIcon(ICON_CUBE, std::format("Entity {}", entityHandle), std::to_string(entityHandle));

		if (Ludus::UI::Widgets::Selectable(entityLabel.c_str(), selection.IsSelected(entityHandle)))
		{
			Commands::EnqueueEdit(context.Shell.State.Commands, Commands::EditCommand::SelectEntity { entityHandle });
		}

		if (Ludus::UI::Scope::PopupContextItemScope contextItem; contextItem)
		{
			if (Ludus::UI::Widgets::MenuItem("Select"))
			{
				Commands::EnqueueEdit(context.Shell.State.Commands, Commands::EditCommand::SelectEntity { entityHandle });
			}

			Ludus::UI::Context::LayoutContext::Separator();

			if (Ludus::UI::Widgets::MenuItem("Remove"))
			{
				Commands::EnqueueEdit(context.Shell.State.Commands, Commands::EditCommand::RemoveEntity { .EntityReference = entityHandle, .SceneHandle = sceneHandle });
				Commands::EnqueueEdit(context.Shell.State.Commands, Commands::EditCommand::DeselectEntity { entityHandle });
			}

			Ludus::UI::Context::LayoutContext::Separator();

			if (Ludus::UI::Scope::MenuScope componentMenu("Add component"); componentMenu)
			{
				auto isComponentAdded = false;

				if (!ecs.Cameras.ContainsOwner(entityHandle) && Ludus::UI::Widgets::MenuItem("Camera 2D"))
				{
					Commands::EnqueueEdit(context.Shell.State.Commands, Commands::EditCommand::AddComponent<Component::Camera2DComponent> {
						.EntityReference = entityHandle, .SceneHandle = sceneHandle
					});
					isComponentAdded = true;
				}

				if (!ecs.Colliders.ContainsOwner(entityHandle) && Ludus::UI::Widgets::MenuItem("Collider 2D"))
				{
					Commands::EnqueueEdit(context.Shell.State.Commands, Commands::EditCommand::AddComponent<Component::Collider2DComponent> {
						.EntityReference = entityHandle, .SceneHandle = sceneHandle
					});
					isComponentAdded = true;
				}

				if (!ecs.RigidBodies.ContainsOwner(entityHandle) && Ludus::UI::Widgets::MenuItem("Rigid Body 2D"))
				{
					Commands::EnqueueEdit(context.Shell.State.Commands, Commands::EditCommand::AddComponent<Component::RigidBody2DComponent> {
						.EntityReference = entityHandle, .SceneHandle = sceneHandle
					});
					isComponentAdded = true;
				}

				if (!ecs.Scripts.ContainsOwner(entityHandle) && Ludus::UI::Widgets::MenuItem("Script"))
				{
					Commands::EnqueueUI(context.Shell.State.Commands, Commands::UICommand::OpenAddScriptDialog {
						.EntityHandle = entityHandle, .SceneHandle = sceneHandle
						});
					// The added component will be selected later as part of the command chain.
				}

				if (!ecs.Sprites.ContainsOwner(entityHandle) && Ludus::UI::Widgets::MenuItem("Sprite 2D"))
				{
					Commands::EnqueueEdit(context.Shell.State.Commands, Commands::EditCommand::AddComponent<Component::Sprite2DComponent> {
						.EntityReference = entityHandle, .SceneHandle = sceneHandle
					});
					isComponentAdded = true;
				}

				if (!ecs.Texts.ContainsOwner(entityHandle) && Ludus::UI::Widgets::MenuItem("Text 2D"))
				{
					Commands::EnqueueEdit(context.Shell.State.Commands, Commands::EditCommand::AddComponent<Component::Text2DComponent> {
						.EntityReference = entityHandle, .SceneHandle = sceneHandle
					});
					isComponentAdded = true;
				}

				if (isComponentAdded)
				{
					Commands::EnqueueEdit(context.Shell.State.Commands, Commands::EditCommand::SelectEntity { .EntityReference = entityHandle });
				}
			}
		}
	}

	void HierarchyPanel::DrawSceneContextMenu(Ludus::Editor::Core::ProjectSessionContext& context, Ludus::Engine::Core::Scene& scene)
	{
		auto& ecs = scene.EntityComponentSystem;

		auto flags = Ludus::UI::Flags::Popup::MouseButtonRight | Ludus::UI::Flags::Popup::NoOpenOverItems;
		if (Ludus::UI::Scope::PopupContextWindowScope contextWindow("HierarchyWindowContext", flags); contextWindow)
		{
			auto entityReference = Commands::EntityReference::Temporary(Commands::TempReference { m_Random.NextId() });
			const auto sceneHandle = scene.Handle;
			bool isEntityAdded = false;

			auto enqueueAddEntity = [&](const Commands::EntityReference& reference)
			{
				Commands::EnqueueEdit(context.Shell.State.Commands, Commands::EditCommand::AddEntity { .EntityReference = reference, .SceneHandle = sceneHandle });
			};

			auto enqueueAddComponent = [&](auto init)
			{
				using Component = std::decay_t<decltype(init)>;
				Commands::EnqueueEdit(context.Shell.State.Commands, Commands::EditCommand::AddComponent<Component> {
					.EntityReference = entityReference,
						.SceneHandle = sceneHandle,
						.Init = std::move(init)
				});
			};

			if (Ludus::UI::Widgets::MenuItem("Add empty"))
			{
				Component::Transform2DComponent transform;
				Component::DisplayNameComponent displayName("Empty");

				enqueueAddEntity(entityReference);
				enqueueAddComponent(std::move(transform));
				enqueueAddComponent(std::move(displayName));

				isEntityAdded = true;
			}

			if (Ludus::UI::Widgets::MenuItem("Add camera"))
			{
				Component::Transform2DComponent transform;
				Component::DisplayNameComponent displayName("Camera");
				Component::Camera2DComponent camera;

				enqueueAddEntity(entityReference);
				enqueueAddComponent(std::move(transform));
				enqueueAddComponent(std::move(displayName));
				enqueueAddComponent(std::move(camera));

				isEntityAdded = true;
			}

			if (Ludus::UI::Scope::MenuScope menu("Add sprite"); menu)
			{
				Ludus::Engine::Graphics::Shape shape { };
				std::string name;
				bool hasSprite = false;

				if (Ludus::UI::Widgets::MenuItem("Circle"))
				{
					name = "Circle";
					shape = Ludus::Engine::Graphics::Shape::Circle;
					hasSprite = true;
				}

				if (Ludus::UI::Widgets::MenuItem("Quad"))
				{
					name = "Quad";
					shape = Ludus::Engine::Graphics::Shape::Quad;
					hasSprite = true;
				}

				if (hasSprite)
				{
					Component::Transform2DComponent transform;
					Component::DisplayNameComponent displayName(std::move(name));
					Component::Sprite2DComponent sprite(shape);

					enqueueAddEntity(entityReference);
					enqueueAddComponent(std::move(transform));
					enqueueAddComponent(std::move(displayName));
					enqueueAddComponent(std::move(sprite));

					isEntityAdded = true;
				}
			}

			if (isEntityAdded)
			{
				Commands::EnqueueEdit(context.Shell.State.Commands, Commands::EditCommand::SelectEntity { .EntityReference = entityReference });
			}
		}
	}

	void HierarchyPanel::DrawSceneRow(Ludus::Editor::Core::ProjectSessionContext& context, Ludus::Engine::Core::Scene& scene)
	{
		auto sceneHeader = scene.Name;
		if (context.ProjectSession.IsSceneDirty())
		{
			sceneHeader.append("*");
		}

		const auto sceneLabel = Ludus::UI::CreateLabelWithIcon(ICON_CUBES, sceneHeader, scene.Name);
		if (Ludus::UI::Scope::TreeNodeScope treeScope(sceneLabel.c_str()); treeScope)
		{
			if (Ludus::UI::Scope::PopupContextItemScope contextItem; contextItem)
			{
				if (Ludus::UI::Widgets::MenuItem("Save"))
				{
					Commands::EnqueueRequest(context.Shell.State.Commands, Commands::RequestCommand::SaveScene { .SceneHandle = scene.Handle });
				}
			}

			DrawSceneContextMenu(context, scene);

			for (const auto& entity : scene.EntityComponentSystem.View())
			{
				DrawEntityRow(context, scene, entity.Handle);
			}

			if (Ludus::UI::Context::InputContext::IsMouseClicked(Ludus::Engine::Windowing::MouseButton::Left) &&
				Ludus::UI::Context::InputContext::IsWindowHovered(Ludus::UI::Flags::Hovered::AllowWhenBlockedByActiveItem) &&
				!Ludus::UI::Context::InputContext::IsAnyItemHovered())
			{
				Commands::EnqueueEdit(context.Shell.State.Commands, Commands::EditCommand::ClearSelection { });
			}
		}
	}

	bool HierarchyPanel::UpdateImpl(Ludus::Editor::Core::ProjectSessionContext& context)
	{
		auto windowTitle = CreateWindowTitleWithIcon(ICON_DIAGRAM_PROJECT, "Hierarchy");
		if (Ludus::UI::Scope::WindowScope window(windowTitle.c_str(), &m_Open, Ludus::Editor::Core::Constants::PanelFlags); window)
		{
			auto& registry = context.ProjectSession.GetSceneRegistry();

			const auto selectedScene = context.ProjectSession.EditorState.ActiveSceneHandle;
			if (!registry.Contains(selectedScene))
			{
				// No scene available.
				return true;
			}

			auto& scene = registry.GetScene(selectedScene);
			DrawSceneRow(context, scene);
		}

		return true;
	}
}
