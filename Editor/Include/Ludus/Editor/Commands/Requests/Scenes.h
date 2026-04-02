#pragma once

#include <Ludus/Editor/Commands/RequestCommand.h>

namespace Ludus::Editor::Commands
{
	struct ProjectSessionCommandContext;
}

namespace Ludus::Editor::Commands::Requests::Scenes
{
	void CreateScene(ProjectSessionCommandContext& context);
	void CreateSceneAs(const RequestCommand::CreateSceneAs& command, ProjectSessionCommandContext& context);
	void OpenScene(const RequestCommand::OpenScene& command, ProjectSessionCommandContext& context);
	void SaveScene(const RequestCommand::SaveScene& command, ProjectSessionCommandContext& context);
	void SaveSceneAs(const RequestCommand::SaveSceneAs& command, ProjectSessionCommandContext& context);
	void RenameScene(const RequestCommand::RenameScene& command, ProjectSessionCommandContext& context);
}
