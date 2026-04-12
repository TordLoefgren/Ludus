#include "pch.h"

#include <format>
#include <string>
#include <vector>

#include <Ludus/Editor/Commands/EditCommand.h>
#include <Ludus/Editor/Commands/Enqueue.h>
#include <Ludus/Editor/Core/Axis.h>
#include <Ludus/Editor/Core/Constants.h>
#include <Ludus/Editor/Panels/InspectorPanel.h>
#include <Ludus/Editor/Widgets/Input.h>
#include <Ludus/Engine/Components/Camera2DComponent.h>
#include <Ludus/Engine/Components/Collider2DComponent.h>
#include <Ludus/Engine/Components/DisplayNameComponent.h>
#include <Ludus/Engine/Components/RigidBody2DComponent.h>
#include <Ludus/Engine/Components/ScriptComponent.h>
#include <Ludus/Engine/Components/Sprite2DComponent.h>
#include <Ludus/Engine/Components/Text2DComponent.h>
#include <Ludus/Engine/Components/Transform2DComponent.h>
#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Core/Enums.h>
#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Core/SceneRegistry.h>
#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Graphics/HorizontalTextAlignment.h>
#include <Ludus/Engine/Graphics/Shape.h>
#include <Ludus/Engine/Physics/Core/LayerMask.h>
#include <Ludus/Engine/Runtime/RuntimeManifest.h>
#include <Ludus/UI/Context/LayoutContext.h>
#include <Ludus/UI/Context/PopupContext.h>
#include <Ludus/UI/Context/TableContext.h>
#include <Ludus/UI/Context/ThemeContext.h>
#include <Ludus/UI/Context/WindowContext.h>
#include <Ludus/UI/Elements/ActionTreeNode.h>
#include <Ludus/UI/Icons/FontAwesome.h>
#include <Ludus/UI/Labels.h>
#include <Ludus/UI/Scope/ComboScope.h>
#include <Ludus/UI/Scope/DisabledScope.h>
#include <Ludus/UI/Scope/FontScope.h>
#include <Ludus/UI/Scope/IDScope.h>
#include <Ludus/UI/Scope/MenuScope.h>
#include <Ludus/UI/Scope/StyleScope.h>
#include <Ludus/UI/Scope/TableScope.h>
#include <Ludus/UI/Scope/TreeNodeScope.h>
#include <Ludus/UI/Scope/WindowScope.h>
#include <Ludus/UI/Widgets/Buttons.h>
#include <Ludus/UI/Widgets/Color.h>
#include <Ludus/UI/Widgets/Headers.h>
#include <Ludus/UI/Widgets/Input.h>
#include <Ludus/UI/Widgets/Menu.h>
#include <Ludus/UI/Widgets/Selection.h>
#include <Ludus/UI/Widgets/Text.h>
#include <Ludus/UI/Widgets/Toggle.h>

namespace Ludus::Editor::Panels
{
	namespace
	{
		namespace Component = Ludus::Engine::Components;

		constexpr const char* AddComponentPopupId = "AddComponentPopup";
		constexpr float InspectorLabelColumnWidth = 130.0f;

		void SetupInspectorTable2()
		{
			Ludus::UI::Context::TableContext::TableSetupColumn("Label", Ludus::UI::Flags::TableColumn::WidthFixed, InspectorLabelColumnWidth);
			Ludus::UI::Context::TableContext::TableSetupColumn("Value", Ludus::UI::Flags::TableColumn::WidthStretch);
		}

		void DrawInspectorLabel(const char* text)
		{
			Ludus::UI::Context::LayoutContext::AlignTextToFramePadding();
			Ludus::UI::Widgets::TextUnformatted(text);
		}

		bool InputFloatFill(
			const std::string& label,
			float* value
		)
		{
			Ludus::UI::Context::LayoutContext::SetNextItemWidthFill();
			return Ludus::UI::Widgets::InputFloat(label, value);
		}

