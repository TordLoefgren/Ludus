#pragma once

#include <Ludus/Engine/ComponentRegistry.h>
#include <Ludus/Engine/EntityRegistry.h>
#include <Ludus/Graphics/HorizontalTextAlignment.h>
#include <Ludus/Graphics/Shape.h>
#include <Ludus/Graphics/Sprite2D.h>
#include <Ludus/Graphics/Text2D.h>
#include <Ludus/Graphics/Texture.h>
#include <Ludus/Math/Transform2D.h>
#include <Ludus/Math/Vector2D.h>
#include <Ludus/Physics/Core/BodyType.h>
#include <Ludus/Physics/Core/Collider2D.h>

namespace Ludus::Engine
{
	struct EntityComponentSystem
	{
	private:
		Ludus::Engine::EntityRegistry m_Entities;

	public:
		Ludus::Engine::ComponentRegistry <Ludus::Physics::Core::Collider2D> Colliders;
		Ludus::Engine::ComponentRegistry <Ludus::Graphics::Sprite2D> Sprites;
		Ludus::Engine::ComponentRegistry <Ludus::Graphics::Text2D> Texts;
		Ludus::Engine::ComponentRegistry <Ludus::Math::Transform2D> Transforms;

		EntityHandle AddEntity()
		{
			return m_Entities.CreateEntity();
		}

		bool DestroyEntity(EntityHandle handle)
		{
			Colliders.RemoveByOwner(handle);
			Sprites.RemoveByOwner(handle);
			Texts.RemoveByOwner(handle);
			Transforms.RemoveByOwner(handle);
			return m_Entities.DestroyEntity(handle);
		}

		void AttachCollider(
			EntityHandle handle,
			Ludus::Physics::Core::Index layer = 0,
			LayerMask collidesWith = LayerMask::GetEmpty(),
			Ludus::Physics::Core::BodyType bodyType = Ludus::Physics::Core::BodyType::Dynamic,
			bool isTrigger = false
		)
		{
			Colliders.Add(handle, layer, collidesWith, bodyType, isTrigger);
		}

		void AttachSprite(
			EntityHandle handle,
			Ludus::Graphics::Shape shape = Ludus::Graphics::Shape::Rect,
			Ludus::Graphics::Color color = Ludus::Graphics::Colors::White,
			Ludus::Graphics::Texture* texture = nullptr,
			bool fill = true
		)
		{
			Sprites.Add(handle, shape, color, texture, fill);
		}

		void AttachText(
			EntityHandle handle,
			std::string text,
			Ludus::Graphics::Color color = Ludus::Graphics::Colors::White,
			Ludus::Graphics::HorizontalTextAlignment horizontalAlignment = Ludus::Graphics::HorizontalTextAlignment::Left
		)
		{
			Texts.Add(handle, text, color, horizontalAlignment);
		}

		void AttachTransform(
			EntityHandle handle,
			Ludus::Math::Vector2D position = { 0.0f, 0.0f },
			Ludus::Math::Vector2D scale = { 1.0f, 1.0f },
			float rotation = 0.0f
		)
		{
			Transforms.Add(handle, position, scale, rotation);
		}

		const size_t GetEntityCount() { return m_Entities.GetCount(); }
	};
}
