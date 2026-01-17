#pragma once

#include <Ludus/Engine/Components/Transform2DComponent.h>
#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Core/Expected.h>
#include <Ludus/Engine/Persistance/Archives/ArchiveScope.h>
#include <Ludus/Engine/Persistance/Archives/ILoadArchive.h>
#include <Ludus/Engine/Persistance/Archives/ISaveArchive.h>
#include <Ludus/Engine/Persistance/PersistanceException.h>

namespace Ludus::Engine::Persistance::Serializers
{
	using Transform = Ludus::Engine::Components::Transform2DComponent;
	using ISaveArchive = Ludus::Engine::Persistance::Archives::ISaveArchive;
	using ILoadArchive = Ludus::Engine::Persistance::Archives::ILoadArchive;

	template <class TArchive>
	using ObjectScope = Ludus::Engine::Persistance::Archives::ObjectScope<TArchive>;
	template <class TArchive>
	using OptionalObjectScope = Ludus::Engine::Persistance::Archives::OptionalObjectScope<TArchive>;

	struct Transform2DComponentSerializer
	{
		inline static void Save(ISaveArchive& archive, const Transform& transform)
		{
			ObjectScope transformScope(archive, "Transform2D");

			archive.WriteUint32("OwnerHandle", transform.OwnerHandle);

			{
				ObjectScope positionScope(archive, "Position");
				archive.WriteFloat("X", transform.Position.X);
				archive.WriteFloat("Y", transform.Position.Y);
			}

			{
				ObjectScope scaleScope(archive, "Scale");
				archive.WriteFloat("X", transform.Scale.X);
				archive.WriteFloat("Y", transform.Scale.Y);
			}

			archive.WriteFloat("Rotation", transform.Rotation);
		}

		inline static Ludus::Engine::Core::Expected<Transform, PersistanceException> Load(ILoadArchive& archive)
		{
			try
			{
				Transform transform(0);

				ObjectScope transformScope(archive, "Transform2D");

				archive.ReadUint32("OwnerHandle", transform.OwnerHandle);

				{
					OptionalObjectScope positionScope(archive, "Position");
					if (positionScope)
					{
						archive.ReadFloatOrDefault("X", transform.Position.X, 0.0f);
						archive.ReadFloatOrDefault("Y", transform.Position.Y, 0.0f);
					}
				}

				{
					OptionalObjectScope scaleScope(archive, "Scale");
					if (scaleScope)
					{
						archive.ReadFloatOrDefault("X", transform.Scale.X, 1.0f);
						archive.ReadFloatOrDefault("Y", transform.Scale.Y, 1.0f);
					}
				}

				archive.ReadFloatOrDefault("Rotation", transform.Rotation, 0.0f);

				return transform;
			}
			catch (const PersistanceException& ex)
			{
				return Ludus::Engine::Core::Expected<Transform, PersistanceException>(
					Ludus::Engine::Core::Unexpected<PersistanceException>::Create(ex)
				);
			}
		}
	};
}