		bool InputIntFill(
			const std::string& label,
			int* value
		)
		{
			Ludus::UI::Context::LayoutContext::SetNextItemWidthFill();
			return Ludus::UI::Widgets::InputInt(label, value);
		}

		bool InputTextFill(
			const std::string& label,
			std::string& text
		)
		{
			Ludus::UI::Context::LayoutContext::SetNextItemWidthFill();
			return Ludus::UI::Widgets::InputText(label, text);
		}

		template<typename TEnum>
			requires Ludus::Engine::Core::Enums::HasValues<TEnum>
		bool ComboEnumFill(const char* label, TEnum& currentItem)
		{
			Ludus::UI::Context::LayoutContext::SetNextItemWidthFill();
			return Ludus::UI::Widgets::ComboEnum(label, currentItem);
		}

		bool ComboFill(const std::string& label, int* currentItem, const std::vector<const char*>& items)
		{
			Ludus::UI::Context::LayoutContext::SetNextItemWidthFill();
			return Ludus::UI::Widgets::Combo(label, currentItem, items);
		}

		template<typename TComponent>
		Ludus::UI::Elements::ActionTreeNode DrawComponentHeaderWithMenu(
			Ludus::Editor::Core::ProjectSessionContext& context,
			const char* label,
			TComponent component
		)
		{
			const std::string treeNodeId = std::string(label) + "_ActionTreeNode";
			const std::string menuId = std::string(label) + "_MenuButton";

			Ludus::UI::Elements::ActionTreeNodeOptions options {
				.Id = treeNodeId.c_str(),
				.MenuId = menuId.c_str(),
				.Label = label,
				.MenuButtonIcon = ICON_ELLIPSIS_V
			};
			Ludus::UI::Elements::ActionTreeNodeResult result;
			Ludus::UI::Elements::ActionTreeNode treeNode(options, &result);

			if (auto popup = treeNode.Menu(); popup)
			{
				if (Ludus::UI::Widgets::MenuItem("Remove"))
				{
					context.Shell.State.Commands.AddEditCommand(
						Ludus::Editor::Commands::EditCommand::RemoveComponent<TComponent>(
							context.ProjectSession.GetPresentedSceneId(),
							component.OwnerId
						)
					);

					Ludus::UI::Context::PopupContext::CloseCurrentPopup();
				}
			}

			return treeNode;
		}

		std::string BuildLayerMaskPreview(const Ludus::Engine::Physics::Core::LayerMask& mask)
		{
			const auto layers = Ludus::Engine::Physics::Core::LayerMask::GetLayersOrdered();

			std::vector<std::string> selected;
			selected.reserve(layers.size());

			for (const auto& layer : layers)
			{
				if (mask.Contains(layer.Index))
				{
					selected.emplace_back(layer.Name);
				}
			}

			if (selected.empty())
			{
				return "None";
			}

			if (selected.size() == layers.size())
			{
				return "All";
			}

			if (selected.size() <= 3)
			{
				std::string result;
				for (size_t i = 0; i < selected.size(); i++)
				{
					if (i > 0)
					{
						result += ", ";
					}

					result += selected[i];
				}

				return result;
			}

			return std::format("{} layers selected", selected.size());
		}
	}

#pragma region Draw calls

	bool InspectorPanel::DrawDisplayName(Component::DisplayNameComponent& component)
	{
		bool changed = false;

		if (Ludus::UI::Scope::TreeNodeScope treeNode("Display Name"); treeNode)
		{
			if (Ludus::UI::Scope::TableScope table("DisplayName_Panel", 2); table)
			{
				SetupInspectorTable2();

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				DrawInspectorLabel("Name");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				changed |= InputTextFill("##DisplayName_Panel_Name", component.Name);
			}
		}

		return changed;
	}

