#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <utility>
#include <variant>

#include <Ludus/Editor/Commands/EntityReference.h>
#include <Ludus/Editor/Core/ExecutionMode.h>
#include <Ludus/Engine/Core/Scene.h>

namespace Ludus::Editor::Commands
{
	struct CommandContext;

	struct RequestCommand
	{
		struct AddViewport { };

		struct CreateScene { };
		struct OpenScene { std::filesystem::path Path; bool Additive = false; };
		struct SaveScene { Ludus::Engine::Core::SceneHandle Handle; };
		struct SaveSceneAs { Ludus::Engine::Core::SceneHandle Handle; std::filesystem::path Path; };

		struct CreateProject { std::string Name; std::optional<std::filesystem::path> RootPath = std::nullopt; };
		struct OpenProject { std::filesystem::path Path; };
		struct CloseProject { };

		struct CreateScript { EntityReference Entity; Ludus::Engine::Core::SceneHandle Scene; std::string Name; };

		struct SetExecutionMode { Ludus::Editor::Core::ExecutionMode Mode; };

		using Variant = std::variant<
			AddViewport,
			CreateScene, OpenScene, SaveScene, SaveSceneAs,
			CreateProject, OpenProject, CloseProject,
			CreateScript,
			SetExecutionMode
		>;

		Variant Data;

		template<typename T>
		RequestCommand(T value) : Data(std::move(value)) { }
	};

	void Execute(const RequestCommand& command, CommandContext& context);
}
