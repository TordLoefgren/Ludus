#pragma once

#include <memory>

#include <Ludus/Engine/Persistence/IRuntimeLaunchSettingsPersistence.h>
#include <Ludus/Engine/Persistence/IRuntimeManifestPersistence.h>
#include <Ludus/Engine/Persistence/IScenePersistence.h>
#include <Ludus/Engine/Persistence/LmlRuntimeLaunchSettingsPersistence.h>
#include <Ludus/Engine/Persistence/LmlRuntimeManifestPersistence.h>
#include <Ludus/Engine/Persistence/LmlScenePersistence.h>

namespace Ludus::Engine::Persistence
{
	class EnginePersistence
	{
	private:
		std::unique_ptr<IRuntimeLaunchSettingsPersistence> m_RuntimeLaunchSettings;
		std::unique_ptr<IRuntimeManifestPersistence> m_RuntimeManifest;
		std::unique_ptr<IScenePersistence> m_Scene;

		EnginePersistence(
			std::unique_ptr<IRuntimeLaunchSettingsPersistence> runtimeLaunchSettings,
			std::unique_ptr<IRuntimeManifestPersistence> runtimeManifest,
			std::unique_ptr<IScenePersistence> scene
		) :
			m_RuntimeLaunchSettings(std::move(runtimeLaunchSettings)),
			m_RuntimeManifest(std::move(runtimeManifest)),
			m_Scene(std::move(scene))
		{}

	public:
		static EnginePersistence DefaultText()
		{
			return {
				std::make_unique<LmlRuntimeLaunchSettingsPersistence>(),
				std::make_unique<LmlRuntimeManifestPersistence>(),
				std::make_unique<LmlScenePersistence>()
			};
		}

		const IRuntimeLaunchSettingsPersistence& RuntimeLaunchSettings() const { return *m_RuntimeLaunchSettings; }
		const IRuntimeManifestPersistence& RuntimeManifest() const { return *m_RuntimeManifest; }
		const IScenePersistence& Scene() const { return *m_Scene; }
	};
}
