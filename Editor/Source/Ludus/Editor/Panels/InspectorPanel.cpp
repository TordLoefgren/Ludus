#include "pch.h"

#include <format>

#include <Ludus/Editor/Commands/EditCommand.h>
#include <Ludus/Editor/Core/Constants.h>
#include <Ludus/Editor/Panels/InspectorPanel.h>
#include <Ludus/Engine/Components/Camera2DComponent.h>
#include <Ludus/Engine/Components/Collider2DComponent.h>
#include <Ludus/Engine/Components/DisplayNameComponent.h>
#include <Ludus/Engine/Components/RigidBody2DComponent.h>
#include <Ludus/Engine/Components/ScriptComponent.h>
#include <Ludus/Engine/Components/Sprite2DComponent.h>
#include <Ludus/Engine/Components/Text2DComponent.h>
#include <Ludus/Engine/Components/Transform2DComponent.h>
#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Core/ProjectContext.h>
#include <Ludus/Engine/Core/SceneRegistry.h>
#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Graphics/HorizontalTextAlignment.h>
#include <Ludus/Engine/Graphics/Shape.h>
#include <Ludus/Engine/Physics/Core/LayerMask.h>
#include <Ludus/UI/Context/LayoutContext.h>
#include <Ludus/UI/Context/TableContext.h>
#include <Ludus/UI/Context/WindowContext.h>
#include <Ludus/UI/Scope/DisabledScope.h>
#include <Ludus/UI/Scope/MenuScope.h>
#include <Ludus/UI/Scope/TableScope.h>
#include <Ludus/UI/Scope/TreeNodeScope.h>
#include <Ludus/UI/Scope/WindowScope.h>
#include <Ludus/UI/Widgets/Color.h>
#include <Ludus/UI/Widgets/Headers.h>
#include <Ludus/UI/Widgets/Input.h>
#include <Ludus/UI/Widgets/Menu.h>
#include <Ludus/UI/Widgets/Selection.h>
#include <Ludus/UI/Widgets/Text.h>
#include <Ludus/UI/Widgets/Toggle.h>

namespace Ludus::Editor::Panels
{

#pragma region Draw calls

	void InspectorPanel::DrawEntityHandle(Ludus::Engine::Core::EntityHandle handle)
	{
		if (Ludus::UI::Scope::TreeNodeScope treeNode("Id"); treeNode)
		{
			if (Ludus::UI::Scope::TableScope table("Id_Panel", 2); table)
			{
				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Value");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				Ludus::UI::Widgets::TextUnformatted(std::to_string(handle));
			}
		}
	}

	void InspectorPanel::DrawDisplayName(Ludus::Engine::Components::DisplayNameComponent& component)
	{
		if (Ludus::UI::Scope::TreeNodeScope treeNode("Display Name"); treeNode)
		{
			if (Ludus::UI::Scope::TableScope table("DisplayName_Panel", 2); table)
			{
				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Value");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				Ludus::UI::Widgets::InputText("##DisplayName_Panel_Value", component.Value);
			}
		}
	}

