#pragma once

#include <filesystem>

#include <Ludus/Engine/Core/Scene.h>

namespace Ludus::Engine::Persistence
{
	class IScenePersistence
	{
	public:
		virtual ~IScenePersistence() = default;

		virtual void Save(const Ludus::Engine::Core::Scene& scene, const std::filesystem::path& path) const = 0;
		virtual Ludus::Engine::Core::Scene Load(const std::filesystem::path& path) const = 0;
	};
}
