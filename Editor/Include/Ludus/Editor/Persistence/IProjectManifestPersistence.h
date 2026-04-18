#pragma once

#include <filesystem>

#include <Ludus/Editor/Core/ProjectManifest.h>

namespace Ludus::Editor::Persistence
{
	class IProjectManifestPersistence
	{
	public:
		virtual ~IProjectManifestPersistence() = default;

		virtual void Save(const Ludus::Editor::Core::ProjectManifest& projectManifest, const std::filesystem::path& path) const = 0;
		virtual Ludus::Editor::Core::ProjectManifest Load(const std::filesystem::path& path) const = 0;
	};
}