	void InspectorPanel::DrawTransform2D(Ludus::Engine::Components::Transform2DComponent& component)
	{
		if (Ludus::UI::Scope::TreeNodeScope treeNode("Transform 2D"); treeNode)
		{
			if (Ludus::UI::Scope::TableScope table("Transform2D_Panel", 3); table)
			{
				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Position");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				Ludus::UI::Widgets::DragFloatLabelButton("X##Transform2D_Panel_Position_X", &component.Position.X);
				Ludus::UI::Context::LayoutContext::SameLine();
				Ludus::UI::Widgets::InputFloat("##Transform2D_Panel_Input_Position_X", &component.Position.X);

				Ludus::UI::Context::TableContext::TableSetColumnIndex(2);
				Ludus::UI::Widgets::DragFloatLabelButton("Y##Transform2D_Panel_Position_Y", &component.Position.Y);
				Ludus::UI::Context::LayoutContext::SameLine();
				Ludus::UI::Widgets::InputFloat("##Transform2D_Panel_Input_Position_Y", &component.Position.Y);

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Scale");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				Ludus::UI::Widgets::DragFloatLabelButton("X##Transform2D_Panel_Scale_X", &component.Scale.X);
				Ludus::UI::Context::LayoutContext::SameLine();
				Ludus::UI::Widgets::InputFloat("##Transform2D_Panel_Input_Scale_X", &component.Scale.X);

				Ludus::UI::Context::TableContext::TableSetColumnIndex(2);
				Ludus::UI::Widgets::DragFloatLabelButton("Y##Transform2D_Panel_Scale_Y", &component.Scale.Y);
				Ludus::UI::Context::LayoutContext::SameLine();
				Ludus::UI::Widgets::InputFloat("##Transform2D_Panel_Input_Scale_Y", &component.Scale.Y);

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Rotation");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				Ludus::UI::Widgets::DragFloatLabelButton("Z##Transform2D_Panel_DragFloat_Rotation", &component.Rotation);
				Ludus::UI::Context::LayoutContext::SameLine();
				Ludus::UI::Widgets::InputFloat("##Transform2D_Panel_InputFloat_Rotation_Z", &component.Rotation);
			}
		}
	}

	void InspectorPanel::DrawCollider2D(Ludus::Engine::Components::Collider2DComponent& component)
	{
		if (Ludus::UI::Scope::TreeNodeScope treeNode("Collider 2D"); treeNode)
		{
			if (Ludus::UI::Scope::TableScope table("##Collider2D_Panel", 3); table)
			{
				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Layer Index");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				Ludus::UI::Widgets::InputUInt8("##Collider2D_Panel_LayerIndex", &component.LayerIndex);
				Ludus::UI::Context::TableContext::TableSetColumnIndex(2);
				Ludus::UI::Widgets::TextUnformatted(Ludus::Engine::Physics::Core::LayerMask::LayerIndexToName(component.LayerIndex));

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Collides With");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);

				const auto count = Ludus::Engine::Physics::Core::LayerMask::GetLayerCount();
				if (Ludus::UI::Widgets::CollapsingHeader("##Collider2D_Panel_CollidesWith"))
				{
					auto newMask = component.CollidesWith;
					auto hasChanges = false;

					for (size_t i = 0; i < count; i++)
					{
						auto layer = Ludus::Engine::Physics::Core::LayerMask::FromIndex(static_cast<uint8_t>(i));
						auto label = Ludus::Engine::Physics::Core::LayerMask::LayerIndexToName(static_cast<uint8_t>(i));
						auto isChecked = component.CollidesWith.Contains(layer);

						auto item = Ludus::UI::Widgets::CheckboxItem(std::format("{} {}", i, label), isChecked);
						if (Ludus::UI::Widgets::Checkbox(item))
						{
							hasChanges = true;
							newMask = item.IsChecked ? newMask |= layer : newMask &= ~layer;
						}
					}

					if (hasChanges)
					{
						component.CollidesWith = newMask;
					}
				}

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Is Trigger");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				Ludus::UI::Widgets::Checkbox("##IsTrigger", &component.IsTrigger);
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
			}
		}
	}

	void InspectorPanel::DrawRigidBody2D(Ludus::Engine::Components::RigidBody2DComponent& component)
	{
		if (Ludus::UI::Scope::TreeNodeScope treeNode("Rigid Body 2D"); treeNode)
		{
			if (Ludus::UI::Scope::TableScope table("RigidBody2D_Panel", 3); table)
			{
				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Velocity");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				Ludus::UI::Widgets::DragFloatLabelButton("X##RigidBody2D_Panel_Velocity_X", &component.Velocity.X);
				Ludus::UI::Context::LayoutContext::SameLine();
				Ludus::UI::Widgets::InputFloat("##RigidBody2D_Panel_Input_Velocity_X", &component.Velocity.X);

				Ludus::UI::Context::TableContext::TableSetColumnIndex(2);
				Ludus::UI::Widgets::DragFloatLabelButton("Y##RigidBody2D_Panel_Velocity_Y", &component.Velocity.Y);
				Ludus::UI::Context::LayoutContext::SameLine();
				Ludus::UI::Widgets::InputFloat("##RigidBody2D_Panel_Input_Velocity_Y", &component.Velocity.Y);

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Body Type");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);

				auto _ = Ludus::UI::Widgets::ComboEnum("##RigidBody2D_Combo", component.Type);

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Gravity Scale");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				Ludus::UI::Widgets::InputFloat("##RigidBody2D_Panel_GravityScale", &component.GravityScale);

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Mass");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				Ludus::UI::Widgets::InputFloat("##RigidBody2D_Panel_Mass", &component.Mass);
			}
		}
	}

