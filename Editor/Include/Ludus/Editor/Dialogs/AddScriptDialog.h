#pragma once

#include <string>
#include <utility>
#include <vector>

#include <Ludus/Editor/Commands/CommandSet.h>
#include <Ludus/Editor/Dialogs/DialogOutcome.h>
#include <Ludus/Engine/Components/ScriptComponent.h>
#include <Ludus/Engine/Core/Id.h>
#include <Ludus/Engine/Runtime/RuntimeManifest.h>

namespace Ludus::Editor::Dialogs
{
	struct AddScriptDialog
	{
	private:
		enum class AddScriptTab { Create, Select };

		bool m_IsOpen = true;
		bool m_JustOpened = true;
		std::string m_Error;
		std::string m_CreateName;
		std::string m_SelectName;
		AddScriptTab m_ActiveTab = AddScriptTab::Create;

		Ludus::Engine::Core::SceneId m_SceneId;
		Ludus::Engine::Core::EntityId m_EntityId;
		std::vector<std::string> m_ScriptNames;
		std::vector<Ludus::Engine::Runtime::ScriptReference> m_ScriptReferences;

		using Outcome = DialogOutcome<std::string>;

	public:
		AddScriptDialog(
			Ludus::Engine::Core::SceneId sceneId,
			Ludus::Engine::Core::EntityId entityId,
			std::vector<std::string> scriptNames,
			std::vector<Ludus::Engine::Runtime::ScriptReference> scriptReferences
		);

		Outcome Draw();
		void Resolve(const Outcome& outcome, Ludus::Editor::Commands::CommandSet& out);
		bool ShouldClose(const Outcome&) const;
	};
}
