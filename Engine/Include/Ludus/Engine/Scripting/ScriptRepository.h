#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>

#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Platform/SharedLibrary.h>
#include <Ludus/Engine/Scripting/API/Registry.h>
#include <Ludus/Engine/Scripting/API/Types.h>

namespace Ludus::Engine::Scripting
{
	struct ScriptRepository
	{
	private:
		using RegisterScriptsFn = Ludus::Engine::Scripting::API::RegisterScriptsFn;
		using Registry = Ludus::Engine::Scripting::API::Registry;
		using ScriptDefinition = Ludus::Engine::Scripting::API::ScriptDefinition;

		std::unordered_map<std::string, const ScriptDefinition*> m_Scripts;
		Ludus::Engine::Platform::SharedLibraryHandle m_Handle { };

		static void AddImpl(void* context, const ScriptDefinition* definition)
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

			auto* repository = static_cast<ScriptRepository*>(context);
			repository->m_Scripts.emplace(definition->Name, definition);
		}

	public:
		const ScriptDefinition* TryFindDefinition(const std::string& name) const
		{
			if (auto iter = m_Scripts.find(name); iter != m_Scripts.end())
			{
				return iter->second;
			}

			return nullptr;
		}

		bool LoadScriptModule(
			const std::filesystem::path& scriptModulePath
		)
		{
			if (!std::filesystem::exists(scriptModulePath))
			{
				LUDUS_LOG_ERROR("Script module path not found: " + scriptModulePath.string());
				return false;
			}

			m_Handle = Ludus::Engine::Platform::LoadSharedLibrary(scriptModulePath);
			if (!m_Handle.NativeHandle)
			{
				LUDUS_LOG_ERROR("Failed to load script DLL: " + scriptModulePath.string());
				return false;
			}

			auto* symbol = Ludus::Engine::Platform::GetSharedLibrarySymbol(m_Handle, API::RegisterSymbolName);
			if (!symbol)
			{
				LUDUS_LOG_ERROR("RegisterScripts symbol not found in: " + scriptModulePath.string());

				Ludus::Engine::Platform::UnloadSharedLibrary(m_Handle);
				m_Handle = { };

				return false;
			}

			const auto registerScriptsFn = reinterpret_cast<RegisterScriptsFn>(symbol);
			if (!registerScriptsFn)
			{
				LUDUS_LOG_ERROR("RegisterScripts symbol not found in: " + scriptModulePath.string());

				Ludus::Engine::Platform::UnloadSharedLibrary(m_Handle);
				m_Handle = { };

				return false;
			}

			Registry registry { };
			registry.Bind(this, &AddImpl);

			// Remove potential stale references.
			m_Scripts.clear();

			registerScriptsFn(&registry);

			return true;
		}

		bool UnloadScriptModule()
		{
			if (m_Handle.NativeHandle == nullptr)
			{
				m_Handle = { };
				m_Scripts.clear();

				return false;
			}

			Ludus::Engine::Platform::UnloadSharedLibrary(m_Handle);
			m_Handle = { };
			m_Scripts.clear();

			return true;
		}
	};
}
