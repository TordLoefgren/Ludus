#include "pch.h"

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>

#include <Ludus/Editor/Build/MSBuild/MSBuildContext.h>
#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Platform/Process.h>

namespace
{
	struct Constants
	{
		static constexpr std::string_view VSWherePath = R"(C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe)";
		static constexpr std::string_view MSBuildDirectoryName = "MSBuild";
		static constexpr std::string_view CurrentDirectoryName = "Current";
		static constexpr std::string_view BinDirectoryName = "bin";
		static constexpr std::string_view MSBuildExecutableName = "MSBuild.exe";
	};
}

namespace Ludus::Editor::Build::MSBuild
{
	std::optional<std::filesystem::path> MSBuildContext::LocateMSBuild()
	{
		// We use vswhere to locate a Visual Studio installation that includes MSBuild.
		const auto vswherePath = std::filesystem::path(Constants::VSWherePath);
		if (!std::filesystem::exists(vswherePath))
		{
			return std::nullopt;
		}

		const auto args =
			"-latest "
			"-requires Microsoft.Component.MSBuild "
			"-property installationPath";

		const auto expected = Ludus::Engine::Platform::Process::Run(vswherePath, args);
		if (!expected.HasValue())
		{
			LUDUS_LOG_ERROR("Process error: " + std::string(expected.GetError().what()));
			return std::nullopt;
		}

		const auto& result = expected.GetValue();
		if (result.ExitCode != 0)
		{
			LUDUS_LOG_ERROR("vswhere failed with exit code: " + result.ExitCode);
			LUDUS_LOG_ERROR(result.Output);
			return std::nullopt;
		}

		// Clean path string.
		auto output = result.Output;
		output.erase(std::remove(output.begin(), output.end(), '\r'), output.end());
		output.erase(std::remove(output.begin(), output.end(), '\n'), output.end());

		if (output.empty())
		{
			LUDUS_LOG_ERROR("vswhere returned empty output for installationPath.");
			return std::nullopt;
		}

		// Visual Studio 2019 and 2022 has a documented convention for the MSBuild path, described here:
		// https://learn.microsoft.com/en-us/visualstudio/msbuild/whats-new-msbuild-17-0?view=visualstudio
		// If this changes in the future, we need to use "-find" instead, or branch cases.
		const auto mSBuildPath = std::filesystem::path(output) /
			std::string(Constants::MSBuildDirectoryName) /
			std::string(Constants::CurrentDirectoryName) /
			std::string(Constants::BinDirectoryName) /
			std::string(Constants::MSBuildExecutableName);

		if (!std::filesystem::exists(mSBuildPath))
		{
			LUDUS_LOG_ERROR("MSBuild.exe not found at expected path: " + mSBuildPath.string());
			LUDUS_LOG_ERROR("vswhere installationPath output was: " + output);
			return std::nullopt;
		}

		return mSBuildPath;
	}

	void MSBuildContext::Initialize()
	{
		// MSBuildPath can be cached when editor configuration persistence is implemented.
		if (!MSBuildPath || !std::filesystem::exists(MSBuildPath.value()))
		{
			MSBuildPath = LocateMSBuild();
			if (MSBuildPath)
			{
				LUDUS_LOG_INFO("Located MSBuild path.");
			}
			else
			{
				LUDUS_LOG_WARN("Could not locate MSBuild path. Script compilation will not be available.");
			}
		}
	}
}
