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
		std::wstring buffer(MAX_PATH, L'\0');

		const auto length = GetModuleFileNameW(nullptr, buffer.data(), MAX_PATH);
		if (length == 0)
		{
			throw std::runtime_error("GetModuleFileNameW failed.");
		}

		buffer.resize(length);
		return std::filesystem::path(buffer);
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
