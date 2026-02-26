#pragma once

#include <Ludus/Editor/Commands/CommandContext.h>
#include <Ludus/Editor/Commands/RequestCommand.h>

namespace Ludus::Editor::Commands::Requests::Scenes
{
	void CreateScene(const RequestCommand::CreateScene& command, CommandContext& context);
	void OpenScene(const RequestCommand::OpenScene& command, CommandContext& context);
	void SaveScene(const RequestCommand::SaveScene& command, CommandContext& context);
	void SaveSceneAs(const RequestCommand::SaveSceneAs& command, CommandContext& context);
}
