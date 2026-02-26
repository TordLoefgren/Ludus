#pragma once

#include <filesystem>

#include <Ludus/Engine/Core/Project.h>
#include <Ludus/Engine/Persistence/IProjectPersistence.h>

namespace Ludus::Engine::Persistence
{
	class LmlProjectPersistence final : public IProjectPersistence
	{
	public:
		LmlProjectPersistence() = default;

		void Save(const Ludus::Engine::Core::Project& project, const std::filesystem::path& path) override;
		Ludus::Engine::Core::Project Load(const std::filesystem::path& path) override;
	};
}
