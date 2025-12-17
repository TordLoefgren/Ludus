#pragma once

#include <format>
#include <vector>

#include <Ludus/Editor/Core/Utilities.h>
#include <Ludus/Editor/Panels/IPanel.h>
#include <Ludus/Engine/Components/Camera2DComponent.h>
#include <Ludus/Engine/Components/Collider2DComponent.h>
#include <Ludus/Engine/Components/RigidBody2DComponent.h>
#include <Ludus/Engine/Components/Sprite2DComponent.h>
#include <Ludus/Engine/Components/Text2DComponent.h>
#include <Ludus/Engine/Components/Transform2DComponent.h>
#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Core/Enums.h>
#include <Ludus/Engine/Core/ISystem.h>
#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Graphics/HorizontalTextAlignment.h>
#include <Ludus/Engine/Graphics/Shape.h>
#include <Ludus/Engine/Physics/Core/LayerMask.h>
#include <Ludus/UI/Context/LayoutContext.h>
#include <Ludus/UI/Context/TableContext.h>
#include <Ludus/UI/Context/WindowContext.h>
#include <Ludus/UI/Scope/TableScope.h>
#include <Ludus/UI/Scope/TreeNodeScope.h>
#include <Ludus/UI/Scope/WindowScope.h>
#include <Ludus/UI/Widgets/Color.h>
#include <Ludus/UI/Widgets/Headers.h>
#include <Ludus/UI/Widgets/Input.h>
#include <Ludus/UI/Widgets/Text.h>
#include <Ludus/UI/Widgets/Toggle.h>

namespace Ludus::Editor::Core
{
	inline void EntityPanel(Ludus::Engine::Core::EntityHandle handle)
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

	inline void Transform2DPanel(Ludus::Engine::Components::Transform2DComponent& transform)
	{
		if (Ludus::UI::Scope::TreeNodeScope treeNode("Transform 2D"); treeNode)
		{
			if (Ludus::UI::Scope::TableScope table("Transform2D_Panel", 3); table)
			{
				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Position");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				Ludus::UI::Widgets::DragFloatLabelButton("X##Transform2D_Panel_Position_X", &transform.Position.X);
				Ludus::UI::Context::LayoutContext::SameLine();
				Ludus::UI::Widgets::InputFloat("##Transform2D_Panel_Input_Position_X", &transform.Position.X);

				Ludus::UI::Context::TableContext::TableSetColumnIndex(2);
				Ludus::UI::Widgets::DragFloatLabelButton("Y##Transform2D_Panel_Position_Y", &transform.Position.Y);
				Ludus::UI::Context::LayoutContext::SameLine();
				Ludus::UI::Widgets::InputFloat("##Transform2D_Panel_Input_Position_Y", &transform.Position.Y);

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Scale");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				Ludus::UI::Widgets::DragFloatLabelButton("X##Transform2D_Panel_Scale_X", &transform.Scale.X);
				Ludus::UI::Context::LayoutContext::SameLine();
				Ludus::UI::Widgets::InputFloat("##Transform2D_Panel_Input_Scale_X", &transform.Scale.X);

				Ludus::UI::Context::TableContext::TableSetColumnIndex(2);
				Ludus::UI::Widgets::DragFloatLabelButton("Y##Transform2D_Panel_Scale_Y", &transform.Scale.Y);
				Ludus::UI::Context::LayoutContext::SameLine();
				Ludus::UI::Widgets::InputFloat("##Transform2D_Panel_Input_Scale_Y", &transform.Scale.Y);

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Rotation");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				Ludus::UI::Widgets::DragFloatLabelButton("Z##Transform2D_Panel_DragFloat_Rotation", &transform.Rotation);
				Ludus::UI::Context::LayoutContext::SameLine();
				Ludus::UI::Widgets::InputFloat("##Transform2D_Panel_InputFloat_Rotation_Z", &transform.Rotation);
			}
		}
	}

	inline void Collider2DPanel(Ludus::Engine::Components::Collider2DComponent& collider)
	{
		if (Ludus::UI::Scope::TreeNodeScope treeNode("Collider 2D"); treeNode)
		{
			if (Ludus::UI::Scope::TableScope table("##Collider2D_Panel", 3); table)
			{
				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Layer Index");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				Ludus::UI::Widgets::InputUInt8("##Collider2D_Panel_LayerIndex", &collider.LayerIndex);
				Ludus::UI::Context::TableContext::TableSetColumnIndex(2);
				Ludus::UI::Widgets::TextUnformatted(Ludus::Engine::Physics::Core::LayerMask::LayerIndexToName(collider.LayerIndex));

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Collides With");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);

				const auto count = Ludus::Engine::Physics::Core::LayerMask::GetLayerCount();
				if (Ludus::UI::Widgets::CollapsingHeader("##Collider2D_Panel_CollidesWith"))
				{
					auto newMask = collider.CollidesWith;
					auto hasChanges = false;

					for (size_t i = 0; i < count; i++)
					{
						auto layer = Ludus::Engine::Physics::Core::LayerMask::FromIndex((uint8_t)i);
						auto label = Ludus::Engine::Physics::Core::LayerMask::LayerIndexToName((uint8_t)i);
						auto isChecked = collider.CollidesWith.Contains(layer);

						auto item = Ludus::UI::Widgets::CheckboxItem(std::format("{} {}", i, label), isChecked);
						if (Ludus::UI::Widgets::Checkbox(item))
						{
							hasChanges = true;
							newMask = item.IsChecked ? newMask |= layer : newMask &= ~layer;
						}
					}

					if (hasChanges)
					{
						collider.CollidesWith = newMask;
					}
				}

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Is Trigger");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				Ludus::UI::Widgets::Checkbox("##IsTrigger", &collider.IsTrigger);
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
			}
		}
	}

