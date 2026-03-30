#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>

#include <Ludus/Engine/Platform/SharedLibrary.h>
#include <Ludus/Engine/Scripting/LoadedScriptDefinition.h>
#include <Ludus/Scripting/ABI/Types.h>

namespace Ludus::Engine::Scripting
{
	Ludus::Engine::Platform::SharedLibraryHandle LoadScriptModule(
		const std::filesystem::path& scriptModulePath
	);

	Ludus::Scripting::ABI::RegisterScriptsFn LoadRegisterScriptsFn(
		Ludus::Engine::Platform::SharedLibraryHandle& handle
	);

	std::unordered_map<std::string, LoadedScriptDefinition> PopulateScriptDefinitions(
		Ludus::Scripting::ABI::RegisterScriptsFn registerScriptsFn
	);

	bool UnloadScriptModule(Ludus::Engine::Platform::SharedLibraryHandle& handle);
}
