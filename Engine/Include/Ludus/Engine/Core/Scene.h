#pragma once

#include <cstdint>
#include <optional>

#include <Ludus/Engine/Components/Camera2DComponent.h>
#include <Ludus/Engine/Core/EntityComponentSystem.h>

namespace Ludus::Engine::Core
{
	using SceneHandle = uint32_t;

	struct Scene
	{
	private:
		inline static SceneHandle s_NextHandle = 1;

	public:
		SceneHandle Handle;
		Ludus::Engine::Core::EntityComponentSystem EntityComponentSystem;

		Scene()
			: Handle(s_NextHandle++)
		{ }

		Scene(const Scene&) = delete;
		Scene& operator=(const Scene&) = delete;
		Scene(Scene&&) noexcept = default;
		Scene& operator=(Scene&&) noexcept = default;

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

		struct PrimaryCamera2D
		{
			Ludus::Engine::Components::Camera2DComponent& Camera;
			Ludus::Engine::Components::Transform2DComponent& Transform;
		};

		std::optional<PrimaryCamera2D> TryGetPrimaryCamera2D()
		{
			auto* cam = TryGetPrimaryCameraComponent();
			if (!cam)
			{
				return std::nullopt;
			}

			auto* transformPtr = EntityComponentSystem.Transforms.TryGetByOwnerMutable(cam->OwnerHandle);
			if (!transformPtr) 
			{
				return std::nullopt;
			}

			return PrimaryCamera2D { *cam, *transformPtr };
		}
	};
}
