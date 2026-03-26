#pragma once

#include <filesystem>

#include <Ludus/Editor/Build/BuildConfiguration.h>
#include <Ludus/Editor/Build/BuildPlatform.h>

namespace Ludus::Editor::Build
{
	struct IPackagePipeline
	{
		virtual ~IPackagePipeline() = default;

		virtual void Initialize() = 0;

		virtual void BuildPackage(
			const std::filesystem::path& projectRoot,
			BuildConfiguration configuration,
			BuildPlatform platform
		) = 0;

		virtual void CleanPackage(
			const std::filesystem::path& projectRoot,
			BuildConfiguration configuration,
			BuildPlatform platform
		) = 0;
	};
}
