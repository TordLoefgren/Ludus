#pragma once

#include <variant>

#include <Ludus/Editor/Commands/CommandContext.h>
#include <Ludus/Editor/Commands/RequestCommand.h>
#include <Ludus/Editor/Commands/Requests/Panels.h>
#include <Ludus/Editor/Commands/Requests/Projects.h>
#include <Ludus/Editor/Commands/Requests/Scenes.h>
#include <Ludus/Engine/Core/Variants.h>

namespace Ludus::Editor::Commands
{
	struct RequestCommandVisitor
	{
		Ludus::Editor::Commands::CommandContext Context;

		void operator()(const RequestCommand::AddViewport& command) { Requests::Panels::AddViewport(command, Context); }
		void operator()(const RequestCommand::CreateScene& command) { Requests::Scenes::CreateScene(command, Context); }
		void operator()(const RequestCommand::OpenScene& command) { Requests::Scenes::OpenScene(command, Context); }
		void operator()(const RequestCommand::SaveScene& command) { Requests::Scenes::SaveScene(command, Context); }
		void operator()(const RequestCommand::SaveSceneAs& command) { Requests::Scenes::SaveSceneAs(command, Context); }
		void operator()(const RequestCommand::CreateProject& command) { Requests::Projects::CreateProject(command, Context); }
		void operator()(const RequestCommand::OpenProject& command) { Requests::Projects::OpenProject(command, Context); }
		void operator()(const RequestCommand::SaveProject& command) { Requests::Projects::SaveProject(command, Context); }
		void operator()(const RequestCommand::CloseProject& command) { Requests::Projects::CloseProject(command, Context); }
		void operator()(const RequestCommand::SetExecutionMode& command) { Requests::Panels::SetExecutionMode(command, Context); }

		template<class T>
		void operator()(T&& unhandled)
		{
			Ludus::Engine::Core::Variants::Unhandled(unhandled);
		}
	};
}
