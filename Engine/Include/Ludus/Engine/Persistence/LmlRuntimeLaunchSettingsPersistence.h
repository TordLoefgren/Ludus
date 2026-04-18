#pragma once

#include <filesystem>

#include <Ludus/Engine/Persistence/IRuntimeLaunchSettingsPersistence.h>
#include <Ludus/Engine/Runtime/RuntimeLaunchSettings.h>

namespace Ludus::Engine::Persistence
{
	class LmlRuntimeLaunchSettingsPersistence final : public IRuntimeLaunchSettingsPersistence
	{
	public:
		LmlRuntimeLaunchSettingsPersistence() = default;

		void Save(const Ludus::Engine::Runtime::RuntimeLaunchSettings& runtimeLaunchSettings, const std::filesystem::path& path) const override;
		Ludus::Engine::Runtime::RuntimeLaunchSettings Load(const std::filesystem::path& path) const override;
	};
}
