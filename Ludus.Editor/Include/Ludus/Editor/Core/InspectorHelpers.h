#pragma once

#include <format>
#include <vector>

#include <Ludus/UI/Containers.h>
#include <Ludus/UI/Layouts.h>
#include <Ludus/UI/Widgets.h>

#include <Ludus/Core/Enums.h>
#include <Ludus/Core/ISystem.h>
#include <Ludus/Editor/Core/PanelSystem.h>
#include <Ludus/Editor/Core/Utilities.h>
#include <Ludus/Engine/Entity.h>
#include <Ludus/Engine/LayerMask.h>
#include <Ludus/Graphics/Color.h>
#include <Ludus/Graphics/HorizontalTextAlignment.h>
#include <Ludus/Graphics/Shape.h>
#include <Ludus/Graphics/Sprite2D.h>
#include <Ludus/Graphics/Text2D.h>
#include <Ludus/Math/Transform2D.h>
#include <Ludus/Physics/Core/Collider2D.h>
#include <Ludus/Physics/Core/RigidBody2D.h>

namespace Ludus::Editor::Core
{
	inline void EntityPanel(Ludus::Engine::EntityHandle handle)
	{
		if (Ludus::UI::Containers::TreeNode treeNode("Entity"); treeNode)
		{
			if (Ludus::UI::Containers::Table table("Entity_Panel", 2); table)
			{
				Ludus::UI::Containers::TableNextRowFirstColumn();
				Ludus::UI::Widgets::Text("Id");
				Ludus::UI::Containers::TableSetColumnIndex(1);
				Ludus::UI::Widgets::Text(std::to_string(handle));
			}
		}
	}

	inline void Transform2DPanel(Ludus::Math::Transform2D& transform)
	{
		if (Ludus::UI::Containers::TreeNode treeNode("Transform 2D"); treeNode)
		{
			if (Ludus::UI::Containers::Table table("Transform2D_Panel", 3); table)
			{
				Ludus::UI::Containers::TableNextRowFirstColumn();
				Ludus::UI::Widgets::Text("Position");
				Ludus::UI::Containers::TableSetColumnIndex(1);
				Ludus::UI::Widgets::InputFloat("X##Transform2D_Panel_Position_X", &transform.Position.X);
				Ludus::UI::Containers::TableSetColumnIndex(2);
				Ludus::UI::Widgets::InputFloat("Y##Transform2D_Panel_Position_Y", &transform.Position.Y);

				Ludus::UI::Containers::TableNextRowFirstColumn();
				Ludus::UI::Widgets::Text("Scale");
				Ludus::UI::Containers::TableSetColumnIndex(1);
				Ludus::UI::Widgets::InputFloat("X##Transform2D_Panel_Scale_X", &transform.Scale.X);
				Ludus::UI::Containers::TableSetColumnIndex(2);
				Ludus::UI::Widgets::InputFloat("Y##Transform2D_Panel_Scale_Y", &transform.Scale.Y);

				Ludus::UI::Containers::TableNextRowFirstColumn();
				Ludus::UI::Widgets::Text("Rotation");
				Ludus::UI::Containers::TableSetColumnIndex(1);
				Ludus::UI::Widgets::InputFloat("##Transform2D_Panel_Rotation", &transform.Rotation);
			}
		}
	}

