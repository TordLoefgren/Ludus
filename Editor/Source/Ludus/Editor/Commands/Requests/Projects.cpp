#include "pch.h"

#include <utility>

#include <Ludus/Editor/Commands/EditCommand.h>
#include <Ludus/Editor/Commands/Requests/Projects.h>
#include <Ludus/Editor/Core/EditorContext.h>
#include <Ludus/Editor/Core/SceneMetadata.h>
#include <Ludus/Editor/Panels/PanelRegistry.h>
#include <Ludus/Editor/Panels/ProjectPanel.h>
#include <Ludus/Editor/Persistence/Paths.h>
#include <Ludus/Engine/Core/SceneRegistry.h>
#include <Ludus/Engine/Core/SystemContext.h>
#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Persistence/ProjectRepository.h>
#include <Ludus/Engine/Windowing/Window.h>

namespace
{
	void RefreshProjectPanel(Ludus::Editor::Commands::CommandContext& context)
	{
		auto* projectPanel = context.Panels.TryGet<Ludus::Editor::Panels::ProjectPanel>();
		if (!projectPanel)
		{
			return;
		}

		projectPanel->Refresh(context.SystemContext.ProjectContext);
	}
}

namespace Ludus::Editor::Commands::Requests::Projects
{
	void CreateProject(const RequestCommand::CreateProject& command, CommandContext& context)
	{
		const auto projectsRoot = command.RootPath.value_or(Ludus::Editor::Persistence::Paths::ProjectsRoot());
		const auto projectRoot = projectsRoot / std::string(command.Name);
		if (std::filesystem::exists(projectRoot))
		{
			LUDUS_LOG_WARN("Project name already exists for path: " + projectRoot.string());
			return;
		}

		auto& systemContext = context.SystemContext;
		auto& registry = systemContext.SceneRegistry;
		auto& session = context.EditorContext.Session;

		registry.Clear();
		session.Clear();
		context.EditorContext.State.Commands.AddEditCommand(Ludus::Editor::Commands::EditCommand::ClearSelection { });

		auto projectContext = systemContext.ProjectRepository.CreateProject(command.Name, command.RootPath);
		systemContext.ProjectContext = std::move(projectContext);

		const auto activeHandle = systemContext.ProjectContext->Project.ActiveSceneHandle;
		auto scene = systemContext.ProjectRepository.LoadScene(systemContext.ProjectContext.value(), activeHandle);

		LUDUS_ASSERT(scene.Handle == activeHandle, "Loaded scene handle does not match project ActiveSceneHandle.");
		(void)registry.AddScene(std::move(scene));

		auto path = systemContext.ProjectContext.value().FindScenePath(activeHandle);
		session.EnsureMetadata(activeHandle, Ludus::Editor::Core::SceneMetadata { .Path = path, .IsDirty = false });
		session.SetActiveScene(activeHandle);

		context.EditorContext.Build.EnsureScriptProject(systemContext.ProjectContext.value());

		RefreshProjectPanel(context);

		systemContext.Window.SetTitle(command.Name + " - Ludus Editor");

		LUDUS_LOG_INFO("Created new Ludus project: " + systemContext.ProjectContext.value().ProjectPath.string());
	}

	void OpenProject(const RequestCommand::OpenProject& command, CommandContext& context)
	{
		auto& systemContext = context.SystemContext;
		auto& registry = systemContext.SceneRegistry;
		auto& session = context.EditorContext.Session;

		registry.Clear();
		session.Clear();
		context.EditorContext.State.Commands.AddEditCommand(Ludus::Editor::Commands::EditCommand::ClearSelection { });

		auto projectContext = systemContext.ProjectRepository.LoadProject(command.Path);
		systemContext.ProjectContext = std::move(projectContext);

		const auto activeHandle = systemContext.ProjectContext->Project.ActiveSceneHandle;
		auto scene = systemContext.ProjectRepository.LoadScene(*systemContext.ProjectContext, activeHandle);

		LUDUS_ASSERT(scene.Handle == activeHandle, "Loaded scene handle does not match project ActiveSceneHandle.");
		(void)registry.AddScene(std::move(scene));

		auto path = systemContext.ProjectContext.value().FindScenePath(activeHandle);
		session.EnsureMetadata(activeHandle, Ludus::Editor::Core::SceneMetadata { .Path = path, .IsDirty = false });
		session.SetActiveScene(activeHandle);

		RefreshProjectPanel(context);

		systemContext.Window.SetTitle(
			systemContext.ProjectContext.value().ProjectPath.stem().string() + " - Ludus Editor"
		);
	}

	void CloseProject(const RequestCommand::CloseProject& command, CommandContext& context)
	{
		auto& systemContext = context.SystemContext;
		if (!systemContext.HasProjectContext())
		{
			LUDUS_LOG_WARN("Cannot close project. No project context exists.");
			return;
		}

		systemContext.ProjectContext.reset();
		systemContext.SceneRegistry.Clear();
		context.EditorContext.Session.Clear();
		context.EditorContext.State.Commands.AddEditCommand(Ludus::Editor::Commands::EditCommand::ClearSelection { });

		RefreshProjectPanel(context);
		systemContext.Window.SetTitle("Ludus Editor");
	}
}
