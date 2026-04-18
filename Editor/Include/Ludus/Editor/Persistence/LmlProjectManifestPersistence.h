#pragma once

#include <filesystem>

#include <Ludus/Editor/Persistence/IProjectManifestPersistence.h>

namespace Ludus::Editor::Persistence
{
	class LmlProjectManifestPersistence final : public IProjectManifestPersistence
	{
	public:
		LmlProjectManifestPersistence() = default;

		void Save(const Ludus::Editor::Core::ProjectManifest& projectManifest, const std::filesystem::path& path) const override;
		Ludus::Editor::Core::ProjectManifest Load(const std::filesystem::path& path) const override;
	};
}
