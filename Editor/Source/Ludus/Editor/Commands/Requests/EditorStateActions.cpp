#include "pch.h"

#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Editor/Commands/Requests/EditorStateActions.h>
#include <Ludus/Editor/Commands/StartupCommandContext.h>
#include <Ludus/Editor/Persistence/IEditorPreferencesPersistence.h>
#include <Ludus/Editor/Persistence/ProjectPaths.h>
#include <Ludus/UI/Context/ThemeContext.h>

namespace Ludus::Editor::Commands::Requests::EditorState
{
	namespace
	{
		void SaveEditorPreferences(
			Ludus::Editor::Core::EditorPreferences& editorPreferences,
			const Ludus::Editor::Persistence::IEditorPreferencesPersistence& editorPreferencesPersistence
		)
		{
			Ludus::Editor::Persistence::ProjectPaths::EnsureEditorRootExists();
			editorPreferencesPersistence.Save(editorPreferences, Ludus::Editor::Persistence::ProjectPaths::EditorPreferencesFile());
		}

		void SetTheme(
			Ludus::UI::Theme::ThemeId themeId,
			Ludus::Editor::Core::ActiveThemeState& themeState,
			Ludus::Editor::Core::EditorPreferences& editorPreferences
		)
		{
			Ludus::UI::Context::ThemeContext::SetActiveTheme(themeId);
			themeState.ActiveThemeId = themeId;
			editorPreferences.ActiveThemeId = themeId;
		}
	}

	void CloseApplicationAction(ProjectSessionCommandContext& context)
	{
		SaveEditorPreferences(
			context.Preferences,
			context.Persistence.EditorPreferences
		);

		context.HostContext.SetWindowShouldClose();
	}

	void SetExecutionModeAction(Ludus::Editor::Core::ExecutionMode mode, ProjectSessionCommandContext& context)
	{
		switch (mode)
		{
			case Ludus::Editor::Core::ExecutionMode::Start:
				context.ProjectSession.StartSimulation(context.HostContext);
				break;
			case Ludus::Editor::Core::ExecutionMode::Stop:
				context.ProjectSession.StopSimulation(context.HostContext);
				break;
			case Ludus::Editor::Core::ExecutionMode::Pause:
				context.ProjectSession.PauseSimulation(context.HostContext);
				break;
		}

		context.Shell.State.Execution.ExecutionMode = mode;
		context.Shell.State.Execution.Apply(context.HostContext.GetExecutionFlags(), mode);
	}

	void SetExecutionFlagAction(Ludus::Editor::Core::EditorExecutionFlags flag, ProjectSessionCommandContext& context)
	{
		context.Shell.State.Execution.SetFlag(context.HostContext.GetExecutionFlags(), flag);
	}

	void SetThemeAction(Ludus::UI::Theme::ThemeId themeId, ProjectSessionCommandContext& context)
	{
		SetTheme(themeId, context.Shell.State.Theme, context.Preferences);
	}

	void UnsetExecutionFlagAction(Ludus::Editor::Core::EditorExecutionFlags flag, ProjectSessionCommandContext& context)
	{
		context.Shell.State.Execution.UnsetFlag(context.HostContext.GetExecutionFlags(), flag);
	}

	void CloseApplicationAction(StartupCommandContext& context)
	{
		SaveEditorPreferences(
			context.Preferences,
			context.Persistence.EditorPreferences
		);

		context.HostContext.SetWindowShouldClose();
	}

	void SetExecutionFlagAction(Ludus::Editor::Core::EditorExecutionFlags flag, StartupCommandContext& context)
	{
		context.Shell.State.Execution.SetFlag(context.HostContext.GetExecutionFlags(), flag);
	}

	void SetThemeAction(Ludus::UI::Theme::ThemeId themeId, StartupCommandContext& context)
	{
		SetTheme(themeId, context.Shell.State.Theme, context.Preferences);
	}

	void UnsetExecutionFlagAction(Ludus::Editor::Core::EditorExecutionFlags flag, StartupCommandContext& context)
	{
		context.Shell.State.Execution.UnsetFlag(context.HostContext.GetExecutionFlags(), flag);
	}
}
