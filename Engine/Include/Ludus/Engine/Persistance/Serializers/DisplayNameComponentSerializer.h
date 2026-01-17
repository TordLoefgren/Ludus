#pragma once

#include <Ludus/Engine/Components/DisplayNameComponent.h>
#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Core/Expected.h>
#include <Ludus/Engine/Persistance/Archives/ArchiveScope.h>
#include <Ludus/Engine/Persistance/Archives/ILoadArchive.h>
#include <Ludus/Engine/Persistance/Archives/ISaveArchive.h>
#include <Ludus/Engine/Persistance/PersistanceException.h>

namespace Ludus::Engine::Persistance::Serializers
{
	using DisplayName = Ludus::Engine::Components::DisplayNameComponent;
	using ISaveArchive = Ludus::Engine::Persistance::Archives::ISaveArchive;
	using ILoadArchive = Ludus::Engine::Persistance::Archives::ILoadArchive;

	template <class TArchive>
	using ObjectScope = Ludus::Engine::Persistance::Archives::ObjectScope<TArchive>;

	struct DisplayNameComponentSerializer
	{
		inline static void Save(ISaveArchive& archive, const DisplayName& displayName)
		{
			ObjectScope displayNameScope(archive, "DisplayName");

			archive.WriteUint32("OwnerHandle", displayName.OwnerHandle);

			archive.WriteString("Value", displayName.Value);
		}

		inline static Ludus::Engine::Core::Expected<DisplayName, PersistanceException> Load(ILoadArchive& archive)
		{
			try
			{
				DisplayName displayName(0);

				ObjectScope displayNameScope(archive, "DisplayName");

				archive.ReadUint32("OwnerHandle", displayName.OwnerHandle);

				archive.ReadStringOrDefault("Value", displayName.Value, "");

				return displayName;
			}
			catch (const PersistanceException& ex)
			{
				return Ludus::Engine::Core::Expected<DisplayName, PersistanceException>(
					Ludus::Engine::Core::Unexpected<PersistanceException>::Create(ex)
				);
			}
		}
	};
}
