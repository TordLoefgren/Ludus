#pragma once

#include <optional>
#include <string>

#include <Ludus/Engine/Components/Camera2DComponent.h>
#include <Ludus/Engine/Components/Collider2DComponent.h>
#include <Ludus/Engine/Components/DisplayNameComponent.h>
#include <Ludus/Engine/Components/RigidBody2DComponent.h>
#include <Ludus/Engine/Components/Sprite2DComponent.h>
#include <Ludus/Engine/Components/Text2DComponent.h>
#include <Ludus/Engine/Components/Transform2DComponent.h>
#include <Ludus/Engine/Core/ComponentRegistry.h>
#include <Ludus/Engine/Core/EntityRegistry.h>
#include <Ludus/Engine/Graphics/HorizontalTextAlignment.h>
#include <Ludus/Engine/Graphics/Shape.h>
#include <Ludus/Engine/Graphics/Texture.h>
#include <Ludus/Engine/Math/Vector2D.h>
#include <Ludus/Engine/Physics/Core/BodyType.h>

namespace Ludus::Engine::Persistance::Serializers { struct SceneSerializer; }

namespace Ludus::Engine::Core
{
	struct EntityComponentSystem
	{
	private:
		Ludus::Engine::Core::EntityRegistry m_Entities;

		void AddEntityWithHandle(EntityHandle handle) { m_Entities.AddEntity(handle); }

		friend struct Ludus::Engine::Persistance::Serializers::SceneSerializer;

	public:
		Ludus::Engine::Core::ComponentRegistry <Ludus::Engine::Components::Camera2DComponent> Cameras;
		Ludus::Engine::Core::ComponentRegistry <Ludus::Engine::Components::Collider2DComponent> Colliders;
		Ludus::Engine::Core::ComponentRegistry <Ludus::Engine::Components::DisplayNameComponent> DisplayNames;
		Ludus::Engine::Core::ComponentRegistry <Ludus::Engine::Components::RigidBody2DComponent> RigidBodies;
		Ludus::Engine::Core::ComponentRegistry <Ludus::Engine::Components::Sprite2DComponent> Sprites;
		Ludus::Engine::Core::ComponentRegistry <Ludus::Engine::Components::Text2DComponent> Texts;
		Ludus::Engine::Core::ComponentRegistry <Ludus::Engine::Components::Transform2DComponent> Transforms;

		EntityComponentSystem() = default;
		EntityComponentSystem(const EntityComponentSystem&) = delete;
		EntityComponentSystem& operator=(const EntityComponentSystem&) = delete;
		EntityComponentSystem(EntityComponentSystem&&) noexcept = default;
		EntityComponentSystem& operator=(EntityComponentSystem&&) noexcept = default;

		EntityHandle AddEntity()
		{
			return m_Entities.CreateEntity();
		}

		bool DestroyEntity(EntityHandle handle)
		{
			Cameras.RemoveByOwner(handle);
			Colliders.RemoveByOwner(handle);
			DisplayNames.RemoveByOwner(handle);
			RigidBodies.RemoveByOwner(handle);
			Sprites.RemoveByOwner(handle);
			Texts.RemoveByOwner(handle);
			Transforms.RemoveByOwner(handle);

			return m_Entities.DestroyEntity(handle);
		}

		void AttachCamera(EntityHandle handle, float orthographicSize = 10.0f, int priority = -1)
		{
			Cameras.Add(handle, orthographicSize, priority);
		}

		void AttachCamera(Ludus::Engine::Components::Camera2DComponent component) { Cameras.Add(component); }

		void AttachCollider(
			EntityHandle handle,
			Ludus::Engine::Physics::Core::LayerIndex layer = 0,
			Ludus::Engine::Physics::Core::LayerMask collidesWith = Ludus::Engine::Physics::Core::LayerMask::GetEmpty(),
			bool isTrigger = false
		)
		{
			Colliders.Add(handle, layer, collidesWith, isTrigger);
		}

		void AttachCollider(Ludus::Engine::Components::Collider2DComponent component) { Colliders.Add(component); }

		void AttachDisplayName(EntityHandle handle, std::string value = "")
		{
			DisplayNames.Add(handle, value);
		}

		void AttachDisplayName(Ludus::Engine::Components::DisplayNameComponent component) { DisplayNames.Add(component); }

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

		void AttachRigidBody(Ludus::Engine::Components::RigidBody2DComponent component) { RigidBodies.Add(component); }

		void AttachSprite(
			EntityHandle handle,
			Ludus::Engine::Graphics::Shape shape = Ludus::Engine::Graphics::Shape::Quad,
			Ludus::Engine::Graphics::Color color = Ludus::Engine::Graphics::Colors::White,
			Ludus::Engine::Graphics::Texture* texture = nullptr,
			bool fill = true
		)
		{
			Sprites.Add(handle, shape, color, texture, fill);
		}

		void AttachSprite(Ludus::Engine::Components::Sprite2DComponent component) { Sprites.Add(component); }

		void AttachText(
			EntityHandle handle,
			std::string text = "",
			Ludus::Engine::Graphics::Color color = Ludus::Engine::Graphics::Colors::White,
			Ludus::Engine::Graphics::HorizontalTextAlignment horizontalAlignment = Ludus::Engine::Graphics::HorizontalTextAlignment::Left
		)
		{
			Texts.Add(handle, text, color, horizontalAlignment);
		}

		void AttachText(Ludus::Engine::Components::Text2DComponent component) { Texts.Add(component); }

		void AttachTransform(
			EntityHandle handle,
			Ludus::Engine::Math::Vector2D position = { 0.0f, 0.0f },
			Ludus::Engine::Math::Vector2D scale = { 1.0f, 1.0f },
			float rotation = 0.0f
		)
		{
			Transforms.Add(handle, position, scale, rotation);
		}

		void AttachTransform(Ludus::Engine::Components::Transform2DComponent component) { Transforms.Add(component); }

		size_t GetEntityCount() const { return m_Entities.GetCount(); }

		std::optional<size_t> IndexOf(EntityHandle handle) const { return m_Entities.IndexOf(handle); }

		std::span<const Entity> View() const { return m_Entities.View(); }
	};
}
