#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <utility>
#include <variant>

#include <Ludus/Editor/Build/BuildCommand.h>
#include <Ludus/Editor/Build/BuildConfiguration.h>
#include <Ludus/Editor/Build/BuildTarget.h>
#include <Ludus/Editor/Commands/EntityReference.h>
#include <Ludus/Editor/Commands/Requests/DeferredAction.h>
#include <Ludus/Editor/Core/EditorExecutionFlags.h>
#include <Ludus/Editor/Core/ExecutionMode.h>
#include <Ludus/Editor/Dialogs/UnsavedChangesResult.h>
#include <Ludus/Editor/Panels/PanelKind.h>
#include <Ludus/Engine/Core/Id.h>
#include <Ludus/UI/Theme/ThemeId.h>

namespace Ludus::Editor::Commands
{
	struct StartupCommandContext;
	struct ProjectSessionCommandContext;

	struct RequestCommand
	{
		struct AddViewport { };
		struct SetExecutionMode { Ludus::Editor::Core::ExecutionMode Mode; };
		struct SetExecutionFlag { Ludus::Editor::Core::EditorExecutionFlags Flag; };
		struct UnsetExecutionFlag { Ludus::Editor::Core::EditorExecutionFlags Flag; };
		struct SetPanelVisibility { Ludus::Editor::Panels::PanelKind PanelKind; bool IsVisible; };
		struct SetTheme { Ludus::UI::Theme::ThemeId ThemeId; };
		struct CloseApplication { };

		struct CreateScene { };
		struct CreateSceneAs { std::filesystem::path Path; };
		struct OpenScene { std::filesystem::path Path; };
		struct SaveScene { Ludus::Engine::Core::SceneId SceneId; };
		struct SaveSceneAs { Ludus::Engine::Core::SceneId SceneId; std::filesystem::path Path; };
		struct RenameScene { Ludus::Engine::Core::SceneId SceneId; std::filesystem::path Path; };

		struct CreateProject { std::string Name; };
		struct CreateProjectAs { std::string Name; std::filesystem::path Path; };
		struct OpenProject { std::filesystem::path Path; };
		struct SaveProject { };
		struct CloseProject { };

		struct CreateScript { Ludus::Engine::Core::SceneId SceneId; EntityReference EntityReference; std::string Name; };

		struct RunTargetBuildCommand
		{
			Ludus::Editor::Build::BuildTarget BuildTarget;
			Ludus::Editor::Build::BuildCommand BuildCommand;
			Ludus::Editor::Build::BuildConfiguration BuildConfiguration;
		};
		struct BuildRuntime { Ludus::Editor::Build::BuildConfiguration BuildConfiguration; };
		struct CleanRuntime { };

		struct ResolveUnsavedChanges { Ludus::Editor::Commands::Requests::DeferredAction DeferredAction; Ludus::Editor::Dialogs::UnsavedChangesResult Result; };

		using Variant = std::variant<
			AddViewport, SetExecutionMode, SetExecutionFlag, UnsetExecutionFlag, SetPanelVisibility, SetTheme, CloseApplication,
			CreateScene, CreateSceneAs, OpenScene, SaveScene, SaveSceneAs, RenameScene,
			CreateProject, CreateProjectAs, OpenProject, SaveProject, CloseProject,
			CreateScript,
			RunTargetBuildCommand, BuildRuntime, CleanRuntime,
			ResolveUnsavedChanges
		>;

		Variant Data;

		template<typename T>
		RequestCommand(T value) : Data(std::move(value)) { }
	};

	void Execute(const RequestCommand& command, StartupCommandContext& context);
	void Execute(const RequestCommand& command, ProjectSessionCommandContext& context);
}
