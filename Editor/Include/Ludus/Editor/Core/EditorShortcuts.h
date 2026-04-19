#pragma once

#include <array>

#include <Ludus/Editor/Build/BuildCommand.h>
#include <Ludus/Editor/Build/BuildConfiguration.h>
#include <Ludus/Editor/Build/BuildTarget.h>
#include <Ludus/Editor/Commands/RequestCommand.h>
#include <Ludus/Editor/Core/EditorShortcutContext.h>
#include <Ludus/Editor/Core/ExecutionMode.h>
#include <Ludus/Engine/Windowing/Input.h>
#include <Ludus/Engine/Windowing/Key.h>
#include <Ludus/UI/Context/InputContext.h>

namespace Ludus::Editor::Core
{
	struct ShortcutBinding
	{
		Ludus::Engine::Windowing::Key Key;
		bool Control = false;
		bool Shift = false;
		bool Alt = false;
	};

	struct ShortcutDefinition
	{
		ShortcutBinding Binding;
		bool (*IsEnabled)(const Ludus::Editor::Core::EditorShortcutContext& context);
		void (*EnqueueCommand)(Ludus::Editor::Core::EditorShortcutContext& context);

		static constexpr ShortcutDefinition Create(
			ShortcutBinding binding,
			bool(*isEnabled)(const Ludus::Editor::Core::EditorShortcutContext&),
			void(*enqueueCommand)(Ludus::Editor::Core::EditorShortcutContext&)
		)
		{
			return {
				.Binding = binding,
				.IsEnabled = isEnabled,
				.EnqueueCommand = enqueueCommand
			};
		}
	};

#pragma region Input Helper

	bool inline IsControlPressed(Ludus::Engine::Windowing::Input& input)
	{
		return input.GetKey(Ludus::Engine::Windowing::Key::LeftControl) || input.GetKey(Ludus::Engine::Windowing::Key::RightControl);
	}

	bool inline IsShiftPressed(Ludus::Engine::Windowing::Input& input)
	{
		return input.GetKey(Ludus::Engine::Windowing::Key::LeftShift) || input.GetKey(Ludus::Engine::Windowing::Key::RightShift);
	}

	bool inline IsAltPressed(Ludus::Engine::Windowing::Input& input)
	{
		return input.GetKey(Ludus::Engine::Windowing::Key::LeftAlt) || input.GetKey(Ludus::Engine::Windowing::Key::RightAlt);
	}

	inline bool IsShortcutPressed(Ludus::Engine::Windowing::Input& input, const ShortcutBinding& binding)
	{
		const auto isControlPressed = IsControlPressed(input);
		const auto isShiftPressed = IsShiftPressed(input);
		const auto isAltPressed = IsAltPressed(input);

		if (binding.Control != isControlPressed)
		{
			return false;
		}

		if (binding.Shift != isShiftPressed)
		{
			return false;
		}

		if (binding.Alt != isAltPressed)
		{
			return false;
		}

		return input.GetKeyDown(binding.Key);
	}

#pragma endregion

#pragma region Predicate Helpers

	bool inline IsTextInputActive(const EditorShortcutContext&)
	{
		return Ludus::UI::Context::InputContext::WantTextInput();
	}

	bool inline HasProjectSession(const EditorShortcutContext& context)
	{
		return context.ProjectSession != nullptr;
	}

	bool inline IsSimulationActive(const EditorShortcutContext& context)
	{
		return context.Shell.State.Execution.ExecutionMode != Ludus::Editor::Core::ExecutionMode::Stop;
	}

	bool inline IsSimulationStopped(const EditorShortcutContext& context)
	{
		return context.Shell.State.Execution.ExecutionMode == Ludus::Editor::Core::ExecutionMode::Stop;
	}

	bool inline IsShortcutInputAllowed(const EditorShortcutContext& context)
	{
		return !IsTextInputActive(context);
	}

	bool inline IsProjectShortcutAllowed(const EditorShortcutContext& context)
	{
		return HasProjectSession(context) && IsShortcutInputAllowed(context);
	}

	bool inline IsEditingShortcutAllowed(const EditorShortcutContext& context)
	{
		return IsProjectShortcutAllowed(context) && IsSimulationStopped(context);
	}

#pragma endregion

#pragma region Command Predicates

