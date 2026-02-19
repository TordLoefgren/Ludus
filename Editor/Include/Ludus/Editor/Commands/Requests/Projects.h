#pragma once

#include <filesystem>
#include <optional>

#include <Ludus/Editor/Commands/CommandContext.h>
#include <Ludus/Editor/Commands/RequestCommand.h>
#include <Ludus/Editor/Core/EditorSceneMetadata.h>
#include <Ludus/Editor/Panels/ProjectPanel.h>
#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Persistence/ProjectRepository.h>

namespace Ludus::Editor::Commands::Requests::Projects
{

#pragma region Helpers

	static void RefreshProjectPanel(CommandContext& context)
	{
		auto* projectPanel = context.Panels.TryGet<Ludus::Editor::Panels::ProjectPanel>();
		if (!projectPanel)
		{
			return;
		}

		projectPanel->Refresh(context.SystemContext.ProjectContext);
	};

#pragma endregion

	void CreateProject(const RequestCommand::CreateProject& command, CommandContext& context)
	{
		const auto& editorContext = context.EditorContext;
		auto& systemContext = context.SystemContext;

		auto& registry = systemContext.SceneRegistry;
		auto& session = context.EditorContext.Session;

		registry.Clear();
		session.Clear();

		auto projectContext = systemContext.ProjectRepository.CreateProject(command.Name, command.RootPath);
		systemContext.ProjectContext = std::move(projectContext);

		const auto activeHandle = systemContext.ProjectContext->Project.ActiveSceneHandle;
		auto scene = systemContext.ProjectRepository.LoadScene(systemContext.ProjectContext.value(), activeHandle);

		LUDUS_ASSERT(scene.Handle == activeHandle, "Loaded scene handle does not match project ActiveSceneHandle.");
		(void)registry.AddScene(std::move(scene));

		auto path = systemContext.ProjectContext.value().FindScenePath(activeHandle);

		session.EnsureMetadata(activeHandle, Ludus::Editor::Core::EditorSceneMetadata { .Path = path, .IsDirty = false });
		session.SetActiveScene(activeHandle);

		RefreshProjectPanel(context);

		systemContext.Window.SetTitle(command.Name + " - Ludus Editor");
		LUDUS_LOG_INFO("Created new Ludus project: " + path.string());
	};

	void OpenProject(const RequestCommand::OpenProject& command, CommandContext& context)
	{
		auto& systemContext = context.SystemContext;

		auto& registry = systemContext.SceneRegistry;
		auto& session = context.EditorContext.Session;

		registry.Clear();
		session.Clear();

		auto projectContext = systemContext.ProjectRepository.LoadProject(command.Path);
		systemContext.ProjectContext = std::move(projectContext);

		const auto activeHandle = systemContext.ProjectContext->Project.ActiveSceneHandle;
		auto scene = systemContext.ProjectRepository.LoadScene(*systemContext.ProjectContext, activeHandle);

		LUDUS_ASSERT(scene.Handle == activeHandle, "Loaded scene handle does not match project ActiveSceneHandle.");
		(void)registry.AddScene(std::move(scene));

		auto path = systemContext.ProjectContext.value().FindScenePath(activeHandle);

		session.EnsureMetadata(activeHandle, Ludus::Editor::Core::EditorSceneMetadata { .Path = path, .IsDirty = false });
		session.SetActiveScene(activeHandle);

		RefreshProjectPanel(context);

		systemContext.Window.SetTitle(
			systemContext.ProjectContext.value().ProjectPath.stem().string() + " - Ludus Editor"
		);
	};

	void SaveProject(const RequestCommand::SaveProject& command, CommandContext& context)
	{
		auto& systemContext = context.SystemContext;
		if (!systemContext.HasProjectContext())
		{
			LUDUS_LOG_WARN("Cannot save project. No project context exists.");
			return;
		}

		systemContext.ProjectRepository.SaveProject(systemContext.ProjectContext.value());
	};

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

		RefreshProjectPanel(context);

		systemContext.Window.SetTitle("Ludus Editor");
	};
}
