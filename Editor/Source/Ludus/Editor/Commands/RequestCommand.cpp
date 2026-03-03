#include "pch.h"

#include <Ludus/Editor/Commands/CommandContext.h>
#include <Ludus/Editor/Commands/EditCommand.h>
#include <Ludus/Editor/Commands/RequestCommand.h>
#include <Ludus/Editor/Commands/Requests/Panels.h>
#include <Ludus/Editor/Commands/Requests/Projects.h>
#include <Ludus/Editor/Commands/Requests/Scenes.h>
#include <Ludus/Editor/Commands/Requests/Scripts.h>
#include <Ludus/Editor/Core/EditorContext.h>
#include <Ludus/Engine/Core/SystemContext.h>
#include <Ludus/Engine/Core/Variants.h>
#include <Ludus/Engine/Debug/Debug.h>

namespace Ludus::Editor::Commands
{
	namespace
	{
		struct RequestCommandVisitor
		{
			CommandContext& Context;

			void operator()(const RequestCommand::AddViewport& command) const { Requests::Panels::AddViewport(command, Context); }
			void operator()(const RequestCommand::CreateScene& command) const { Requests::Scenes::CreateScene(command, Context); }
			void operator()(const RequestCommand::OpenScene& command) const { Requests::Scenes::OpenScene(command, Context); }
			void operator()(const RequestCommand::SaveScene& command) const { Requests::Scenes::SaveScene(command, Context); }
			void operator()(const RequestCommand::SaveSceneAs& command) const { Requests::Scenes::SaveSceneAs(command, Context); }
			void operator()(const RequestCommand::CreateProject& command) const { Requests::Projects::CreateProject(command, Context); }
			void operator()(const RequestCommand::OpenProject& command) const { Requests::Projects::OpenProject(command, Context); }
			void operator()(const RequestCommand::CloseProject& command) const { Requests::Projects::CloseProject(command, Context); }
			void operator()(const RequestCommand::CreateScript& command) const { Requests::Scripts::CreateScript(command, Context); }
			void operator()(const RequestCommand::SetExecutionMode& command) const { Requests::Panels::SetExecutionMode(command, Context); }

			template<typename T>
			void operator()(T&& unhandled) const
			{
				Ludus::Engine::Core::Variants::Unhandled(unhandled);
			}
		};
	}

	void Execute(const RequestCommand& command, CommandContext& context)
	{
		std::visit(RequestCommandVisitor { context }, command.Data);
	}
}
