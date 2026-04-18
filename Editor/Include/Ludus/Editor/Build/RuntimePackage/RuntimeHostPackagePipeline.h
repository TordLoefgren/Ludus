#pragma once

#include <Ludus/Editor/Build/BuildConfiguration.h>
#include <Ludus/Editor/Build/BuildPlatform.h>
#include <Ludus/Editor/Build/IPackagePipeline.h>

namespace Ludus::Engine::Persistence
{
	class IRuntimeLaunchSettingsPersistence;
	class IRuntimeManifestPersistence;
}

namespace Ludus::Editor::Build::RuntimePackage
{
	struct RuntimeHostPackagePipeline final : public IPackagePipeline
	{
	private:
		const Ludus::Engine::Persistence::IRuntimeManifestPersistence& m_RuntimeManifestPersistence;
		const Ludus::Engine::Persistence::IRuntimeLaunchSettingsPersistence& m_RuntimeLaunchSettingsPersistence;

	public:
		RuntimeHostPackagePipeline(
			const Ludus::Engine::Persistence::IRuntimeManifestPersistence& runtimeManifestPersistence,
			const Ludus::Engine::Persistence::IRuntimeLaunchSettingsPersistence& runtimeLaunchSettingsPersistence
		);

		virtual void Initialize() override;

		virtual void BuildPackage(
			const std::filesystem::path& projectRoot,
			BuildConfiguration configuration,
			BuildPlatform platform
		) override;

		virtual void CleanPackage(
			const std::filesystem::path& projectRoot,
			BuildConfiguration configuration,
			BuildPlatform platform
		) override;
	};
}
