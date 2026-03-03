#include "pch.h"

#include <algorithm>
#include <charconv>
#include <format>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

#include <Ludus/Editor/Commands/EditCommand.h>
#include <Ludus/Editor/Commands/Requests/Scenes.h>
#include <Ludus/Editor/Core/EditorContext.h>
#include <Ludus/Editor/Core/SceneMetadata.h>
#include <Ludus/Editor/Panels/PanelRegistry.h>
#include <Ludus/Editor/Panels/ProjectPanel.h>
#include <Ludus/Engine/Core/SceneRegistry.h>
#include <Ludus/Engine/Core/SystemContext.h>
#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Persistence/ProjectRepository.h>

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

	std::string GenerateUniqueSceneName(Ludus::Editor::Commands::CommandContext& context)
	{
		std::string baseName = "Untitled";
		auto count = 0;

		for (const auto& scene : context.SystemContext.SceneRegistry.View())
		{
			const auto& name = scene.Name;

			if (!name.starts_with(baseName))
			{
				continue;
			}

			if (name.size() == baseName.size())
			{
				count = std::max(count, 1);
				continue;
			}

			if (name.size() > baseName.size() + 3 &&
				name[baseName.size()] == ' ' &&
				name[baseName.size() + 1] == '(' &&
				name.back() == ')')
			{
				std::string_view numberPart(
					name.data() + baseName.size() + 2,
					name.size() - baseName.size() - 3
				);

				int value = 0;
				auto [_, errorCode] = std::from_chars(
					numberPart.data(),
					numberPart.data() + numberPart.size(),
					value
				);

				if (errorCode == std::errc())
				{
					count = std::max(count, value);
				}
			}
		}

		return count == 0 ? baseName : std::format("{} ({})", baseName, count + 1);
	}

	bool EnsureProjectScriptReferences(
		const Ludus::Engine::Core::Scene& scene,
		Ludus::Engine::Core::ProjectContext& projectContext
	)
	{
		bool hasChanges = false;
		const auto scripts = scene.EntityComponentSystem.Scripts.View();
		for (const auto& script : scripts)
		{
			hasChanges = projectContext.AddOrUpdateScriptReference(script.Handle, script.Name) || hasChanges;
		}

		return hasChanges;
	}

	bool EnsureProjectActiveScene(
		Ludus::Engine::Core::ProjectContext& projectContext,
		Ludus::Engine::Core::SceneHandle sceneHandle
	)
	{
		if (projectContext.Project.ActiveSceneHandle == sceneHandle)
		{
			return false;
		}

		projectContext.Project.ActiveSceneHandle = sceneHandle;
		return true;
	}
}

namespace Ludus::Editor::Commands::Requests::Scenes
{
	void CreateScene(const RequestCommand::CreateScene& command, CommandContext& context)
	{
		const auto finalName = GenerateUniqueSceneName(context);
		const auto handle = context.SystemContext.SceneRegistry.AddScene(finalName);

		context.EditorContext.Session.EnsureMetadata(
			handle,
			Ludus::Editor::Core::SceneMetadata {
				.Path = std::nullopt,
				.IsDirty = true,
				.IsOpenInHierarchy = true,
				.IsVisibleInViewports = true,
				.IsEditableInViewports = true
			}
		);

		context.EditorContext.Session.SetActiveScene(handle);
	}

	void OpenScene(const RequestCommand::OpenScene& command, CommandContext& context)
	{
		auto& editorContext = context.EditorContext;
		auto& systemContext = context.SystemContext;

		if (!systemContext.HasProjectContext())
		{
			LUDUS_LOG_WARN("Cannot open scene. No project context exists.");
			return;
		}

		auto& registry = systemContext.SceneRegistry;
		auto& session = editorContext.Session;

		if (!command.Additive)
		{
			registry.Clear();
			session.Clear();

			editorContext.State.Commands.AddEditCommand(Ludus::Editor::Commands::EditCommand::ClearSelection { });
		}

		if (const auto knownHandle = systemContext.ProjectContext.value().TryFindSceneHandleByPath(command.Path);
			knownHandle.has_value() && registry.Contains(*knownHandle))
		{
			session.EnsureMetadata(
				*knownHandle,
				Ludus::Editor::Core::SceneMetadata {
					.Path = command.Path,
					.IsDirty = false
				}
			);
			session.SetActiveScene(*knownHandle);
			return;
		}

		auto scene = systemContext.ProjectRepository.LoadScene(command.Path);
		const auto handle = scene.Handle;

		if (registry.Contains(handle))
		{
			session.EnsureMetadata(
				handle,
				Ludus::Editor::Core::SceneMetadata {
					.Path = command.Path,
					.IsDirty = false
				}
			);
			session.SetActiveScene(handle);
			return;
		}

		(void)systemContext.SceneRegistry.AddScene(std::move(scene));

		editorContext.Session.EnsureMetadata(
			handle,
			Ludus::Editor::Core::SceneMetadata {
				.Path = command.Path,
				.IsDirty = false
			}
		);
		editorContext.Session.SetActiveScene(handle);
	}

