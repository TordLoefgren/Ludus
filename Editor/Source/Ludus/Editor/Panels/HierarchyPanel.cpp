#include "pch.h"

#include <format>
#include <string>
#include <type_traits>
#include <utility>

#include <Ludus/Editor/Commands/Enqueue.h>
#include <Ludus/Editor/Core/Constants.h>
#include <Ludus/Editor/Panels/HierarchyPanel.h>
#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Core/SceneRegistry.h>
#include <Ludus/Engine/Debug/Debug.h>
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

		bool MenuItemConditional(const char* label, bool enabled)
		{
			return Ludus::UI::Widgets::MenuItem(label, nullptr, false, enabled);
		}
	}

	void HierarchyPanel::DrawEntityRow(Ludus::Editor::Core::ProjectSessionContext& context, Ludus::Engine::Core::Scene& scene, Ludus::Engine::Core::EntityId entityId)
	{
		auto& selection = context.ProjectSession.EditorState.GetSelection();
		auto& ecs = scene.EntityComponentSystem;
		const auto sceneId = scene.Id;

		const auto* displayNamePtr = ecs.DisplayNames.TryGetByOwner(entityId);
		LUDUS_ASSERT(displayNamePtr != nullptr, "An entity must have a DisplayNameComponent.");
		const auto entityLabel = displayNamePtr != nullptr
			? Ludus::UI::CreateLabelWithIcon(ICON_CUBE, displayNamePtr->Name, std::to_string(entityId.Value))
			: Ludus::UI::CreateLabelWithIcon(ICON_CUBE, std::format("Entity {}", entityId.Value), std::to_string(entityId.Value));

		if (Ludus::UI::Widgets::Selectable(entityLabel.c_str(), selection.IsSelected(entityId)))
		{
			Commands::EnqueueEdit(context.Shell.State.Commands, Commands::EditCommand::SelectEntity { entityId });
		}

		if (Ludus::UI::Scope::PopupContextItemScope contextItem; contextItem)
		{
			if (Ludus::UI::Widgets::MenuItem("Select"))
			{
				Commands::EnqueueEdit(context.Shell.State.Commands, Commands::EditCommand::SelectEntity { entityId });
			}

			Ludus::UI::Context::LayoutContext::Separator();

			if (Ludus::UI::Widgets::MenuItem("Remove"))
			{
				Commands::EnqueueEdit(context.Shell.State.Commands, Commands::EditCommand::RemoveEntity { .SceneId = sceneId, .EntityReference = entityId });
				Commands::EnqueueEdit(context.Shell.State.Commands, Commands::EditCommand::DeselectEntity { entityId });
			}

			Ludus::UI::Context::LayoutContext::Separator();

			if (Ludus::UI::Scope::MenuScope componentMenu("Add component"); componentMenu)
			{
				auto isComponentAdded = false;

				if (MenuItemConditional("Camera 2D", !ecs.Cameras.ContainsOwner(entityId)))
				{
					Commands::EnqueueEdit(context.Shell.State.Commands, Commands::EditCommand::AddComponent<Component::Camera2DComponent> {
						.SceneId = sceneId, .EntityReference = entityId
					});
					isComponentAdded = true;
				}

				if (MenuItemConditional("Collider 2D", !ecs.Colliders.ContainsOwner(entityId)))
				{
					Commands::EnqueueEdit(context.Shell.State.Commands, Commands::EditCommand::AddComponent<Component::Collider2DComponent> {
						.SceneId = sceneId, .EntityReference = entityId
					});
					isComponentAdded = true;
				}

				if (MenuItemConditional("Rigidbody 2D", !ecs.RigidBodies.ContainsOwner(entityId)))
				{
					Commands::EnqueueEdit(context.Shell.State.Commands, Commands::EditCommand::AddComponent<Component::RigidBody2DComponent> {
						.SceneId = sceneId, .EntityReference = entityId
					});
					isComponentAdded = true;
				}

				if (MenuItemConditional("Script", !ecs.Scripts.ContainsOwner(entityId)))
				{
					Commands::EnqueueUI(context.Shell.State.Commands, Commands::UICommand::OpenAddScriptDialog {
						.SceneId = sceneId, .EntityId = entityId
						});
					// The added component will be selected later as part of the command chain.
				}

				if (MenuItemConditional("Sprite 2D", !ecs.Sprites.ContainsOwner(entityId)))
				{
					Commands::EnqueueEdit(context.Shell.State.Commands, Commands::EditCommand::AddComponent<Component::Sprite2DComponent> {
						.SceneId = sceneId, .EntityReference = entityId
					});
					isComponentAdded = true;
				}

				if (MenuItemConditional("Text 2D", !ecs.Texts.ContainsOwner(entityId)))
				{
					Commands::EnqueueEdit(context.Shell.State.Commands, Commands::EditCommand::AddComponent<Component::Text2DComponent> {
						.SceneId = sceneId, .EntityReference = entityId
					});
					isComponentAdded = true;
				}

				if (isComponentAdded)
				{
					Commands::EnqueueEdit(context.Shell.State.Commands, Commands::EditCommand::SelectEntity { .EntityReference = entityId });
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
			const auto sceneId = scene.Id;
			bool isEntityAdded = false;

			auto enqueueAddEntity = [&](const Commands::EntityReference& reference)
			{
				Commands::EnqueueEdit(context.Shell.State.Commands, Commands::EditCommand::AddEntity { .SceneId = sceneId, .EntityReference = reference });
			};

			auto enqueueAddComponent = [&](auto init)
			{
				using Component = std::decay_t<decltype(init)>;
				Commands::EnqueueEdit(context.Shell.State.Commands, Commands::EditCommand::AddComponent<Component> {
					.SceneId = sceneId, .EntityReference = entityReference, .Init = std::move(init)
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
		if (context.ProjectSession.EditorState.IsSceneDirty() && !context.ProjectSession.RuntimeState.IsSimulationActive())
		{
			sceneHeader.append("*");
		}

		const auto sceneLabel = Ludus::UI::CreateLabelWithIcon(ICON_CUBES, sceneHeader, scene.Name);
		if (Ludus::UI::Scope::TreeNodeScope treeScope(sceneLabel.c_str()); treeScope)
		{
			if (Ludus::UI::Scope::PopupContextItemScope contextItem; contextItem)
			{
				if (Ludus::UI::Widgets::MenuItem("Rename"))
				{
					Commands::EnqueueUI(context.Shell.State.Commands, Commands::UICommand::OpenRenameSceneDialog { .SceneId = scene.Id });
				}

				if (Ludus::UI::Widgets::MenuItem("Save"))
				{
					Commands::EnqueueRequest(context.Shell.State.Commands, Commands::RequestCommand::SaveScene { .SceneId = scene.Id });
				}
			}

			DrawSceneContextMenu(context, scene);

			for (const auto& entity : scene.EntityComponentSystem.View())
			{
				DrawEntityRow(context, scene, entity.Id);
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
			auto& registry = context.ProjectSession.RuntimeState.GetActiveSceneRegistry();

			const auto selectedScene = context.ProjectSession.RuntimeState.GetActiveScenePresentationState().CurrentSceneId;
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
