#pragma once

#include <filesystem>

namespace Ludus::Engine::Platform
{
	struct SharedLibraryHandle
	{
		void* NativeHandle = nullptr;
	};

	SharedLibraryHandle LoadSharedLibrary(const std::filesystem::path& path);
	void UnloadSharedLibrary(SharedLibraryHandle handle);
	void* GetSharedLibrarySymbol(SharedLibraryHandle handle, const char* name);
}
