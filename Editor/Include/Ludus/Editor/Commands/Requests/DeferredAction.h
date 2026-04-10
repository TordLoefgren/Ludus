#pragma once

#include <filesystem>
#include <string>
#include <utility>
#include <variant>

#include <Ludus/Editor/Build/BuildConfiguration.h>

namespace Ludus::Editor::Commands
{
	struct ProjectSessionCommandContext;
}

namespace Ludus::Editor::Commands::Requests
{
	struct DeferredAction
	{
		struct BuildRuntimeDeferredAction { Ludus::Editor::Build::BuildConfiguration BuildConfiguration; };
		struct CloseApplicationDeferredAction { };
		struct CloseProjectDeferredAction { };
		struct CreateProjectDeferredAction { std::string Name; };
		struct CreateProjectAsDeferredAction { std::string Name; std::filesystem::path Path; };
		struct CreateSceneDeferredAction { };
		struct CreateSceneAsDeferredAction { std::filesystem::path Path; };
		struct OpenProjectDeferredAction { std::filesystem::path Path; };
		struct OpenSceneDeferredAction { std::filesystem::path Path; };

		using Variant = std::variant<
			BuildRuntimeDeferredAction,
			CloseApplicationDeferredAction,
			CloseProjectDeferredAction,
			CreateProjectDeferredAction,
			CreateProjectAsDeferredAction,
			CreateSceneDeferredAction,
			CreateSceneAsDeferredAction,
			OpenProjectDeferredAction,
			OpenSceneDeferredAction
		>;

		Variant Data;

		static DeferredAction BuildRuntime(Ludus::Editor::Build::BuildConfiguration buildConfiguration)
		{
			return { DeferredAction::BuildRuntimeDeferredAction { buildConfiguration } };
		}

		static DeferredAction CloseProject()
		{
			return { DeferredAction::CloseProjectDeferredAction { } };
		}

		static DeferredAction CloseApplication()
		{
			return { DeferredAction::CloseApplicationDeferredAction { } };
		}

		static DeferredAction CreateProject(std::string name)
		{
			return { DeferredAction::CreateProjectDeferredAction { std::move(name) } };
		}

		static DeferredAction CreateProjectAs(std::string name, std::filesystem::path path)
		{
			return { DeferredAction::CreateProjectAsDeferredAction { std::move(name), std::move(path) } };
		}

		static DeferredAction CreateScene()
		{
			return { DeferredAction::CreateSceneDeferredAction { } };
		}

		static DeferredAction CreateSceneAs(std::filesystem::path path)
		{
			return { DeferredAction::CreateSceneAsDeferredAction { std::move(path) } };
		}

		static DeferredAction OpenProject(std::filesystem::path path)
		{
			return { DeferredAction::OpenProjectDeferredAction { std::move(path) } };
		}

		static DeferredAction OpenScene(std::filesystem::path path)
		{
			return { DeferredAction::OpenSceneDeferredAction { std::move(path) } };
		}
	};

	void ExecuteDeferredAction(
		const DeferredAction& deferredAction,
		ProjectSessionCommandContext& context
	);

	bool TryOpenUnsavedChangesDialog(
		const DeferredAction& action,
		ProjectSessionCommandContext& context
	);
}