	void SaveScene(const RequestCommand::SaveScene& command, CommandContext& context)
	{
		auto& editorContext = context.EditorContext;
		auto& systemContext = context.SystemContext;

		if (!systemContext.HasProjectContext())
		{
			LUDUS_LOG_WARN("Cannot save scene. No project context exists.");
			return;
		}

		if (!systemContext.SceneRegistry.Contains(command.Handle))
		{
			LUDUS_LOG_WARN("Cannot save scene. Scene is not loaded.");
			return;
		}

		auto& projectContext = systemContext.ProjectContext.value();
		auto& sceneRegistry = systemContext.SceneRegistry;
		auto& scene = sceneRegistry.GetScene(command.Handle);

		if (EnsureProjectActiveScene(projectContext, command.Handle))
		{
			editorContext.Session.MarkProjectDirty();
		}

		const bool hasScriptReferenceChanges = EnsureProjectScriptReferences(scene, projectContext);
		if (hasScriptReferenceChanges)
		{
			editorContext.Session.MarkProjectDirty();
		}

		if (!projectContext.HasSceneReference(command.Handle))
		{
			auto path = systemContext.ProjectRepository.GetDefaultScenePath(projectContext, scene.Name);

			const bool hasSceneReferenceChange = projectContext.AddOrUpdateSceneReference(command.Handle, scene.Name, path);
			if (hasSceneReferenceChange)
			{
				editorContext.Session.MarkProjectDirty();
			}

			if (editorContext.Session.IsProjectDirty())
			{
				systemContext.ProjectRepository.SaveProject(projectContext);
				editorContext.Session.MarkProjectDirty(false);
			}
			systemContext.ProjectRepository.SaveScene(scene, path);

			editorContext.Session.SetPath(command.Handle, path);
			editorContext.Session.MarkDirty(command.Handle, false);

			RefreshProjectPanel(context);
			return;
		}

		if (editorContext.Session.IsProjectDirty())
		{
			systemContext.ProjectRepository.SaveProject(projectContext);
			editorContext.Session.MarkProjectDirty(false);
		}

		systemContext.ProjectRepository.SaveScene(projectContext, scene, command.Handle);
		RefreshProjectPanel(context);
	}

	void SaveSceneAs(const RequestCommand::SaveSceneAs& command, CommandContext& context)
	{
		auto& editorContext = context.EditorContext;
		auto& systemContext = context.SystemContext;

		if (!systemContext.HasProjectContext())
		{
			LUDUS_LOG_WARN("Cannot save scene. No project context exists.");
			return;
		}

		if (!systemContext.SceneRegistry.Contains(command.Handle))
		{
			LUDUS_LOG_WARN("Cannot save scene. Scene is not loaded.");
			return;
		}

		const auto& scene = systemContext.SceneRegistry.GetScene(command.Handle);

		if (EnsureProjectActiveScene(systemContext.ProjectContext.value(), command.Handle))
		{
			editorContext.Session.MarkProjectDirty();
		}

		systemContext.ProjectRepository.SaveSceneAs(command.Path, systemContext.ProjectContext.value(), scene, command.Handle);
		editorContext.Session.MarkProjectDirty(false);
		editorContext.Session.SetPath(command.Handle, command.Path);
		editorContext.Session.MarkDirty(command.Handle, false);

		RefreshProjectPanel(context);
	}
}
