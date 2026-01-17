#pragma once

#include <optional>
#include <string_view>

#include <Ludus/Engine/Components/Camera2DComponent.h>
#include <Ludus/Engine/Components/Collider2DComponent.h>
#include <Ludus/Engine/Components/DisplayNameComponent.h>
#include <Ludus/Engine/Components/RigidBody2DComponent.h>
#include <Ludus/Engine/Components/Sprite2DComponent.h>
#include <Ludus/Engine/Components/Text2DComponent.h>
#include <Ludus/Engine/Components/Transform2DComponent.h>
#include <Ludus/Engine/Core/Expected.h>
#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Persistance/Archives/ArchiveAction.h>
#include <Ludus/Engine/Persistance/Archives/ArchiveScope.h>
#include <Ludus/Engine/Persistance/Archives/ArchiveSeverity.h>
#include <Ludus/Engine/Persistance/Archives/ILoadArchive.h>
#include <Ludus/Engine/Persistance/Archives/ISaveArchive.h>
#include <Ludus/Engine/Persistance/PersistanceException.h>
#include <Ludus/Engine/Persistance/Serializers/Camera2DComponentSerializer.h>
#include <Ludus/Engine/Persistance/Serializers/Collider2DComponentSerializer.h>
#include <Ludus/Engine/Persistance/Serializers/DisplayNameComponentSerializer.h>
#include <Ludus/Engine/Persistance/Serializers/RigidBody2DComponentSerializer.h>
#include <Ludus/Engine/Persistance/Serializers/Sprite2DComponentSerializer.h>
#include <Ludus/Engine/Persistance/Serializers/Text2DComponentSerializer.h>
#include <Ludus/Engine/Persistance/Serializers/Transform2DComponentSerializer.h>

namespace Ludus::Engine::Persistance::Serializers
{
	using Scene = Ludus::Engine::Core::Scene;
	using ISaveArchive = Ludus::Engine::Persistance::Archives::ISaveArchive;
	using ILoadArchive = Ludus::Engine::Persistance::Archives::ILoadArchive;

	template <class TArchive>
	using ObjectScope = Ludus::Engine::Persistance::Archives::ObjectScope<TArchive>;
	template <class TArchive>
	using ArrayScope = Ludus::Engine::Persistance::Archives::ArrayScope<TArchive>;
	template <class TArchive>
	using CountedArrayScope = Ludus::Engine::Persistance::Archives::CountedArrayScope<TArchive>;
	template <class TArchive>
	using OptionalObjectScope = Ludus::Engine::Persistance::Archives::OptionalObjectScope<TArchive>;
	template <class TArchive>
	using ArrayElementScope = Ludus::Engine::Persistance::Archives::ArrayElementScope<TArchive>;

	struct SceneSerializer
	{
	private:
		static constexpr std::string_view DisplayNameString = "DisplayName";
		static constexpr std::string_view Camera2DString = "Camera2D";
		static constexpr std::string_view Collider2DString = "Collider2D";
		static constexpr std::string_view RigidBody2DString = "RigidBody2D";
		static constexpr std::string_view Sprite2DString = "Sprite2D";
		static constexpr std::string_view Text2DString = "Text2D";
		static constexpr std::string_view Transform2DString = "Transform2D";

		static constexpr auto HandleMistmatchMessage = "Mismatch between entity and component handle.";

		struct StagedComponents
		{
			std::optional<Ludus::Engine::Components::Camera2DComponent> Camera;
			std::optional<Ludus::Engine::Components::Collider2DComponent> Collider;
			std::optional<Ludus::Engine::Components::DisplayNameComponent> DisplayName;
			std::optional<Ludus::Engine::Components::RigidBody2DComponent> RigidBody;
			std::optional<Ludus::Engine::Components::Sprite2DComponent> Sprite;
			std::optional<Ludus::Engine::Components::Text2DComponent> Text;
			std::optional<Ludus::Engine::Components::Transform2DComponent> Transform;
		};

		static inline void FailComponent(ILoadArchive& archive, std::string_view name, std::string_view errorMessage)
		{
			if (auto* diagnostics = archive.GetDiagnostics())
			{
				diagnostics->Report(
					{
						Ludus::Engine::Persistance::Archives::ArchiveSeverity::Error,
						Ludus::Engine::Persistance::Archives::ArchiveAction::EntitySkipped,
						archive.GetCurrentPath(name),
						"Malformed entity skipped. " + std::string(name) + " was malformed: " + std::string(errorMessage)
					}
				);
			}
		};

		template<class SlotT, class LoaderT>
		static inline bool TryLoadComponent(ILoadArchive& archive, uint32_t entityHandle, std::string_view key, SlotT& componentSlot, LoaderT loader)
		{
			if (!archive.Has(key))
			{
				// Missing key is allowed.
				return true;
			}

			const auto& result = loader(archive);
			if (!result.HasValue())
			{
				FailComponent(archive, key, result.GetError().what());
				return false;
			}

			auto component = result.GetValue();
			if (component.OwnerHandle != entityHandle)
			{
				FailComponent(archive, key, HandleMistmatchMessage);
				return false;
			}

			componentSlot = std::move(component);
			return true;
		};

