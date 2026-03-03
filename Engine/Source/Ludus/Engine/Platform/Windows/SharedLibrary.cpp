#include "pch.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <filesystem>
#include <string>

#include <Ludus/Engine/Platform/SharedLibrary.h>
#include <Ludus/Engine/Debug/Debug.h>

namespace Ludus::Engine::Platform
{
	SharedLibraryHandle LoadSharedLibrary(const std::filesystem::path& path)
	{
		if (!std::filesystem::exists(path))
		{
			LUDUS_LOG_WARN(path.string() + " not found.");
			return { nullptr };
		}

		auto dll = LoadLibraryW(path.wstring().c_str());
		if (!dll)
		{
			const auto code = GetLastError();
			LUDUS_LOG_ERROR("Failed to load library: " + path.string() + " (error code: " + std::to_string(code) + ")");
			return { nullptr };
		}

		return { dll };
	}

	void UnloadSharedLibrary(SharedLibraryHandle handle)
	{
		if (!handle.NativeHandle)
		{
			return;
		}

		FreeLibrary(static_cast<HMODULE>(handle.NativeHandle));
	}

	void* GetSharedLibrarySymbol(SharedLibraryHandle handle, const char* name)
	{
		if (!handle.NativeHandle)
		{
			LUDUS_LOG_WARN("Could not get shared library symbol. Native handle was null.");
			return nullptr;
		}

		if (name == nullptr || !name[0])
		{
			LUDUS_LOG_WARN("Could not get shared library symbol. Name was malformed or missing.");
			return nullptr;
		}

		auto* symbol = GetProcAddress(static_cast<HMODULE>(handle.NativeHandle), name);
		if (!symbol)
		{
			auto error = GetLastError();
			LUDUS_LOG_ERROR(
				std::string("GetProcAddress failed for symbol '") + name + "' (error " + std::to_string(error) + ")"
			);
			return nullptr;
		}

		return reinterpret_cast<void*>(symbol);
	}
}