	bool InspectorPanel::DrawTransform2D(Component::Transform2DComponent& component)
	{
		bool changed = false;

		if (Ludus::UI::Scope::TreeNodeScope treeNode("Transform 2D"); treeNode)
		{
			if (Ludus::UI::Scope::TableScope table("Transform2D_Panel", 2); table)
			{
				SetupInspectorTable2();

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				DrawInspectorLabel("Position");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				changed |= Ludus::Editor::Widgets::DrawVector2Field("Transform2D_Panel_Position", &component.Position.X, &component.Position.Y);

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				DrawInspectorLabel("Scale");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				changed |= Ludus::Editor::Widgets::DrawVector2Field("Transform2D_Panel_Scale", &component.Scale.X, &component.Scale.Y);

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				DrawInspectorLabel("Rotation");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				changed |= Ludus::Editor::Widgets::DrawAxisFloatField("Transform2D_Panel_Rotation", &component.Rotation, Ludus::Editor::Core::Axis::Z);
			}
		}

		return changed;
	}

	bool InspectorPanel::DrawCamera2D(Ludus::Editor::Core::ProjectSessionContext& context, Component::Camera2DComponent& component)
	{
		bool changed = false;

		if (auto treeNode = DrawComponentHeaderWithMenu(context, "Camera 2D", component); treeNode)
		{
			if (Ludus::UI::Scope::TableScope table("Camera2D_Panel", 2); table)
			{
				SetupInspectorTable2();

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				DrawInspectorLabel("Orthographic Size");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				changed |= Ludus::Editor::Widgets::DrawAxisFloatField("Camera2D_Panel_OrthographicSize", &component.OrthographicSize, Ludus::Editor::Core::Axis::Y);

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				DrawInspectorLabel("Priority");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				changed |= InputIntFill("##Camera2D_Panel_Priority", &component.Priority);
			}
		}

		return changed;
	}

	bool InspectorPanel::DrawCollider2D(Ludus::Editor::Core::ProjectSessionContext& context, Component::Collider2DComponent& component)
	{
		bool changed = false;

		if (auto treeNode = DrawComponentHeaderWithMenu(context, "Collider 2D", component); treeNode)
		{
			if (Ludus::UI::Scope::TableScope table("##Collider2D_Panel", 2); table)
			{
				SetupInspectorTable2();

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				DrawInspectorLabel("Layer");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);

				const auto layers = Ludus::Engine::Physics::Core::LayerMask::GetLayersOrdered();
				auto values = std::vector<const char*> { };
				values.reserve(layers.size());

				auto currentLayerRow = 0;
				for (auto i = 0; i < static_cast<int>(layers.size()); i++)
				{
					values.push_back(layers[static_cast<size_t>(i)].Name.data());
					if (layers[static_cast<size_t>(i)].Index == component.LayerIndex)
					{
						currentLayerRow = i;
					}
				}

				if (ComboFill("##Collider2D_Panel_LayerIndex", &currentLayerRow, values))
				{
					component.LayerIndex = layers[static_cast<size_t>(currentLayerRow)].Index;
					changed = true;
				}

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				DrawInspectorLabel("Collides With");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);

				const auto preview = BuildLayerMaskPreview(component.CollidesWith);

				Ludus::UI::Context::LayoutContext::SetNextItemWidthFill();

				if (Ludus::UI::Scope::ComboScope combo("##Collider2D_Panel_CollidesWith", preview.c_str()); combo)
				{
					auto newMask = component.CollidesWith;

					for (const auto& layerEntry : Ludus::Engine::Physics::Core::LayerMask::GetLayersOrdered())
					{
						auto layer = Ludus::Engine::Physics::Core::LayerMask::FromIndex(layerEntry.Index);
						auto label = std::string(layerEntry.Name);
						auto isChecked = newMask.Contains(layer);

						auto item = Ludus::UI::Widgets::CheckboxItem(label, isChecked);
						if (Ludus::UI::Widgets::Checkbox(item))
						{
							changed = true;
							newMask = item.IsChecked ? (newMask |= layer) : (newMask &= ~layer);
						}
					}

					component.CollidesWith = newMask;
				}

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				DrawInspectorLabel("Is Trigger");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);

