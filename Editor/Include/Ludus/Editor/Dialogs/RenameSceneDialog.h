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

		bool m_IsOpen = true;
		bool m_JustOpened = true;
		std::string m_Error;

		std::string m_Name;
		std::filesystem::path m_NewPath;

		Ludus::Engine::Core::SceneId m_SceneId;
		std::filesystem::path m_CurrentPath;

		using Outcome = DialogOutcome<RenameOutcome>;

	public:
		RenameSceneDialog(Ludus::Engine::Core::SceneId sceneId, std::filesystem::path currentPath);

		Outcome Draw();
		void Resolve(const Outcome& outcome, Ludus::Editor::Commands::CommandSet& out);
		bool ShouldClose(const Outcome&) const;
	};
}