	inline void Collider2DPanel(Ludus::Physics::Core::Collider2D& collider)
	{
		if (Ludus::UI::Containers::TreeNode treeNode("Collider 2D"); treeNode)
		{
			if (Ludus::UI::Containers::Table table("##Collider2D_Panel", 3); table)
			{
				Ludus::UI::Containers::TableNextRowFirstColumn();
				Ludus::UI::Widgets::Text("Layer Index");
				Ludus::UI::Containers::TableSetColumnIndex(1);
				Ludus::UI::Widgets::InputUInt8("##Collider2D_Panel_LayerIndex", &collider.LayerIndex);
				Ludus::UI::Containers::TableSetColumnIndex(2);
				Ludus::UI::Widgets::Text(Ludus::Engine::LayerMask::LayerIndexToName(collider.LayerIndex));

				Ludus::UI::Containers::TableNextRowFirstColumn();
				Ludus::UI::Widgets::Text("Collides With");
				Ludus::UI::Containers::TableSetColumnIndex(1);

				const auto count = Ludus::Engine::LayerMask::GetLayerCount();
				if (Ludus::UI::Widgets::CollapsingHeader("##Collider2D_Panel_CollidesWith"))
				{
					auto newMask = collider.CollidesWith;
					auto hasChanges = false;

					for (size_t i = 0; i < count; i++)
					{
						auto layer = Ludus::Engine::LayerMask::FromIndex((uint8_t)i);
						auto label = Ludus::Engine::LayerMask::LayerIndexToName((uint8_t)i);
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

				Ludus::UI::Containers::TableNextRowFirstColumn();
				Ludus::UI::Widgets::Text("Is Trigger");
				Ludus::UI::Containers::TableSetColumnIndex(1);
				Ludus::UI::Widgets::Checkbox("##IsTrigger", &collider.IsTrigger);
				Ludus::UI::Containers::TableSetColumnIndex(1);
			}
		}
	}

	inline void RigidBody2DPanel(Ludus::Physics::Core::RigidBody2D& rigidBody)
	{
		if (Ludus::UI::Containers::TreeNode treeNode("Rigid Body 2D"); treeNode)
		{
			if (Ludus::UI::Containers::Table table("RigidBody2D_Panel", 3); table)
			{
				Ludus::UI::Containers::TableNextRowFirstColumn();
				Ludus::UI::Widgets::Text("Velocity");
				Ludus::UI::Containers::TableSetColumnIndex(1);
				Ludus::UI::Widgets::InputFloat("X##RigidBody2D_Panel_Velocity_X", &rigidBody.Velocity.X);
				Ludus::UI::Containers::TableSetColumnIndex(2);
				Ludus::UI::Widgets::InputFloat("Y##RigidBody2D_Panel_Velocity_Y", &rigidBody.Velocity.Y);

				Ludus::UI::Containers::TableNextRowFirstColumn();
				Ludus::UI::Widgets::Text("Body Type");
				Ludus::UI::Containers::TableSetColumnIndex(1);

				auto _ = Ludus::Editor::Core::Utilities::ComboEnum("##RigidBody2D_Combo", rigidBody.Type);

				Ludus::UI::Containers::TableNextRowFirstColumn();
				Ludus::UI::Widgets::Text("Gravity Scale");
				Ludus::UI::Containers::TableSetColumnIndex(1);
				Ludus::UI::Widgets::InputFloat("##RigidBody2D_Panel_GravityScale", &rigidBody.GravityScale);

				Ludus::UI::Containers::TableNextRowFirstColumn();
				Ludus::UI::Widgets::Text("Mass");
				Ludus::UI::Containers::TableSetColumnIndex(1);
				Ludus::UI::Widgets::InputFloat("##RigidBody2D_Panel_Mass", &rigidBody.Mass);

				Ludus::UI::Containers::TableNextRowFirstColumn();
				Ludus::UI::Widgets::Text("Inverse Mass");
				Ludus::UI::Containers::TableSetColumnIndex(1);
				Ludus::UI::Widgets::InputFloat("##RigidBody2D_Panel_InverseMass", &rigidBody.InverseMass);
			}
		}
	}

	inline void Sprite2DPanel(Ludus::Graphics::Sprite2D& sprite)
	{
		if (Ludus::UI::Containers::TreeNode treeNode("Sprite 2D"); treeNode)
		{
			if (Ludus::UI::Containers::Table table("Sprite2D_Panel", 3); table)
			{
				Ludus::UI::Containers::TableNextRowFirstColumn();
				Ludus::UI::Widgets::Text("Shape");
				Ludus::UI::Containers::TableSetColumnIndex(1);

				auto _ = Ludus::Editor::Core::Utilities::ComboEnum("##Sprite2D_Panel_Combo", sprite.Shape);

				Ludus::UI::Containers::TableNextRowFirstColumn();
				Ludus::UI::Widgets::Text("Color");
				Ludus::UI::Containers::TableSetColumnIndex(1);
				Ludus::UI::Widgets::ColorEdit4("##Sprite2D_Panel_Color", sprite.Color.GetData());

				Ludus::UI::Containers::TableNextRowFirstColumn();
				Ludus::UI::Widgets::Text("Texture");
				Ludus::UI::Containers::TableSetColumnIndex(1);
				Ludus::UI::Widgets::Text("N/A");

				Ludus::UI::Containers::TableNextRowFirstColumn();
				Ludus::UI::Widgets::Text("Fill");
				Ludus::UI::Containers::TableSetColumnIndex(1);
				Ludus::UI::Widgets::Checkbox("##Fill", &sprite.Fill);
				Ludus::UI::Containers::TableSetColumnIndex(1);
			}
		}
	}

	inline void Text2DPanel(Ludus::Graphics::Text2D& text)
	{
		if (Ludus::UI::Containers::TreeNode treeNode("Text 2D"); treeNode)
		{
			if (Ludus::UI::Containers::Table table("Text2D_Panel", 3); table)
			{
				Ludus::UI::Containers::TableNextRowFirstColumn();
				Ludus::UI::Widgets::Text("Text");
				Ludus::UI::Containers::TableSetColumnIndex(1);
				Ludus::UI::Widgets::InputText("##Text2D_Panel_Text", text.Text);

				Ludus::UI::Containers::TableNextRowFirstColumn();
				Ludus::UI::Widgets::Text("Color");
				Ludus::UI::Containers::TableSetColumnIndex(1);
				Ludus::UI::Widgets::ColorEdit4("##Text2D_Panel_Color", text.Color.GetData());

				Ludus::UI::Containers::TableNextRowFirstColumn();
				Ludus::UI::Widgets::Text("Horizontal Alignment");
				Ludus::UI::Containers::TableSetColumnIndex(1);

				auto _ = Ludus::Editor::Core::Utilities::ComboEnum("##Text2D_Panel_Combo", text.HorizontalAlignment);
			}
		}
	}
}