				changed |= Ludus::UI::Widgets::Checkbox("##Collider2D_Panel_IsTrigger", &component.IsTrigger);
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
			}
		}

		return changed;
	}

	bool InspectorPanel::DrawRigidBody2D(Ludus::Editor::Core::ProjectSessionContext& context, Component::RigidBody2DComponent& component)
	{
		bool changed = false;

		if (auto treeNode = DrawComponentHeaderWithMenu(context, "Rigidbody 2D", component); treeNode)
		{
			if (Ludus::UI::Scope::TableScope table("RigidBody2D_Panel", 2); table)
			{
				SetupInspectorTable2();

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				DrawInspectorLabel("Velocity");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				changed |= Ludus::Editor::Widgets::DrawVector2Field("RigidBody2D_Panel_Velocity", &component.Velocity.X, &component.Velocity.Y);

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				DrawInspectorLabel("Body Type");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				changed |= ComboEnumFill("##RigidBody2D_Panel_BodyType", component.BodyType);

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				DrawInspectorLabel("Gravity Scale");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				changed |= InputFloatFill("##RigidBody2D_Panel_GravityScale", &component.GravityScale);

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				DrawInspectorLabel("Mass");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				changed |= InputFloatFill("##RigidBody2D_Panel_Mass", &component.Mass);
			}
		}

		return changed;
	}

	bool InspectorPanel::DrawScript(
		Ludus::Editor::Core::ProjectSessionContext& context,
		Component::ScriptComponent& component
	)
	{
		bool changed = false;

		if (auto treeNode = DrawComponentHeaderWithMenu(context, "Script", component); treeNode)
		{
			if (Ludus::UI::Scope::TableScope table("Script_Panel", 2); table)
			{
				SetupInspectorTable2();

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				DrawInspectorLabel("Name");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);

				const auto& scriptReferences = context.ProjectSession.Persistence.GetScripts();
				if (scriptReferences.empty())
				{
					const auto noneValues = { "None" };
					auto currentIndex = 0;

					Ludus::UI::Scope::DisabledScope disabled(true);
					auto _ = ComboFill("##Script_Panel_Name", &currentIndex, noneValues);
				}
				else
				{
					// The variable passed to Combo must be an integer. 
					auto currentIndex = -1;

					for (auto i = 0; i < static_cast<int>(scriptReferences.size()); i++)
					{
						const auto& reference = scriptReferences[static_cast<size_t>(i)];
						if (reference.Id == component.Id)
						{
							currentIndex = i;
							break;
						}
					}

					if (currentIndex < 0)
					{
						const auto missingValues = { "Missing Script Reference" };
						auto missingIndex = 0;

						Ludus::UI::Scope::DisabledScope disabled(true);
						auto _ = ComboFill("##Script_Panel_Name", &missingIndex, missingValues);
					}
					else
					{
						auto items = Ludus::UI::Widgets::GetCStringItems(scriptReferences, [](const Ludus::Engine::Runtime::ScriptReference& item)
						{
							return item.Name.c_str();
						});

						if (ComboFill("##Script_Panel_Name", &currentIndex, items))
						{
							const auto& selected = scriptReferences[static_cast<size_t>(currentIndex)];
							component.Id = selected.Id;

							changed = true;
						}
					}
				}
			}
		}

		return changed;
	}

	bool InspectorPanel::DrawSprite2D(Ludus::Editor::Core::ProjectSessionContext& context, Component::Sprite2DComponent& component)
	{
		bool changed = false;

		if (auto treeNode = DrawComponentHeaderWithMenu(context, "Sprite 2D", component); treeNode)
		{
			if (Ludus::UI::Scope::TableScope table("Sprite2D_Panel", 2); table)
			{
				SetupInspectorTable2();

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				DrawInspectorLabel("Shape");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				changed |= ComboEnumFill("##Sprite2D_Panel_Shape", component.Shape);

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				DrawInspectorLabel("Color");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				changed |= Ludus::UI::Widgets::ColorEdit4("##Sprite2D_Panel_Color", component.Color.GetData());

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				DrawInspectorLabel("Fill");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				changed |= Ludus::UI::Widgets::Checkbox("##Sprite2D_Panel_Fill", &component.Fill);
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
			}
		}

		return changed;
	}

	bool InspectorPanel::DrawText2D(Ludus::Editor::Core::ProjectSessionContext& context, Component::Text2DComponent& component)
	{
		bool changed = false;

		if (auto treeNode = DrawComponentHeaderWithMenu(context, "Text 2D", component); treeNode)
		{
			if (Ludus::UI::Scope::TableScope table("Text2D_Panel", 2); table)
			{
				SetupInspectorTable2();

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				DrawInspectorLabel("Text");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				changed |= InputTextFill("##Text2D_Panel_Text", component.Text);

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				DrawInspectorLabel("Color");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				changed |= Ludus::UI::Widgets::ColorEdit4("##Text2D_Panel_Color", component.Color.GetData());

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				DrawInspectorLabel("Horizontal Alignment");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				changed |= ComboEnumFill("##Text2D_Panel_HorizontalTextAlignment", component.HorizontalTextAlignment);
			}
		}

		return changed;
	}

