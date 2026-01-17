#pragma once

#include <Ludus/Engine/Components/Camera2DComponent.h>
#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Core/Expected.h>
#include <Ludus/Engine/Persistance/Archives/ArchiveScope.h>
#include <Ludus/Engine/Persistance/Archives/ILoadArchive.h>
#include <Ludus/Engine/Persistance/Archives/ISaveArchive.h>
#include <Ludus/Engine/Persistance/PersistanceException.h>

namespace Ludus::Engine::Persistance::Serializers
{
	using Camera = Ludus::Engine::Components::Camera2DComponent;
	using ISaveArchive = Ludus::Engine::Persistance::Archives::ISaveArchive;
	using ILoadArchive = Ludus::Engine::Persistance::Archives::ILoadArchive;

	template <class TArchive>
	using ObjectScope = Ludus::Engine::Persistance::Archives::ObjectScope<TArchive>;

	struct Camera2DComponentSerializer
	{
		inline static void Save(ISaveArchive& archive, const Camera& camera)
		{
			ObjectScope cameraScope(archive, "Camera2D");

			archive.WriteUint32("OwnerHandle", camera.OwnerHandle);

			archive.WriteFloat("OrthographicSize", camera.OrthographicSize);

			archive.WriteInt("Priority", camera.Priority);
		}

		inline static Ludus::Engine::Core::Expected<Camera, PersistanceException> Load(ILoadArchive& archive)
		{
			try
			{
				Camera camera(0);

				ObjectScope cameraScope(archive, "Camera2D");

				archive.ReadUint32("OwnerHandle", camera.OwnerHandle);

				archive.ReadFloatOrDefault("OrthographicSize", camera.OrthographicSize, 10.0f);

				archive.ReadIntOrDefault("Priority", camera.Priority, -1);

				return camera;
			}
			catch (const PersistanceException& ex)
			{
				return Ludus::Engine::Core::Expected<Camera, PersistanceException>(
					Ludus::Engine::Core::Unexpected<PersistanceException>::Create(ex)
				);
			}
		}
	};
}
