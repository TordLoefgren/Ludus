#pragma once

#include <Ludus/Engine/Components/Collider2DComponent.h>
#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Core/Expected.h>
#include <Ludus/Engine/Persistance/Archives/ArchiveScope.h>
#include <Ludus/Engine/Persistance/Archives/ILoadArchive.h>
#include <Ludus/Engine/Persistance/Archives/ISaveArchive.h>
#include <Ludus/Engine/Persistance/PersistanceException.h>
#include <Ludus/Engine/Physics/Core/LayerMask.h>

namespace Ludus::Engine::Persistance::Serializers
{
	using Collider = Ludus::Engine::Components::Collider2DComponent;
	using ISaveArchive = Ludus::Engine::Persistance::Archives::ISaveArchive;
	using ILoadArchive = Ludus::Engine::Persistance::Archives::ILoadArchive;

	template <class TArchive>
	using ObjectScope = Ludus::Engine::Persistance::Archives::ObjectScope<TArchive>;

	struct Collider2DComponentSerializer
	{
		inline static void Save(ISaveArchive& archive, const Collider& collider)
		{
			ObjectScope colliderScope(archive, "Collider2D");

			archive.WriteUint32("OwnerHandle", collider.OwnerHandle);

			archive.WriteUint8("LayerIndex", collider.LayerIndex);

			archive.WriteUint32("CollidesWith", collider.CollidesWith.Value);

			archive.WriteBool("IsTrigger", collider.IsTrigger);
		}

		inline static Ludus::Engine::Core::Expected<Collider, PersistanceException> Load(ILoadArchive& archive)
		{
			try
			{
				Collider collider(0);

				ObjectScope colliderScope(archive, "Collider2D");

				archive.ReadUint32("OwnerHandle", collider.OwnerHandle);

				archive.ReadUint8OrDefault("LayerIndex", collider.LayerIndex, 0);

				uint32_t collidesWith;
				if (archive.TryReadUint32("CollidesWith", collidesWith))
				{
					collider.CollidesWith = Ludus::Engine::Physics::Core::LayerMask(collidesWith);
				}

				archive.ReadBoolOrDefault("IsTrigger", collider.IsTrigger, false);

				return collider;
			}
			catch (const PersistanceException& ex)
			{
				return Ludus::Engine::Core::Expected<Collider, PersistanceException>(
					Ludus::Engine::Core::Unexpected<PersistanceException>::Create(ex)
				);
			}
		}
	};
}
