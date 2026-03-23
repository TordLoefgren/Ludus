#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>

#include <Ludus/Engine/Components/Camera2DComponent.h>
#include <Ludus/Engine/Core/EntityComponentSystem.h>

namespace Ludus::Engine::Core
{
	using SceneHandle = uint64_t;

	struct Scene
	{
	public:
		SceneHandle Handle;
		std::string Name;
		Ludus::Engine::Core::EntityComponentSystem EntityComponentSystem;

		Scene(SceneHandle handle)
			: Handle(handle), Name("Untitled")
		{ }

		Scene(SceneHandle handle, std::string_view name)
			: Handle(handle), Name(name)
		{ }

		Scene(const Scene&) = delete;
		Scene& operator=(const Scene&) = delete;
		Scene(Scene&&) noexcept = default;
		Scene& operator=(Scene&&) noexcept = default;

		static Scene Clone(const Scene& source)
		{
			auto clone = Scene(source.Handle, source.Name);

			for (const auto& entity : source.EntityComponentSystem.View())
			{
				clone.EntityComponentSystem.RestoreEntity(entity.Handle);
			}

			for (const auto& component : source.EntityComponentSystem.Cameras.View())
			{
				clone.EntityComponentSystem.AttachCamera(component);
			}

			for (const auto& component : source.EntityComponentSystem.Colliders.View())
			{
				clone.EntityComponentSystem.AttachCollider(component);
			}

			for (const auto& component : source.EntityComponentSystem.DisplayNames.View())
			{
				clone.EntityComponentSystem.AttachDisplayName(component);
			}

			for (const auto& component : source.EntityComponentSystem.RigidBodies.View())
			{
				clone.EntityComponentSystem.AttachRigidBody(component);
			}

			for (const auto& component : source.EntityComponentSystem.Scripts.View())
			{
				clone.EntityComponentSystem.AttachScript(component);
			}

			for (const auto& component : source.EntityComponentSystem.Sprites.View())
			{
				clone.EntityComponentSystem.AttachSprite(component);
			}

			for (const auto& component : source.EntityComponentSystem.Texts.View())
			{
				clone.EntityComponentSystem.AttachText(component);
			}

			for (const auto& component : source.EntityComponentSystem.Transforms.View())
			{
				clone.EntityComponentSystem.AttachTransform(component);
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

			auto* transformPtr = EntityComponentSystem.Transforms.TryGetByOwnerMutable(cameraPtr->OwnerHandle);
			if (!transformPtr)
			{
				return std::nullopt;
			}

			return PrimaryCamera2D { *cameraPtr, *transformPtr };
		}
	};
}
