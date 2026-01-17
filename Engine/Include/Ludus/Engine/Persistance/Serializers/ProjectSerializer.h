#pragma once

#include <Ludus/Engine/Core/Expected.h>
#include <Ludus/Engine/Core/Project.h>
#include <Ludus/Engine/Persistance/Archives/ArchiveAction.h>
#include <Ludus/Engine/Persistance/Archives/ArchiveScope.h>
#include <Ludus/Engine/Persistance/Archives/ArchiveSeverity.h>
#include <Ludus/Engine/Persistance/Archives/ILoadArchive.h>
#include <Ludus/Engine/Persistance/Archives/ISaveArchive.h>
#include <Ludus/Engine/Persistance/PersistanceException.h>
#include <Ludus/Engine/Persistance/Serializers/SceneSerializer.h>

namespace Ludus::Engine::Persistance::Serializers
{
	using Project = Ludus::Engine::Core::Project;
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

	struct ProjectSerializer
	{
		inline static void Save(ISaveArchive& archive, const Project& project)
		{
			ObjectScope projectScope(archive, "Project");

			{
				ObjectScope versionScope(archive, "Version");
				archive.WriteUint32("Major", project.Version.Major);
				archive.WriteUint32("Minor", project.Version.Minor);
				archive.WriteUint32("Patch", project.Version.Patch);
			}

			ArrayScope scenes(archive, "Scenes");

			for (const auto& scene : project.Scenes)
			{
				ArrayElementScope sceneScope(archive);

				SceneSerializer::Save(archive, scene);
			}
		}

		inline static Ludus::Engine::Core::Expected<Project, PersistanceException> Load(ILoadArchive& archive)
		{
			Project project;

			try
			{
				ObjectScope projectScope(archive, "Project");

				{
					ObjectScope versionScope(archive, "Version");
					archive.ReadUint32("Major", project.Version.Major);
					archive.ReadUint32("Minor", project.Version.Minor);
					archive.ReadUint32("Patch", project.Version.Patch);
				}

				CountedArrayScope scenesScope(archive, "Scenes");
				project.Scenes.reserve(scenesScope.Count());

				for (size_t i = 0; i < scenesScope.Count(); i++)
				{
					ArrayElementScope element(archive, i);

					auto sceneResult = SceneSerializer::Load(archive);
					if (!sceneResult.HasValue())
					{
						return Ludus::Engine::Core::Expected<Project, PersistanceException>(
							Ludus::Engine::Core::Unexpected<PersistanceException>::Create(sceneResult.GetError())
						);
					}

					project.Scenes.emplace_back(std::move(sceneResult.GetValue()));
				}

				return project;
			}
			catch (const PersistanceException& ex)
			{
				return Ludus::Engine::Core::Expected<Project, PersistanceException>(
					Ludus::Engine::Core::Unexpected<PersistanceException>::Create(ex)
				);
			}
		}
	};
}
