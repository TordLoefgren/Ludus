#pragma once

#include <optional>

#include <Ludus/Engine/Components/Camera2DComponent.h>
#include <Ludus/Engine/Components/Collider2DComponent.h>
#include <Ludus/Engine/Components/DisplayNameComponent.h>
#include <Ludus/Engine/Components/RigidBody2DComponent.h>
#include <Ludus/Engine/Components/ScriptComponent.h>
#include <Ludus/Engine/Components/Sprite2DComponent.h>
#include <Ludus/Engine/Components/Text2DComponent.h>
#include <Ludus/Engine/Components/Transform2DComponent.h>
#include <Ludus/Engine/Core/ComponentRegistry.h>
#include <Ludus/Engine/Core/EntityRegistry.h>
#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Graphics/HorizontalTextAlignment.h>
#include <Ludus/Engine/Graphics/Shape.h>
#include <Ludus/Engine/Graphics/Texture.h>
#include <Ludus/Engine/Math/Vector2D.h>
#include <Ludus/Engine/Physics/Core/BodyType.h>

namespace Ludus::Engine::Core
{
	struct EntityComponentSystem
	{
	private:
		Ludus::Engine::Core::EntityRegistry m_Entities;

	public:
		Ludus::Engine::Core::ComponentRegistry<Ludus::Engine::Components::Camera2DComponent> Cameras;
		Ludus::Engine::Core::ComponentRegistry<Ludus::Engine::Components::Collider2DComponent> Colliders;
		Ludus::Engine::Core::ComponentRegistry<Ludus::Engine::Components::DisplayNameComponent> DisplayNames;
		Ludus::Engine::Core::ComponentRegistry<Ludus::Engine::Components::RigidBody2DComponent> RigidBodies;
		Ludus::Engine::Core::ComponentRegistry<Ludus::Engine::Components::ScriptComponent> Scripts;
		Ludus::Engine::Core::ComponentRegistry<Ludus::Engine::Components::Sprite2DComponent> Sprites;
		Ludus::Engine::Core::ComponentRegistry<Ludus::Engine::Components::Text2DComponent> Texts;
		Ludus::Engine::Core::ComponentRegistry<Ludus::Engine::Components::Transform2DComponent> Transforms;

		EntityComponentSystem() = default;
		EntityComponentSystem(const EntityComponentSystem&) = delete;
		EntityComponentSystem& operator=(const EntityComponentSystem&) = delete;
		EntityComponentSystem(EntityComponentSystem&&) noexcept = default;
		EntityComponentSystem& operator=(EntityComponentSystem&&) noexcept = default;

		EntityId RestoreEntity(EntityId id)
		{
			return m_Entities.RestoreEntity(id);
		}

		EntityId AddEntity()
		{
			return m_Entities.CreateEntity();
		}

		bool DestroyEntity(EntityId id)
		{
			Cameras.RemoveByOwner(id);
			Colliders.RemoveByOwner(id);
			DisplayNames.RemoveByOwner(id);
			RigidBodies.RemoveByOwner(id);
			Scripts.RemoveByOwner(id);
			Sprites.RemoveByOwner(id);
			Texts.RemoveByOwner(id);
			Transforms.RemoveByOwner(id);

			return m_Entities.DestroyEntity(id);
		}

		void AttachCamera(EntityId id, float orthographicSize = 10.0f, int priority = -1)
		{
			Cameras.Add(id, orthographicSize, priority);
		}

		void AttachCamera(Ludus::Engine::Components::Camera2DComponent component) { Cameras.Add(component); }

		void AttachCollider(
			EntityId id,
			Ludus::Engine::Physics::Core::LayerIndex layer = 0,
			Ludus::Engine::Physics::Core::LayerMask collidesWith = Ludus::Engine::Physics::Core::LayerMask::GetEmpty(),
			bool isTrigger = false
		)
		{
			Colliders.Add(id, layer, collidesWith, isTrigger);
		}

		void AttachCollider(Ludus::Engine::Components::Collider2DComponent component) { Colliders.Add(component); }

		void AttachDisplayName(EntityId id, std::string name = "")
		{
			DisplayNames.Add(id, name);
		}

		void AttachDisplayName(Ludus::Engine::Components::DisplayNameComponent component) { DisplayNames.Add(component); }

		void AttachRigidBody(
			EntityId id,
			Ludus::Engine::Math::Vector2D velocity = { 0.0f, 0.0f },
			Ludus::Engine::Physics::Core::BodyType type = Ludus::Engine::Physics::Core::BodyType::Dynamic,
			float gravityScale = 1.0f,
			float mass = 1.0f
		)
		{
			RigidBodies.Add(id, velocity, type, gravityScale, mass);
		}

		void AttachRigidBody(Ludus::Engine::Components::RigidBody2DComponent component) { RigidBodies.Add(component); }

		void AttachScript(EntityId ownerId, Ludus::Engine::Core::ScriptId id = Ludus::Engine::Core::ScriptId::Invalid())
		{
			Scripts.Add(ownerId, id);
		}

		void AttachScript(Ludus::Engine::Components::ScriptComponent component) { Scripts.Add(component); }

		void AttachSprite(
			EntityId id,
			Ludus::Engine::Graphics::Shape shape = Ludus::Engine::Graphics::Shape::Quad,
			Ludus::Engine::Graphics::Color color = Ludus::Engine::Graphics::Colors::White,
			Ludus::Engine::Graphics::Texture* texture = nullptr,
			bool fill = true
		)
		{
			Sprites.Add(id, shape, color, texture, fill);
		}

		void AttachSprite(Ludus::Engine::Components::Sprite2DComponent component) { Sprites.Add(component); }

		void AttachText(
			EntityId id,
			std::string text = "",
			Ludus::Engine::Graphics::Color color = Ludus::Engine::Graphics::Colors::White,
			Ludus::Engine::Graphics::HorizontalTextAlignment horizontalTextAlignment = Ludus::Engine::Graphics::HorizontalTextAlignment::Left
		)
		{
			Texts.Add(id, text, color, horizontalTextAlignment);
		}

		void AttachText(Ludus::Engine::Components::Text2DComponent component) { Texts.Add(component); }

		void AttachTransform(
			EntityId id,
			Ludus::Engine::Math::Vector2D position = { 0.0f, 0.0f },
			Ludus::Engine::Math::Vector2D scale = { 1.0f, 1.0f },
			float rotation = 0.0f
		)
		{
			Transforms.Add(id, position, scale, rotation);
		}

		void AttachTransform(Ludus::Engine::Components::Transform2DComponent component) { Transforms.Add(component); }

		size_t GetEntityCount() const { return m_Entities.GetCount(); }

		std::optional<size_t> IndexOf(EntityId id) const { return m_Entities.IndexOf(id); }

		std::span<const Entity> View() const { return m_Entities.View(); }
	};
}
