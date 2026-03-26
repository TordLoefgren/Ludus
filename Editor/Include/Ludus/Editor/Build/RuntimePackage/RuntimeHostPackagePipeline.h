#pragma once

#include <Ludus/Editor/Build/BuildConfiguration.h>
#include <Ludus/Editor/Build/BuildPlatform.h>
#include <Ludus/Editor/Build/IPackagePipeline.h>

namespace Ludus::Editor::Build::RuntimePackage
{
	struct RuntimeHostPackagePipeline final : public IPackagePipeline
	{
	public:
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
