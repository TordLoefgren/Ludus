#pragma once

#include <filesystem>
#include <string>
#include <string_view>

#include <Ludus/Engine/Core/Expected.h>
#include <Ludus/Engine/Platform/PlatformException.h>

namespace Ludus::Engine::Platform::Process
{
	using ExitCode = std::uint32_t;

	struct ProcessResult
	{
		ExitCode ExitCode;
		std::string Output;
	};

	Ludus::Engine::Core::Expected<ProcessResult, PlatformException> Run(
		const std::filesystem::path& application,
		std::string_view args
	);
}
