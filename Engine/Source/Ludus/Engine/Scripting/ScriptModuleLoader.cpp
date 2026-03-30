#include "pch.h"

#include <filesystem>
#include <unordered_map>

#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Platform/SharedLibrary.h>
#include <Ludus/Engine/Scripting/LoadedScriptDefinition.h>
#include <Ludus/Engine/Scripting/ScriptModuleLoader.h>
#include <Ludus/Scripting/ABI/Types.h>

namespace
{
	struct LoadContext
	{
		std::unordered_map<std::string, Ludus::Engine::Scripting::LoadedScriptDefinition>* Scripts = nullptr;
	};

	Ludus::Engine::Scripting::LoadedScriptDefinition ToLoadedDefinition(
		const Ludus::Scripting::ABI::ScriptDefinition& definition
	)
	{
		return {
			.Name = definition.Name ? definition.Name : "",
			.OnCreate = definition.OnCreate,
			.OnDestroy = definition.OnDestroy,
			.OnUpdate = definition.OnUpdate,
			.OnCollisionEnter = definition.OnCollisionEnter,
			.OnCollisionStay = definition.OnCollisionStay,
			.OnCollisionExit = definition.OnCollisionExit
		};
	}

	void AddImpl(void* context, const Ludus::Scripting::ABI::ScriptDefinition* definition)
	{
		if (!context || !definition)
		{
			LUDUS_ASSERT(false, "Script registry Add called with null.");
			return;
		}

		if (!definition->Name || definition->Name[0] == '\0')
		{
			LUDUS_LOG_ERROR("Script registry Add called with missing name.");
			return;
		}

		auto* loadContext = static_cast<LoadContext*>(context);
		LUDUS_ASSERT(loadContext->Scripts != nullptr, "Script registry load context must contain a scripts map.");

		loadContext->Scripts->emplace(definition->Name, ToLoadedDefinition(*definition));
	}
}

namespace Ludus::Engine::Scripting
{
	Ludus::Engine::Platform::SharedLibraryHandle LoadScriptModule(const std::filesystem::path& scriptModulePath)
	{
		if (!std::filesystem::exists(scriptModulePath))
		{
			LUDUS_LOG_ERROR("Script module path not found: " + scriptModulePath.string());
			return { };
		}

		const auto handle = Ludus::Engine::Platform::LoadSharedLibrary(scriptModulePath);
		if (!handle.NativeHandle)
		{
			LUDUS_LOG_ERROR("Failed to load script DLL: " + scriptModulePath.string());
			return { };
		}

		return handle;
	}

	Ludus::Scripting::ABI::RegisterScriptsFn LoadRegisterScriptsFn(Ludus::Engine::Platform::SharedLibraryHandle& handle)
	{
		auto* symbol = Ludus::Engine::Platform::GetSharedLibrarySymbol(handle, Ludus::Scripting::ABI::RegisterSymbolName);
		if (!symbol)
		{
			LUDUS_LOG_ERROR("RegisterScripts symbol not found.");

			Ludus::Engine::Platform::UnloadSharedLibrary(handle);
			handle = { };

			return nullptr;
		}

		return reinterpret_cast<Ludus::Scripting::ABI::RegisterScriptsFn>(symbol);
	}

	std::unordered_map<std::string, LoadedScriptDefinition> PopulateScriptDefinitions(
		Ludus::Scripting::ABI::RegisterScriptsFn registerScriptsFn
	)
	{
		std::unordered_map<std::string, LoadedScriptDefinition> scripts;
		if (!registerScriptsFn)
		{
			return scripts;
		}

		LoadContext context { .Scripts = &scripts };
		Ludus::Scripting::ABI::Registry registry
		{
			.Context = &context,
			.AddFn = &AddImpl
		};

		registerScriptsFn(&registry);

		return scripts;
	}

	bool UnloadScriptModule(Ludus::Engine::Platform::SharedLibraryHandle& handle)
	{
		if (handle.NativeHandle == nullptr)
		{
			handle = { };
			return false;
		}

		Ludus::Engine::Platform::UnloadSharedLibrary(handle);
		handle = { };

		return true;
	}
}
