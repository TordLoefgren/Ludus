#pragma once

#include <filesystem>
#include <string>

#include <Ludus/Editor/Commands/CommandSet.h>
#include <Ludus/Editor/Dialogs/DialogOutcome.h>
#include <Ludus/Engine/Core/Id.h>

namespace Ludus::Editor::Dialogs
{
	struct RenameSceneDialog
	{
	private:
		struct RenameOutcome
		{
			Ludus::Engine::Core::SceneId SceneId;
			std::filesystem::path Path;
		};

		bool IsOpen = true;
		bool JustOpened = true;
		std::string Error;

		std::string Name;
		std::filesystem::path NewPath;

		Ludus::Engine::Core::SceneId SceneId;
		std::filesystem::path CurrentPath;

		using Outcome = DialogOutcome<RenameOutcome>;

	public:
		RenameSceneDialog(Ludus::Engine::Core::SceneId sceneId, std::filesystem::path currentPath);

		Outcome Draw();
		void Resolve(const Outcome& outcome, Ludus::Editor::Commands::CommandSet& out);
		bool ShouldClose(const Outcome&) const;
	};
}
