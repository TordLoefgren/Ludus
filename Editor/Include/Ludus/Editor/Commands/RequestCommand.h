#pragma once

#include <filesystem>
#include <string>
#include <utility>
#include <variant>

#include <Ludus/Editor/Build/BuildCommand.h>
#include <Ludus/Editor/Commands/EntityReference.h>
#include <Ludus/Editor/Core/ExecutionMode.h>
#include <Ludus/Engine/Core/Scene.h>

namespace Ludus::Editor::Commands
{
	struct StartupCommandContext;
	struct ProjectSessionCommandContext;

	struct RequestCommand
	{
		struct AddViewport { };

		struct CreateScene { };
		struct CreateSceneAs { std::filesystem::path Path; };
		struct OpenScene { std::filesystem::path Path; bool Additive = false; };
		struct SaveScene { Ludus::Engine::Core::SceneHandle SceneHandle; };
		struct SaveSceneAs { Ludus::Engine::Core::SceneHandle SceneHandle; std::filesystem::path Path; };

		struct CreateProject { std::string Name; };
		struct CreateProjectAs { std::string Name; std::filesystem::path ProjectRoot; };
		struct OpenProject { std::filesystem::path Path; };
		struct CloseProject { };

		struct CreateScript { EntityReference EntityReference; Ludus::Engine::Core::SceneHandle SceneHandle; std::string Name; };
		struct BuildScript { Ludus::Editor::Build::BuildCommand BuildCommand; };

		struct SetExecutionMode { Ludus::Editor::Core::ExecutionMode Mode; };

		using Variant = std::variant<
			AddViewport,
			CreateScene, CreateSceneAs, OpenScene, SaveScene, SaveSceneAs,
			CreateProject, CreateProjectAs, OpenProject, CloseProject,
			CreateScript, BuildScript,
			SetExecutionMode
		>;

		Variant Data;

		template<typename T>
		RequestCommand(T value) : Data(std::move(value)) { }
	};

	void Execute(const RequestCommand& command, StartupCommandContext& context);
	void Execute(const RequestCommand& command, ProjectSessionCommandContext& context);
}
