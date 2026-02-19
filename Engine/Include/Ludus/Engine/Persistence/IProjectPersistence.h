#pragma once

#include <filesystem>
#include <Ludus/Engine/Core/Project.h>

namespace Ludus::Engine::Persistence
{
	class IProjectPersistence
	{
	public:
		virtual ~IProjectPersistence() = default;

		virtual void Save(const Ludus::Engine::Core::Project& project, const std::filesystem::path& path) = 0;
		virtual Ludus::Engine::Core::Project Load(const std::filesystem::path& path) = 0;
	};
}