#pragma endregion

	bool InspectorPanel::UpdateImpl(Ludus::Editor::Core::ProjectSessionContext& context)
	{
		auto windowTitle = CreateWindowTitleWithIcon(ICON_SLIDERS, "Inspector");
		if (Ludus::UI::Scope::WindowScope window(windowTitle.c_str(), &m_Open, Ludus::Editor::Core::Constants::PanelFlags); window)
		{
			auto& selection = context.ProjectSession.EditorState.GetSelection();
			if (!selection.HasEntity())
			{
				return true;
			}

			auto& registry = context.ProjectSession.RuntimeState.GetActiveSceneRegistry();
			auto activeSceneId = context.ProjectSession.GetPresentedSceneId();
			if (!registry.Contains(activeSceneId))
			{
				return true;
			}

			auto& scene = registry.GetScene(activeSceneId);

			auto& ecs = scene.EntityComponentSystem;
			auto entityId = *selection.SelectedEntityId;
			if (!ecs.IndexOf(entityId))
			{
				context.Shell.State.Commands.AddEditCommand(
					Ludus::Editor::Commands::EditCommand::ClearSelection { }
				);
				return true;
			}

			bool changed = false;

			// Draw required components.
			auto* transformPtr = ecs.Transforms.TryGetByOwnerMutable(entityId);
			auto* displayNamePtr = ecs.DisplayNames.TryGetByOwnerMutable(entityId);

			if (!displayNamePtr || !transformPtr)
			{
				throw std::runtime_error("DisplayName and Transform cannot be null.");
			}

			changed |= DrawDisplayName(*displayNamePtr);
			changed |= DrawTransform2D(*transformPtr);

			// Draw remaining components.
			auto* colliderPtr = ecs.Colliders.TryGetByOwnerMutable(entityId);
			auto* rigidBodyPtr = ecs.RigidBodies.TryGetByOwnerMutable(entityId);
			auto* scriptPtr = ecs.Scripts.TryGetByOwnerMutable(entityId);
			auto* spritePtr = ecs.Sprites.TryGetByOwnerMutable(entityId);
			auto* textPtr = ecs.Texts.TryGetByOwnerMutable(entityId);
			auto* cameraPtr = ecs.Cameras.TryGetByOwnerMutable(entityId);

			if (cameraPtr)
			{
				changed |= DrawCamera2D(context, *cameraPtr);
			}

			if (colliderPtr)
			{
				changed |= DrawCollider2D(context, *colliderPtr);
			}

			if (rigidBodyPtr)
			{
				changed |= DrawRigidBody2D(context, *rigidBodyPtr);
			}

			if (scriptPtr)
			{
				changed |= DrawScript(context, *scriptPtr);
			}

			if (spritePtr)
			{
				changed |= DrawSprite2D(context, *spritePtr);
			}

			if (textPtr)
			{
				changed |= DrawText2D(context, *textPtr);
			}

			if (changed)
			{
				context.ProjectSession.MarkActiveSceneDirty();
			}

			Ludus::UI::Context::LayoutContext::Separator();

			{
				auto hasAllComponents = colliderPtr && rigidBodyPtr && scriptPtr && spritePtr && textPtr && cameraPtr;
				Ludus::UI::Scope::DisabledScope disabled(hasAllComponents);

				const auto addComponentLabel = "Add Component";
				const auto buttonWidth = Ludus::UI::Context::LayoutContext::CalculateTextSize(addComponentLabel).X;
				const auto availableWidth = Ludus::UI::Context::WindowContext::GetContentRegionAvailable().X;
				Ludus::UI::Context::WindowContext::SetCursorPositionX((availableWidth - buttonWidth) * 0.5f);

				if (Ludus::UI::Widgets::Button(addComponentLabel))
				{
					const auto min = Ludus::UI::Context::LayoutContext::GetItemRectMin();
					const auto max = Ludus::UI::Context::LayoutContext::GetItemRectMax();
					const auto width = max.X - min.X;
					Ludus::UI::Context::WindowContext::SetNextWindowPosition({ min.X, max.Y });
					Ludus::UI::Context::WindowContext::SetNextWindowSize({ width, 0.0f });

					Ludus::UI::Context::PopupContext::OpenPopup(AddComponentPopupId);
				}
			}

			if (Ludus::UI::Scope::PopupScope componentMenu(AddComponentPopupId); componentMenu)
			{
				auto isComponentAdded = false;

				if (!ecs.Cameras.ContainsOwner(entityId) && Ludus::UI::Widgets::MenuItem("Camera 2D"))
				{
					Commands::EnqueueEdit(context.Shell.State.Commands, Commands::EditCommand::AddComponent<Component::Camera2DComponent> {
						.SceneId = activeSceneId, .EntityReference = entityId
					});
					isComponentAdded = true;
				}

				if (!ecs.Colliders.ContainsOwner(entityId) && Ludus::UI::Widgets::MenuItem("Collider 2D"))
				{
					Commands::EnqueueEdit(context.Shell.State.Commands, Commands::EditCommand::AddComponent<Component::Collider2DComponent> {
						.SceneId = activeSceneId, .EntityReference = entityId
					});
					isComponentAdded = true;
				}

				if (!ecs.RigidBodies.ContainsOwner(entityId) && Ludus::UI::Widgets::MenuItem("Rigid Body 2D"))
				{
					Commands::EnqueueEdit(context.Shell.State.Commands, Commands::EditCommand::AddComponent<Component::RigidBody2DComponent> {
						.SceneId = activeSceneId, .EntityReference = entityId
					});
					isComponentAdded = true;
				}

				if (!ecs.Scripts.ContainsOwner(entityId) && Ludus::UI::Widgets::MenuItem("Script"))
				{
					Commands::EnqueueUI(context.Shell.State.Commands, Commands::UICommand::OpenAddScriptDialog {
						.SceneId = activeSceneId, .EntityId = entityId
						});
					// The added component will be selected later as part of the command chain.
				}

				if (!ecs.Sprites.ContainsOwner(entityId) && Ludus::UI::Widgets::MenuItem("Sprite 2D"))
				{
					Commands::EnqueueEdit(context.Shell.State.Commands, Commands::EditCommand::AddComponent<Component::Sprite2DComponent> {
						.SceneId = activeSceneId, .EntityReference = entityId
					});
					isComponentAdded = true;
				}

				if (!ecs.Texts.ContainsOwner(entityId) && Ludus::UI::Widgets::MenuItem("Text 2D"))
				{
					Commands::EnqueueEdit(context.Shell.State.Commands, Commands::EditCommand::AddComponent<Component::Text2DComponent> {
						.SceneId = activeSceneId, .EntityReference = entityId
					});
					isComponentAdded = true;
				}

				if (isComponentAdded)
				{
					Commands::EnqueueEdit(context.Shell.State.Commands, Commands::EditCommand::SelectEntity { .EntityReference = entityId });
					Ludus::UI::Context::PopupContext::CloseCurrentPopup();
				}
			}
		}

		return true;
	}
}
