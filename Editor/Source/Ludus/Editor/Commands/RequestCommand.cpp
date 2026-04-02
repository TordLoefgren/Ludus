#include "pch.h"

#include <Ludus/Editor/Commands/EditCommand.h>
#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Editor/Commands/RequestCommand.h>
#include <Ludus/Editor/Commands/Requests/Builds.h>
#include <Ludus/Editor/Commands/Requests/Panels.h>
#include <Ludus/Editor/Commands/Requests/Projects.h>
#include <Ludus/Editor/Commands/Requests/Scenes.h>
#include <Ludus/Editor/Commands/Requests/Scripts.h>
#include <Ludus/Editor/Commands/StartupCommandContext.h>
#include <Ludus/Engine/Core/Variants.h>
#include <Ludus/Engine/Debug/Debug.h>

namespace Ludus::Editor::Commands
{
	namespace
	{
		struct ProjectSessionRequestCommandVisitor
		{
			ProjectSessionCommandContext& Context;

			void operator()(const RequestCommand::AddViewport&) const { Requests::Panels::AddViewport(Context); }
			void operator()(const RequestCommand::CreateScene&) const { Requests::Scenes::CreateScene(Context); }
			void operator()(const RequestCommand::CreateSceneAs& command) const { Requests::Scenes::CreateSceneAs(command, Context); }
			void operator()(const RequestCommand::OpenScene& command) const { Requests::Scenes::OpenScene(command, Context); }
			void operator()(const RequestCommand::SaveScene& command) const { Requests::Scenes::SaveScene(command, Context); }
			void operator()(const RequestCommand::SaveSceneAs& command) const { Requests::Scenes::SaveSceneAs(command, Context); }
			void operator()(const RequestCommand::RenameScene& command) const { Requests::Scenes::RenameScene(command, Context); }
			void operator()(const RequestCommand::CreateProject& command) const { Requests::Projects::CreateProject(command, Context); }
			void operator()(const RequestCommand::CreateProjectAs& command) const { Requests::Projects::CreateProjectAs(command, Context); }
			void operator()(const RequestCommand::OpenProject& command) const { Requests::Projects::OpenProject(command, Context); }
			void operator()(const RequestCommand::CloseProject&) const { Requests::Projects::CloseProject(Context); }
			void operator()(const RequestCommand::CreateScript& command) const { Requests::Scripts::CreateScript(command, Context); }
			void operator()(const RequestCommand::RunTargetBuildCommand& command) const { Requests::Builds::RunTargetBuildCommand(command, Context); }
			void operator()(const RequestCommand::BuildRuntime& command) const { Requests::Builds::BuildRuntime(command, Context); }
			void operator()(const RequestCommand::CleanRuntime&) const { Requests::Builds::CleanRuntime(Context); }
			void operator()(const RequestCommand::SetExecutionMode& command) const { Requests::Panels::SetExecutionMode(command, Context); }
			void operator()(const RequestCommand::SetPanelVisibility& command) const { Requests::Panels::SetPanelVisibility(command, Context); }
			void operator()(const RequestCommand::SetTheme& command) const { Requests::Panels::SetTheme(command, Context); }

			template<typename T>
			void operator()(T&& unhandled) const
			{
				Ludus::Engine::Core::Variants::Unhandled(unhandled);
			}
		};

		struct StartupRequestCommandVisitor
		{
			StartupCommandContext& Context;
			void operator()(const RequestCommand::CreateProject& command) const { Requests::Projects::CreateProject(command, Context); }
			void operator()(const RequestCommand::CreateProjectAs& command) const { Requests::Projects::CreateProjectAs(command, Context); }
			void operator()(const RequestCommand::OpenProject& command) const { Requests::Projects::OpenProject(command, Context); }
			void operator()(const RequestCommand::AddViewport&) const { LUDUS_ASSERT(false, "AddViewport is unavailable during startup."); }
			void operator()(const RequestCommand::CreateScene&) const { LUDUS_ASSERT(false, "CreateScene is unavailable during startup."); }
			void operator()(const RequestCommand::CreateSceneAs&) const { LUDUS_ASSERT(false, "CreateSceneAs is unavailable during startup."); }
			void operator()(const RequestCommand::OpenScene&) const { LUDUS_ASSERT(false, "OpenScene is unavailable during startup."); }
			void operator()(const RequestCommand::SaveScene&) const { LUDUS_ASSERT(false, "SaveScene is unavailable during startup."); }
			void operator()(const RequestCommand::SaveSceneAs&) const { LUDUS_ASSERT(false, "SaveSceneAs is unavailable during startup."); }
			void operator()(const RequestCommand::RenameScene&) const { LUDUS_ASSERT(false, "RenameScene is unavailable during startup."); }
			void operator()(const RequestCommand::CloseProject&) const { LUDUS_ASSERT(false, "CloseProject is unavailable during startup."); }
			void operator()(const RequestCommand::CreateScript&) const { LUDUS_ASSERT(false, "CreateScript is unavailable during startup."); }
			void operator()(const RequestCommand::RunTargetBuildCommand&) const { LUDUS_ASSERT(false, "RunTargetBuildCommand is unavailable during startup."); }
			void operator()(const RequestCommand::BuildRuntime&) const { LUDUS_ASSERT(false, "BuildRuntime is unavailable during startup."); }
			void operator()(const RequestCommand::CleanRuntime&) const { LUDUS_ASSERT(false, "CleanRuntime is unavailable during startup."); }
			void operator()(const RequestCommand::SetExecutionMode&) const { LUDUS_ASSERT(false, "SetExecutionMode is unavailable during startup."); }
			void operator()(const RequestCommand::SetPanelVisibility&) const { LUDUS_ASSERT(false, "SetPanelVisibility is unavailable during startup."); }
			void operator()(const RequestCommand::SetTheme&) const { LUDUS_ASSERT(false, "SetTheme is unavailable during startup."); }

			template<typename T>
			void operator()(T&& unhandled) const
			{
				Ludus::Engine::Core::Variants::Unhandled(unhandled);
			}
		};
	}

	void Execute(const RequestCommand& command, ProjectSessionCommandContext& context)
	{
		std::visit(ProjectSessionRequestCommandVisitor { context }, command.Data);
	}

	void Execute(const RequestCommand& command, StartupCommandContext& context)
	{
		std::visit(StartupRequestCommandVisitor { context }, command.Data);
	}
}