	inline void RigidBody2DPanel(Ludus::Engine::Components::RigidBody2DComponent& rigidBody)
	{
		if (Ludus::UI::Scope::TreeNodeScope treeNode("Rigid Body 2D"); treeNode)
		{
			if (Ludus::UI::Scope::TableScope table("RigidBody2D_Panel", 3); table)
			{
				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Velocity");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				Ludus::UI::Widgets::DragFloatLabelButton("X##RigidBody2D_Panel_Velocity_X", &rigidBody.Velocity.X);
				Ludus::UI::Context::LayoutContext::SameLine();
				Ludus::UI::Widgets::InputFloat("##RigidBody2D_Panel_Input_Velocity_X", &rigidBody.Velocity.X);

				Ludus::UI::Context::TableContext::TableSetColumnIndex(2);
				Ludus::UI::Widgets::DragFloatLabelButton("Y##RigidBody2D_Panel_Velocity_Y", &rigidBody.Velocity.Y);
				Ludus::UI::Context::LayoutContext::SameLine();
				Ludus::UI::Widgets::InputFloat("##RigidBody2D_Panel_Input_Velocity_Y", &rigidBody.Velocity.Y);

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Body Type");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);

				auto _ = Ludus::Editor::Core::Utilities::ComboEnum("##RigidBody2D_Combo", rigidBody.Type);

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Gravity Scale");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				Ludus::UI::Widgets::InputFloat("##RigidBody2D_Panel_GravityScale", &rigidBody.GravityScale);

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Mass");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				Ludus::UI::Widgets::InputFloat("##RigidBody2D_Panel_Mass", &rigidBody.Mass);
			}
		}
	}

	inline void Sprite2DPanel(Ludus::Engine::Components::Sprite2DComponent& sprite)
	{
		if (Ludus::UI::Scope::TreeNodeScope treeNode("Sprite 2D"); treeNode)
		{
			if (Ludus::UI::Scope::TableScope table("Sprite2D_Panel", 3); table)
			{
				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Shape");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);

				auto _ = Ludus::Editor::Core::Utilities::ComboEnum("##Sprite2D_Panel_Combo", sprite.Shape);

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Color");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				Ludus::UI::Widgets::ColorEdit4("##Sprite2D_Panel_Color", sprite.Color.GetData());

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Texture");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				Ludus::UI::Widgets::TextUnformatted("N/A");

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Fill");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				Ludus::UI::Widgets::Checkbox("##Fill", &sprite.Fill);
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
			}
		}
	}

	inline void Text2DPanel(Ludus::Engine::Components::Text2DComponent& text)
	{
		if (Ludus::UI::Scope::TreeNodeScope treeNode("Text 2D"); treeNode)
		{
			if (Ludus::UI::Scope::TableScope table("Text2D_Panel", 3); table)
			{
				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Text");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				Ludus::UI::Widgets::InputText("##Text2D_Panel_Text", text.Text);

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Color");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				Ludus::UI::Widgets::ColorEdit4("##Text2D_Panel_Color", text.Color.GetData());

				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Horizontal Alignment");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);

				auto _ = Ludus::Editor::Core::Utilities::ComboEnum("##Text2D_Panel_Combo", text.HorizontalAlignment);
			}
		}
	}

	inline void Camera2DPanel(Ludus::Engine::Components::Camera2DComponent& component)
	{
		if (Ludus::UI::Scope::TreeNodeScope treeNode("Camera 2D"); treeNode)
		{
			if (Ludus::UI::Scope::TableScope table("Camera2D_Panel", 2); table)
			{
				Ludus::UI::Context::TableContext::TableNextRowFirstColumn();
				Ludus::UI::Widgets::TextUnformatted("Orthographic Size");
				Ludus::UI::Context::TableContext::TableSetColumnIndex(1);
				Ludus::UI::Widgets::DragFloat("##Camera2D_OrthographicSize", &component.OrthographicSize);
			}
		}
	}
}
