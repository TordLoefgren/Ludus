#pragma once

#include <filesystem>

#include <Ludus/Engine/Runtime/RuntimeManifest.h>

namespace Ludus::Engine::Persistence
{
	class IRuntimeManifestPersistence
	{
	public:
		virtual ~IRuntimeManifestPersistence() = default;

		virtual void Save(const Ludus::Engine::Runtime::RuntimeManifest& runtimeManifest, const std::filesystem::path& path) = 0;
		virtual Ludus::Engine::Runtime::RuntimeManifest Load(const std::filesystem::path& path) = 0;
	};
}
