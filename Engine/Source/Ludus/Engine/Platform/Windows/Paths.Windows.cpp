#include "pch.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <shlobj.h>
#include <shellapi.h>

#include <filesystem>
#include <string>
#include <stdexcept>
#include <system_error>

#include "WinShellExecute.h"

#include <Ludus/Engine/Platform/Paths.h>

namespace Ludus::Engine::Platform::Paths
{
	// Inspiration from: https://stackoverflow.com/questions/5920853/how-to-open-a-folder-in-appdata-with-c
	std::filesystem::path AppData(REFKNOWNFOLDERID  folderId)
	{
		PWSTR raw;
		const HRESULT result = SHGetKnownFolderPath(folderId, 0, nullptr, &raw);

		if (FAILED(result) || raw == nullptr)
		{
			throw std::runtime_error("Error getting an app data folder.");
		}

		std::filesystem::path path(raw);
		CoTaskMemFree(raw);

		return path;
	}

	std::filesystem::path LocalAppData()
	{
		return AppData(FOLDERID_LocalAppData);
	}

	std::filesystem::path RoamingAppData()
	{
		return AppData(FOLDERID_RoamingAppData);
	}

	std::filesystem::path GetExecutablePath()
	{
		constexpr int MaxPathResizeAttempts = 6;

		std::wstring buffer(MAX_PATH, L'\0');

		for (int attempt = 0; attempt < MaxPathResizeAttempts; ++attempt)
		{
			const DWORD size = static_cast<DWORD>(buffer.size());
			const DWORD length = ::GetModuleFileNameW(nullptr, buffer.data(), size);

			if (length == 0)
			{
				throw std::runtime_error("GetModuleFileNameW failed.");
			}

			if (length < size - 1)
			{
				buffer.resize(length);

				std::filesystem::path executablePath { buffer };

				std::error_code errorCode;
				if (!std::filesystem::exists(executablePath, errorCode) || errorCode)
				{
					throw std::runtime_error("Executable path does not exist.");
				}

				if (!std::filesystem::is_regular_file(executablePath, errorCode) || errorCode)
				{
					throw std::runtime_error("Executable path is not a regular file.");
				}

				return executablePath;
			}

			buffer.resize(buffer.size() * 2);
		}

		throw std::runtime_error("Executable path exceeds supported buffer resize limit.");
	}

	void OpenFolder(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
		{
			throw std::runtime_error(
				"Failed to open folder. Path '" + path.string() + "' did not exist."
			);
		}

		Ludus::Engine::Platform::Windows::Detail::ShellOpen(path);
	}
}
