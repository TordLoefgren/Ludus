#pragma once

#include <filesystem>

#include <Ludus/Engine/Persistence/IRuntimeManifestPersistence.h>
#include <Ludus/Engine/Runtime/RuntimeManifest.h>

namespace Ludus::Engine::Persistence
{
	class LmlRuntimeManifestPersistence final : public IRuntimeManifestPersistence
	{
	public:
		LmlRuntimeManifestPersistence() = default;

		void Save(const Ludus::Engine::Runtime::RuntimeManifest& runtimeManifest, const std::filesystem::path& path) override;
		Ludus::Engine::Runtime::RuntimeManifest Load(const std::filesystem::path& path) override;
	};
}
