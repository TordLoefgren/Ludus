#include "pch.h"

#include <filesystem>
#include <stdexcept>
#include <system_error>

#include <Ludus/Engine/Platform/FileSystem.h>

namespace Ludus::Engine::Platform::FileSystem
{
	void ReplaceFile(
		const std::filesystem::path& temp,
		const std::filesystem::path& destination
	)
	{
		std::error_code errorCode;
		const auto exists = std::filesystem::exists(destination, errorCode);
		if (errorCode)
		{
			throw std::runtime_error("Could not find file '" + destination.string() + "': " + errorCode.message());
		}

		if (exists)
		{
			std::filesystem::remove(destination, errorCode);
			if (errorCode)
			{
				throw std::runtime_error("Removing '" + destination.string() + "' failed: " + errorCode.message());
			}
		}

		std::filesystem::rename(temp, destination, errorCode);
		if (errorCode)
		{
			throw std::runtime_error("Renaming '" + destination.string() + "' failed: " + errorCode.message());
		}
	}
}
