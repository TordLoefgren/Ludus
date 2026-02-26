#pragma once

#include <filesystem>

#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Persistence/IScenePersistence.h>

namespace Ludus::Engine::Persistence
{
	class LmlScenePersistence final : public IScenePersistence
	{
	public:
		LmlScenePersistence() = default;

		void Save(const Ludus::Engine::Core::Scene& scene, const std::filesystem::path& path) override;
		Ludus::Engine::Core::Scene Load(const std::filesystem::path& path) override;
	};
}