	void InspectorPanel::DrawScript(
		Ludus::Engine::Components::ScriptComponent& component,
		const Ludus::Engine::Core::ProjectContext& projectContext
	)
	{
		if (Ludus::UI::Scope::TreeNodeScope treeNode("Script"); treeNode)
		{
			if (Ludus::UI::Scope::TableScope table("Script_Panel", 2); table)
			{
				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Handle");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				Ludus::UI::Widgets::TextUnformatted(std::to_string(component.Handle));

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Name");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);

				const auto& scriptReferences = projectContext.Project.Scripts;
				if (scriptReferences.empty())
				{
					const auto noneValues = { "None" };
					auto currentIndex = 0;

					Ludus::UI::Scope::DisabledScope disabled(true);
					auto _ = Ludus::UI::Widgets::Combo("##Script_Panel_Name", &currentIndex, noneValues);
				}
				else
				{
					// The variable passed to Combo must be an integer. 
					auto currentIndex = -1;

					// Check for handle.
					for (auto i = 0; i < static_cast<int>(scriptReferences.size()); i++)
					{
						const auto& reference = scriptReferences[static_cast<size_t>(i)];
						if (reference.Handle == component.Handle)
						{
							currentIndex = i;
							break;
						}
					}

					// Check for name.
					if (currentIndex < 0)
					{
						for (auto i = 0; i < static_cast<int>(scriptReferences.size()); i++)
						{
							const auto& reference = scriptReferences[static_cast<size_t>(i)];
							if (reference.Name == component.Name)
							{
								currentIndex = i;
								break;
							}
						}
					}

					// Default to first item.
					if (currentIndex < 0)
					{
						currentIndex = 0;
					}

					auto items = Ludus::UI::Widgets::GetCStringItems(scriptReferences, [](const Ludus::Engine::Core::ProjectScriptReference& item)
					{
						return item.Name.c_str();
					});

					if (Ludus::UI::Widgets::Combo("##Script_Panel_Name", &currentIndex, items))
					{
						const auto& selected = scriptReferences[static_cast<size_t>(currentIndex)];
						component.Name = selected.Name;
						component.Handle = selected.Handle;
					}
				}
			}
		}
	}

	void InspectorPanel::DrawSprite2D(Ludus::Engine::Components::Sprite2DComponent& component)
	{
		if (Ludus::UI::Scope::TreeNodeScope treeNode("Sprite 2D"); treeNode)
		{
			if (Ludus::UI::Scope::TableScope table("Sprite2D_Panel", 3); table)
			{
				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Shape");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);

				auto _ = Ludus::UI::Widgets::ComboEnum("##Sprite2D_Panel_Combo", component.Shape);

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Color");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				Ludus::UI::Widgets::ColorEdit4("##Sprite2D_Panel_Color", component.Color.GetData());

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Texture");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				Ludus::UI::Widgets::TextUnformatted("N/A");

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Fill");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				Ludus::UI::Widgets::Checkbox("##Fill", &component.Fill);
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
			}
		}
	}

