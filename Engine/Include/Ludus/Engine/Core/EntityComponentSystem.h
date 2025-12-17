#pragma once

#include <Ludus/Engine/Core/ComponentRegistry.h>
#include <Ludus/Engine/Core/EntityRegistry.h>
#include <Ludus/Engine/Graphics/Camera2DComponent.h>
#include <Ludus/Engine/Graphics/HorizontalTextAlignment.h>
#include <Ludus/Engine/Graphics/Shape.h>
#include <Ludus/Engine/Graphics/Sprite2D.h>
#include <Ludus/Engine/Graphics/Text2D.h>
#include <Ludus/Engine/Graphics/Texture.h>
#include <Ludus/Engine/Math/Transform2D.h>
#include <Ludus/Engine/Math/Vector2D.h>
#include <Ludus/Engine/Physics/Core/BodyType.h>
#include <Ludus/Engine/Physics/Core/Collider2D.h>
#include <Ludus/Engine/Physics/Core/RigidBody2D.h>

namespace Ludus::Engine::Core
{
	struct EntityComponentSystem
	{
	private:
		Ludus::Engine::Core::EntityRegistry m_Entities;

	public:
		Ludus::Engine::Core::ComponentRegistry <Ludus::Engine::Graphics::Camera2DComponent> Cameras;
		Ludus::Engine::Core::ComponentRegistry <Ludus::Engine::Physics::Core::Collider2D> Colliders;
		Ludus::Engine::Core::ComponentRegistry <Ludus::Engine::Physics::Core::RigidBody2D> RigidBodies;
		Ludus::Engine::Core::ComponentRegistry <Ludus::Engine::Graphics::Sprite2D> Sprites;
		Ludus::Engine::Core::ComponentRegistry <Ludus::Engine::Graphics::Text2D> Texts;
		Ludus::Engine::Core::ComponentRegistry <Ludus::Engine::Math::Transform2D> Transforms;

		EntityHandle AddEntity()
		{
			return m_Entities.CreateEntity();
		}

		bool DestroyEntity(EntityHandle handle)
		{
			Cameras.RemoveByOwner(handle);
			Colliders.RemoveByOwner(handle);
			RigidBodies.RemoveByOwner(handle);
			Sprites.RemoveByOwner(handle);
			Texts.RemoveByOwner(handle);
			Transforms.RemoveByOwner(handle);
			return m_Entities.DestroyEntity(handle);
		}

		void AttachCamera(
			EntityHandle handle,
			float orthographicSize = 10.0f
		)
		{
			Cameras.Add(handle, orthographicSize);
		}

		void AttachCollider(
			EntityHandle handle,
			Ludus::Engine::Physics::Core::Index layer = 0,
			Ludus::Engine::Physics::Core::LayerMask collidesWith = Ludus::Engine::Physics::Core::LayerMask::GetEmpty(),
			bool isTrigger = false
		)
		{
			Colliders.Add(handle, layer, collidesWith, isTrigger);
		}

		void AttachRigidBody(
			EntityHandle handle,
			Ludus::Engine::Math::Vector2D velocity = { 0.0f, 0.0f },
			Ludus::Engine::Physics::Core::BodyType type = Ludus::Engine::Physics::Core::BodyType::Dynamic,
			float gravityScale = 1.0f,
			float mass = 1.0f
		)
		{
			RigidBodies.Add(handle, velocity, type, gravityScale, mass);
		}

		void AttachSprite(
			EntityHandle handle,
			Ludus::Engine::Graphics::Shape shape = Ludus::Engine::Graphics::Shape::Rect,
			Ludus::Engine::Graphics::Color color = Ludus::Engine::Graphics::Colors::White,
			Ludus::Engine::Graphics::Texture* texture = nullptr,
			bool fill = true
		)
		{
			Sprites.Add(handle, shape, color, texture, fill);
		}

		void AttachText(
			EntityHandle handle,
			std::string text,
			Ludus::Engine::Graphics::Color color = Ludus::Engine::Graphics::Colors::White,
			Ludus::Engine::Graphics::HorizontalTextAlignment horizontalAlignment = Ludus::Engine::Graphics::HorizontalTextAlignment::Left
		)
		{
			Texts.Add(handle, text, color, horizontalAlignment);
		}

		void AttachTransform(
			EntityHandle handle,
			Ludus::Engine::Math::Vector2D position = { 0.0f, 0.0f },
			Ludus::Engine::Math::Vector2D scale = { 1.0f, 1.0f },
			float rotation = 0.0f
		)
		{
			Transforms.Add(handle, position, scale, rotation);
		}

		const size_t GetEntityCount() { return m_Entities.GetCount(); }

		size_t IndexOf(EntityHandle handle) { return m_Entities.IndexOf(handle); }

		std::span<const Entity> View() const { return m_Entities.View(); }
	};
}
