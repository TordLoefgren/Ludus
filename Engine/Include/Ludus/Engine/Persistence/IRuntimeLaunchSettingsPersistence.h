#pragma once

#include <filesystem>

#include <Ludus/Engine/Runtime/RuntimeLaunchSettings.h>

namespace Ludus::Engine::Persistence
{
	class IRuntimeLaunchSettingsPersistence
	{
	public:
		virtual ~IRuntimeLaunchSettingsPersistence() = default;

		virtual void Save(const Ludus::Engine::Runtime::RuntimeLaunchSettings& runtimeLaunchSettings, const std::filesystem::path& path) = 0;
		virtual Ludus::Engine::Runtime::RuntimeLaunchSettings Load(const std::filesystem::path& path) = 0;
	};
}