	void InspectorPanel::DrawText2D(Ludus::Engine::Components::Text2DComponent& component)
	{
		if (Ludus::UI::Scope::TreeNodeScope treeNode("Text 2D"); treeNode)
		{
			if (Ludus::UI::Scope::TableScope table("Text2D_Panel", 3); table)
			{
				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Text");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				Ludus::UI::Widgets::InputText("##Text2D_Panel_Text", component.Text);

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Color");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				Ludus::UI::Widgets::ColorEdit4("##Text2D_Panel_Color", component.Color.GetData());

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Horizontal Alignment");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);

				auto _ = Ludus::UI::Widgets::ComboEnum("##Text2D_Panel_Combo", component.HorizontalAlignment);
			}
		}
	}

	void InspectorPanel::DrawCamera2D(Ludus::Engine::Components::Camera2DComponent& component)
	{
		if (Ludus::UI::Scope::TreeNodeScope treeNode("Camera 2D"); treeNode)
		{
			if (Ludus::UI::Scope::TableScope table("Camera2D_Panel", 2); table)
			{
				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Orthographic Size");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				Ludus::UI::Widgets::DragFloatLabelButton("Z##Camera2D_OrthographicSize_DragFloat_Rotation", &component.OrthographicSize);
				Ludus::UI::Context::LayoutContext::SameLine();
				Ludus::UI::Widgets::InputFloat("##Camera2D_OrthographicSize_InputFloat_Rotation_Z", &component.OrthographicSize);
			}
		}
	}

#pragma endregion

	bool InspectorPanel::UpdateImpl(Ludus::Editor::Panels::PanelContext& context)
	{
		auto windowTitle = CreateWindowTitle("Inspector");
		if (Ludus::UI::Scope::WindowScope window(windowTitle.c_str(), &m_Open, Ludus::Editor::Core::Constants::PanelFlags); window)
		{
			auto& selection = context.EditorContext.State.Selection;
			if (!selection.HasEntity())
			{
				return true;
			}

			auto& registry = context.SystemContext.SceneRegistry;
			auto& session = context.EditorContext.Session;

			auto active = session.GetActiveScene();
			if (!active.has_value() || !registry.Contains(active.value()))
			{
				return true;
			}

			auto& scene = registry.GetScene(active.value());

			auto& ecs = scene.EntityComponentSystem;
			auto handle = selection.SelectedEntity.value();
			const bool existsInActiveScene = ecs.IndexOf(handle).has_value();
			if (!existsInActiveScene)
			{
				context.EditorContext.State.Commands.AddEditCommand(
					Ludus::Editor::Commands::EditCommand::ClearSelection { }
				);
				return true;
			}

			auto* transformPtr = ecs.Transforms.TryGetByOwnerMutable(handle);
			auto* displayNamePtr = ecs.DisplayNames.TryGetByOwnerMutable(handle);
			auto* colliderPtr = ecs.Colliders.TryGetByOwnerMutable(handle);
			auto* rigidBodyPtr = ecs.RigidBodies.TryGetByOwnerMutable(handle);
			auto* scriptPtr = ecs.Scripts.TryGetByOwnerMutable(handle);
			auto* spritePtr = ecs.Sprites.TryGetByOwnerMutable(handle);
			auto* textPtr = ecs.Texts.TryGetByOwnerMutable(handle);
			auto* cameraPtr = ecs.Cameras.TryGetByOwnerMutable(handle);

			DrawEntityHandle(handle);

			if (displayNamePtr)
			{
				DrawDisplayName(*displayNamePtr);
			}

			if (transformPtr)
			{
				DrawTransform2D(*transformPtr);
			}

			if (colliderPtr)
			{
				DrawCollider2D(*colliderPtr);
			}

			if (rigidBodyPtr)
			{
				DrawRigidBody2D(*rigidBodyPtr);
			}

			if (scriptPtr && context.SystemContext.ProjectContext.has_value())
			{
				DrawScript(*scriptPtr, context.SystemContext.ProjectContext.value());
			}

			if (spritePtr)
			{
				DrawSprite2D(*spritePtr);
			}

			if (textPtr)
			{
				DrawText2D(*textPtr);
			}

			if (cameraPtr)
			{
				DrawCamera2D(*cameraPtr);
			}
		}

		return true;
	}
}