	public:
		inline static void Save(ISaveArchive& archive, const Scene& scene)
		{
			ObjectScope sceneScope(archive, "Scene");

			archive.WriteUint32("Handle", scene.Handle);

			ArrayScope entitiesScope(archive, "Entities");

			auto& ecs = scene.EntityComponentSystem;
			for (const auto& entity : ecs.View())
			{
				ArrayElementScope element(archive);

				archive.WriteUint32("Handle", entity.Handle);

				if (const auto* camera = ecs.Cameras.TryGetByOwner(entity.Handle))
				{
					Camera2DComponentSerializer::Save(archive, *camera);
				}

				if (const auto* collider = ecs.Colliders.TryGetByOwner(entity.Handle))
				{
					Collider2DComponentSerializer::Save(archive, *collider);
				}

				if (const auto* displayName = ecs.DisplayNames.TryGetByOwner(entity.Handle))
				{
					DisplayNameComponentSerializer::Save(archive, *displayName);
				}

				if (const auto* rigidBody = ecs.RigidBodies.TryGetByOwner(entity.Handle))
				{
					RigidBody2DComponentSerializer::Save(archive, *rigidBody);
				}

				if (const auto* sprite = ecs.Sprites.TryGetByOwner(entity.Handle))
				{
					Sprite2DComponentSerializer::Save(archive, *sprite);
				}

				if (const auto* text = ecs.Texts.TryGetByOwner(entity.Handle))
				{
					Text2DComponentSerializer::Save(archive, *text);
				}

				if (const auto* transform = ecs.Transforms.TryGetByOwner(entity.Handle))
				{
					Transform2DComponentSerializer::Save(archive, *transform);
				}
			}
		}

		inline static Ludus::Engine::Core::Expected<Scene, PersistanceException> Load(ILoadArchive& archive)
		{
			Scene scene;

			try
			{
				ObjectScope sceneScope(archive, "Scene");

				archive.ReadUint32("Handle", scene.Handle);

				CountedArrayScope entitiesScope(archive, "Entities");

				for (size_t i = 0; i < entitiesScope.Count(); i++)
				{
					ArrayElementScope element(archive, i);

					uint32_t entityHandle;
					if (!archive.TryReadUint32("Handle", entityHandle))
					{
						// Skip malformed entity entirely.
						if (auto* diagnostics = archive.GetDiagnostics())
						{
							diagnostics->Report(
								{
									Ludus::Engine::Persistance::Archives::ArchiveSeverity::Error,
									Ludus::Engine::Persistance::Archives::ArchiveAction::EntitySkipped,
									archive.GetCurrentPath("Handle"),
									"Malformed entity skipped. Handle missing."
								}
							);
						}

						continue;
					}

					StagedComponents stagedComponents;

					if (!TryLoadComponent(archive, entityHandle, Camera2DString, stagedComponents.Camera, Camera2DComponentSerializer::Load))
					{
						continue;
					}

					if (!TryLoadComponent(archive, entityHandle, Collider2DString, stagedComponents.Collider, Collider2DComponentSerializer::Load))
					{
						continue;
					}

					if (!TryLoadComponent(archive, entityHandle, DisplayNameString, stagedComponents.DisplayName, DisplayNameComponentSerializer::Load))
					{
						continue;
					}

					if (!TryLoadComponent(archive, entityHandle, RigidBody2DString, stagedComponents.RigidBody, RigidBody2DComponentSerializer::Load))
					{
						continue;
					}

					if (!TryLoadComponent(archive, entityHandle, Sprite2DString, stagedComponents.Sprite, Sprite2DComponentSerializer::Load))
					{
						continue;
					}

					if (!TryLoadComponent(archive, entityHandle, Text2DString, stagedComponents.Text, Text2DComponentSerializer::Load))
					{
						continue;
					}

					if (!TryLoadComponent(archive, entityHandle, Transform2DString, stagedComponents.Transform, Transform2DComponentSerializer::Load))
					{
						continue;
					}

					// SceneSerializer is a friend of the ECS.
					// Only the serializer should be able to add a specific handle to the ECS.
					// When UUIDs are introduced we might want to revisit this setup.
					scene.EntityComponentSystem.AddEntityWithHandle(entityHandle);

					if (stagedComponents.Camera.has_value())
					{
						scene.EntityComponentSystem.AttachCamera(stagedComponents.Camera.value());
					}

					if (stagedComponents.Collider.has_value())
					{
						scene.EntityComponentSystem.AttachCollider(stagedComponents.Collider.value());
					}

					if (stagedComponents.DisplayName.has_value())
					{
						scene.EntityComponentSystem.AttachDisplayName(stagedComponents.DisplayName.value());
					}

					if (stagedComponents.RigidBody.has_value())
					{
						scene.EntityComponentSystem.AttachRigidBody(stagedComponents.RigidBody.value());
					}

					if (stagedComponents.Sprite.has_value())
					{
						scene.EntityComponentSystem.AttachSprite(stagedComponents.Sprite.value());
					}

					if (stagedComponents.Text.has_value())
					{
						scene.EntityComponentSystem.AttachText(stagedComponents.Text.value());
					}

					if (stagedComponents.Transform.has_value())
					{
						scene.EntityComponentSystem.AttachTransform(stagedComponents.Transform.value());
					}
				}
			}
			catch (const PersistanceException& ex)
			{
				return Ludus::Engine::Core::Expected<Scene, PersistanceException>(
					Ludus::Engine::Core::Unexpected<PersistanceException>::Create(ex)
				);
			}

			return scene;
		}
	};
}
