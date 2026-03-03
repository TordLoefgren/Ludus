#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <Ludus/Editor/Commands/CommandSet.h>
#include <Ludus/Editor/Dialogs/DialogOutcome.h>
#include <Ludus/Engine/Components/ScriptComponent.h>
#include <Ludus/Engine/Core/Entity.h>
#include <Ludus/Engine/Core/Scene.h>

namespace Ludus::Editor::Dialogs
{
	struct AddScriptDialog
	{
	private:
		enum class AddScriptTab { Create, Select };

		bool IsOpen = true;
		bool JustOpened = true;
		std::string Error;
		std::string CreateName;
		std::string SelectName;
		AddScriptTab ActiveTab = AddScriptTab::Create;

		Ludus::Engine::Core::EntityHandle Entity;
		Ludus::Engine::Core::SceneHandle Scene;
		std::vector<std::string> ScriptNames;
		std::unordered_map<std::string, Ludus::Engine::Components::ScriptHandle> ScriptHandlesByName;

		using Outcome = DialogOutcome<std::string>;

	public:
		AddScriptDialog(
			Ludus::Engine::Core::EntityHandle entity,
			Ludus::Engine::Core::SceneHandle scene,
			std::vector<std::string> scriptNames,
			std::unordered_map<std::string, Ludus::Engine::Components::ScriptHandle> scriptHandlesByName
		) :
			Entity(entity),
			Scene(scene),
			ScriptNames(std::move(scriptNames)),
			ScriptHandlesByName(std::move(scriptHandlesByName))
		{ }

		Outcome Draw();
		void Resolve(const Outcome& outcome, Ludus::Editor::Commands::CommandSet& out);
		bool ShouldClose(const Outcome&) const;
	};
}