	bool inline IsSaveSceneEnabled(const EditorShortcutContext& context)
	{
		return IsEditingShortcutAllowed(context);
	}

	bool inline IsSaveProjectEnabled(const EditorShortcutContext& context)
	{
		return IsProjectShortcutAllowed(context);
	}

	bool inline IsRebuildScriptsEnabled(const EditorShortcutContext& context)
	{
		return IsEditingShortcutAllowed(context);
	}

	bool inline IsStartSimulationEnabled(const EditorShortcutContext& context)
	{
		return IsProjectShortcutAllowed(context) && IsSimulationStopped(context);
	}

	bool inline IsStopSimulationEnabled(const EditorShortcutContext& context)
	{
		return IsProjectShortcutAllowed(context) && IsSimulationActive(context);
	}

#pragma endregion

#pragma region Command Enqueues

	void inline EnqueueSaveScene(Ludus::Editor::Core::EditorShortcutContext& context)
	{
		const auto activeSceneId = context.ProjectSession->EditorState.GetActiveSceneId();
		context.Shell.State.Commands.AddRequestCommand(Ludus::Editor::Commands::RequestCommand::SaveScene { .SceneId = activeSceneId });
	}

	void inline EnqueueSaveProject(Ludus::Editor::Core::EditorShortcutContext& context)
	{
		context.Shell.State.Commands.AddRequestCommand(Ludus::Editor::Commands::RequestCommand::SaveProject { });
	}

	void inline EnqueueRebuildScripts(Ludus::Editor::Core::EditorShortcutContext& context)
	{
		context.Shell.State.Commands.AddRequestCommand(
			Ludus::Editor::Commands::RequestCommand::RunTargetBuildCommand
			{
				.BuildTarget = Ludus::Editor::Build::BuildTarget::Scripts,
				.BuildCommand = Ludus::Editor::Build::BuildCommand::Rebuild,
				.BuildConfiguration = Ludus::Editor::Build::BuildConfiguration::Debug
			});
	}

	void inline EnqueueStartSimulation(Ludus::Editor::Core::EditorShortcutContext& context)
	{
		context.Shell.State.Commands.AddRequestCommand(
			Ludus::Editor::Commands::RequestCommand::SetExecutionMode
			{
				.Mode = Ludus::Editor::Core::ExecutionMode::Start
			});
	}

	void inline EnqueueStopSimulation(Ludus::Editor::Core::EditorShortcutContext& context)
	{
		context.Shell.State.Commands.AddRequestCommand(
			Ludus::Editor::Commands::RequestCommand::SetExecutionMode
			{
				.Mode = Ludus::Editor::Core::ExecutionMode::Stop
			});
	}

#pragma endregion

	inline constexpr std::array<ShortcutDefinition, 5> Shortcuts = {
		ShortcutDefinition::Create({ .Key = Ludus::Engine::Windowing::Key::S, .Control = true }, &IsSaveSceneEnabled, &EnqueueSaveScene),
		ShortcutDefinition::Create({ .Key = Ludus::Engine::Windowing::Key::S, .Control = true, .Shift = true }, &IsSaveProjectEnabled, &EnqueueSaveProject),
		ShortcutDefinition::Create({ .Key = Ludus::Engine::Windowing::Key::B, .Control = true, .Shift = true }, &IsRebuildScriptsEnabled, &EnqueueRebuildScripts),
		ShortcutDefinition::Create({ .Key = Ludus::Engine::Windowing::Key::F5 }, &IsStartSimulationEnabled, &EnqueueStartSimulation),
		ShortcutDefinition::Create({ .Key = Ludus::Engine::Windowing::Key::F5, .Shift = true }, &IsStopSimulationEnabled, &EnqueueStopSimulation)
	};

	void inline DelegateShortcutCommands(Ludus::Editor::Core::EditorShortcutContext& context)
	{
		auto& input = context.HostContext.GetInput();

		for (const auto& shortcut : Shortcuts)
		{
			if (shortcut.IsEnabled(context) && IsShortcutPressed(input, shortcut.Binding))
			{
				shortcut.EnqueueCommand(context);
			}
		}
	}
}
