#pragma once

#include <filesystem>
#include <string>
#include <unordered_map>

#include <Ludus/Engine/Platform/SharedLibrary.h>
#include <Ludus/Engine/Scripting/LoadedScriptDefinition.h>
#include <Ludus/Engine/Scripting/ScriptModuleLoader.h>

namespace Ludus::Engine::Scripting
{
	struct LoadedScriptModule
	{
	private:
		std::unordered_map<std::string, LoadedScriptDefinition> m_Scripts;
		Ludus::Engine::Platform::SharedLibraryHandle m_Handle { };

	public:
		const LoadedScriptDefinition* TryFindDefinition(const std::string& name) const
		{
			if (auto iter = m_Scripts.find(name); iter != m_Scripts.end())
			{
				return &iter->second;
			}

			return nullptr;
		}

		bool LoadScriptModule(
			const std::filesystem::path& scriptModulePath
		)
		{
			m_Handle = Ludus::Engine::Scripting::LoadScriptModule(scriptModulePath);
			if (m_Handle.NativeHandle == nullptr)
			{
				m_Scripts.clear();
				return false;
			}

			// Remove potential stale references.
			m_Scripts.clear();

			const auto registerScriptsFn = Ludus::Engine::Scripting::LoadRegisterScriptsFn(m_Handle);
			if (!registerScriptsFn)
			{
				return false;
			}

			m_Scripts = Ludus::Engine::Scripting::PopulateScriptDefinitions(registerScriptsFn);

			return m_Handle.NativeHandle != nullptr;
		}

		bool UnloadScriptModule()
		{
			if (m_Handle.NativeHandle == nullptr)
			{
				m_Handle = { };
				m_Scripts.clear();

				return false;
			}

			Ludus::Engine::Scripting::UnloadScriptModule(m_Handle);
			m_Handle = { };
			m_Scripts.clear();

			return true;
		}
	};
}
