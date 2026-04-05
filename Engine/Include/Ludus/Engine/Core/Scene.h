#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>

#include <Ludus/Engine/Components/Camera2DComponent.h>
#include <Ludus/Engine/Core/EntityComponentSystem.h>
#include <Ludus/Engine/Core/Id.h>

namespace Ludus::Engine::Core
{
	struct Scene
	{
	public:
		SceneId Id { SceneId::Invalid() };
		std::string Name;
		Ludus::Engine::Core::EntityComponentSystem EntityComponentSystem;

		Scene() = default;

		Scene(SceneId id)
			: Id(id), Name("Untitled")
		{ }

		Scene(SceneId id, std::string_view name)
			: Id(id), Name(name)
		{ }

		Scene(const Scene&) = delete;
		Scene& operator=(const Scene&) = delete;
		Scene(Scene&&) noexcept = default;
		Scene& operator=(Scene&&) noexcept = default;

		static Scene Clone(const Scene& source)
		{
			auto clone = Scene(source.Id, source.Name);
			auto clonedEntityIds = std::unordered_map<EntityId, EntityId> { };

			for (const auto& entity : source.EntityComponentSystem.View())
			{
				const auto clonedEntityId = clone.EntityComponentSystem.RestoreEntity(entity.Id);
				clonedEntityIds.emplace(entity.Id, clonedEntityId);
			}

			for (const auto& component : source.EntityComponentSystem.Cameras.View())
			{
				auto cloned = component;
				cloned.OwnerId = clonedEntityIds.at(component.OwnerId);
				clone.EntityComponentSystem.AttachCamera(cloned);
			}

			for (const auto& component : source.EntityComponentSystem.Colliders.View())
			{
				auto cloned = component;
				cloned.OwnerId = clonedEntityIds.at(component.OwnerId);
				clone.EntityComponentSystem.AttachCollider(cloned);
			}

			for (const auto& component : source.EntityComponentSystem.DisplayNames.View())
			{
				auto cloned = component;
				cloned.OwnerId = clonedEntityIds.at(component.OwnerId);
				clone.EntityComponentSystem.AttachDisplayName(cloned);
			}

			for (const auto& component : source.EntityComponentSystem.RigidBodies.View())
			{
				auto cloned = component;
				cloned.OwnerId = clonedEntityIds.at(component.OwnerId);
				clone.EntityComponentSystem.AttachRigidBody(cloned);
			}

			for (const auto& component : source.EntityComponentSystem.Scripts.View())
			{
				auto cloned = component;
				cloned.OwnerId = clonedEntityIds.at(component.OwnerId);
				clone.EntityComponentSystem.AttachScript(cloned);
			}

			for (const auto& component : source.EntityComponentSystem.Sprites.View())
			{
				auto cloned = component;
				cloned.OwnerId = clonedEntityIds.at(component.OwnerId);
				clone.EntityComponentSystem.AttachSprite(cloned);
			}

			for (const auto& component : source.EntityComponentSystem.Texts.View())
			{
				auto cloned = component;
				cloned.OwnerId = clonedEntityIds.at(component.OwnerId);
				clone.EntityComponentSystem.AttachText(cloned);
			}

			for (const auto& component : source.EntityComponentSystem.Transforms.View())
			{
				auto cloned = component;
				cloned.OwnerId = clonedEntityIds.at(component.OwnerId);
				clone.EntityComponentSystem.AttachTransform(cloned);
			}

			return clone;
		}

		Ludus::Engine::Components::Camera2DComponent* TryGetPrimaryCameraComponent()
		{
			auto cameras = EntityComponentSystem.Cameras.ViewMutable();
			if (cameras.empty())
			{
				return nullptr;
			}

			auto* primaryCamera = &cameras.front();
			for (auto& camera : cameras)
			{
				if (camera.Priority > primaryCamera->Priority)
				{
					primaryCamera = &camera;
				}
			}

			return primaryCamera;
		}

		const Ludus::Engine::Components::Camera2DComponent* TryGetPrimaryCameraComponent() const
		{
			auto cameras = EntityComponentSystem.Cameras.View();
			if (cameras.empty())
			{
				return nullptr;
			}

			auto* primaryCameraPtr = &cameras.front();
			for (auto& camera : cameras)
			{
				if (camera.Priority > primaryCameraPtr->Priority)
				{
					primaryCameraPtr = &camera;
				}
			}

			return primaryCameraPtr;
		}

		struct PrimaryCamera2D
		{
			Ludus::Engine::Components::Camera2DComponent& Camera;
			Ludus::Engine::Components::Transform2DComponent& Transform;
		};

		std::optional<PrimaryCamera2D> TryGetPrimaryCamera2D()
		{
			auto* cameraPtr = TryGetPrimaryCameraComponent();
			if (!cameraPtr)
			{
				return std::nullopt;
			}

			auto* transformPtr = EntityComponentSystem.Transforms.TryGetByOwnerMutable(cameraPtr->OwnerId);
			if (!transformPtr)
			{
				return std::nullopt;
			}

			return PrimaryCamera2D { *cameraPtr, *transformPtr };
		}
	};
}
