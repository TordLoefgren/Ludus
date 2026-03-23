#include "pch.h"

#include <filesystem>
#include <string>
#include <string_view>

#include <Ludus/Editor/Commands/RequestCommand.h>
#include <Ludus/Editor/Commands/UICommand.h>
#include <Ludus/Editor/Core/Constants.h>
#include <Ludus/Editor/Core/ExecutionMode.h>
#include <Ludus/Editor/Panels/DockPanel.h>
#include <Ludus/Editor/Persistence/Paths.h>
#include <Ludus/Engine/Core/SceneRegistry.h>
#include <Ludus/Engine/Persistence/Paths.h>
#include <Ludus/Engine/Platform/Modals.h>
#include <Ludus/Engine/Platform/Process.h>
#include <Ludus/UI/Context/DockingContext.h>
#include <Ludus/UI/Context/LayoutContext.h>
#include <Ludus/UI/Context/PopupContext.h>
#include <Ludus/UI/Context/ViewportContext.h>
#include <Ludus/UI/Context/WindowContext.h>
#include <Ludus/UI/Icons/FontAwesome.h>
#include <Ludus/UI/Labels.h>
#include <Ludus/UI/Scope/DisabledScope.h>
#include <Ludus/UI/Scope/MenuBarScope.h>
#include <Ludus/UI/Scope/MenuScope.h>
#include <Ludus/UI/Scope/ModalScope.h>
#include <Ludus/UI/Scope/StyleScope.h>
#include <Ludus/UI/Scope/WindowScope.h>
#include <Ludus/UI/Widgets/Buttons.h>
#include <Ludus/UI/Widgets/Input.h>
#include <Ludus/UI/Widgets/Menu.h>
#include <Ludus/UI/Widgets/Text.h>
#include <Ludus/UI/Widgets/Toggle.h>

namespace Ludus::Editor::Panels
{
	void DockPanel::DrawMenuBar(Ludus::Editor::Core::ProjectSessionContext& context)
	{
		if (Ludus::UI::Scope::MenuBarScope menuBar; menuBar)
		{
			const auto projectRoot = context.ProjectSession.GetProjectRoot();

			if (Ludus::UI::Scope::MenuScope fileMenu("File"); fileMenu)
			{
				// File -> Scene logic.
				{
					if (Ludus::UI::Widgets::MenuItem("New Scene"))
					{
						context.Shell.State.Commands.AddRequestCommand(Ludus::Editor::Commands::RequestCommand::CreateScene());
					}

					if (Ludus::UI::Widgets::MenuItem("Open Scene"))
					{
						std::filesystem::path path;
						if (Ludus::Engine::Platform::Modals::OpenFileDialog(
							path,
							"ludus.scene",
							Ludus::Engine::Persistence::Paths::ScenesDirectory(projectRoot)
						))
						{
							context.Shell.State.Commands.AddRequestCommand(Ludus::Editor::Commands::RequestCommand::OpenScene { path });
						}
					}

					Ludus::UI::Context::LayoutContext::Separator();

					if (Ludus::UI::Widgets::MenuItem("Save"))
					{
						const auto activeHandle = context.ProjectSession.EditorState.ActiveSceneHandle;
						if (!context.ProjectSession.GetSceneRegistry().Contains(activeHandle))
						{
							return;
						}

						if (context.ProjectSession.ActiveSceneHasSavePath())
						{
							context.Shell.State.Commands.AddRequestCommand(Ludus::Editor::Commands::RequestCommand::SaveScene { .SceneHandle = activeHandle });
						}
						else
						{
							std::filesystem::path path;
							if (Ludus::Engine::Platform::Modals::SaveFileDialog(
								path,
								"ludus.scene",
								Ludus::Engine::Persistence::Paths::ScenesDirectory(projectRoot),
								"Untitled"
							))
							{
								context.Shell.State.Commands.AddRequestCommand(Ludus::Editor::Commands::RequestCommand::SaveSceneAs { .SceneHandle = activeHandle, .Path = path });
							}
						}
					}

					if (Ludus::UI::Widgets::MenuItem("Save As"))
					{
						const auto activeHandle = context.ProjectSession.EditorState.ActiveSceneHandle;
						if (!context.ProjectSession.GetSceneRegistry().Contains(activeHandle))
						{
							return;
						}

						std::filesystem::path path;
						if (Ludus::Engine::Platform::Modals::SaveFileDialog(
							path,
							"ludus.scene",
							Ludus::Engine::Persistence::Paths::ScenesDirectory(projectRoot),
							"Untitled"
						))
						{
							context.Shell.State.Commands.AddRequestCommand(Ludus::Editor::Commands::RequestCommand::SaveSceneAs { .SceneHandle = activeHandle, .Path = path });
						}
					}
				}

				Ludus::UI::Context::LayoutContext::Separator();

				// File -> Project logic.
				if (Ludus::UI::Widgets::MenuItem("New Project"))
				{
					context.Shell.State.Commands.AddUICommand(Ludus::Editor::Commands::UICommand::OpenCreateProjectDialog { });
				}

				if (Ludus::UI::Widgets::MenuItem("Open Project"))
				{
					std::filesystem::path path;
					if (Ludus::Engine::Platform::Modals::OpenFileDialog(
						path,
						"ludus.project",
						Ludus::Editor::Persistence::Paths::ProjectsRoot()
					))
					{
						context.Shell.State.Commands.AddRequestCommand(Ludus::Editor::Commands::RequestCommand::OpenProject { path });
					}
				}

				if (Ludus::UI::Widgets::MenuItem("Close Project"))
				{
					context.Shell.State.Commands.AddRequestCommand(Ludus::Editor::Commands::RequestCommand::CloseProject { });
				}
			}

			// Edit -> Undo/Redo logic.
			{
				if (Ludus::UI::Scope::MenuScope editMenu("Edit"); editMenu)
				{
					Ludus::UI::Scope::DisabledScope disabled(true);

					if (Ludus::UI::Widgets::MenuItem("Undo"))
					{
						LUDUS_ASSERT(false, "Undo has not been implemented yet.");
					}

					if (Ludus::UI::Widgets::MenuItem("Redo"))
					{
						LUDUS_ASSERT(false, "Redo has not been implemented yet.");
					}
				}
			}

			// Assets.
			if (Ludus::UI::Scope::MenuScope assetsMenu("Assets"); assetsMenu)
			{
				if (Ludus::UI::Widgets::MenuItem("Show All Projects"))
				{
					Ludus::Engine::Platform::Paths::OpenFolder(
						Ludus::Editor::Persistence::Paths::ProjectsRoot()
					);
				}

				Ludus::UI::Context::LayoutContext::Separator();

				if (Ludus::UI::Widgets::MenuItem("Show in Explorer"))
				{
					Ludus::Engine::Platform::Paths::OpenFolder(
						Ludus::Engine::Persistence::Paths::AssetsDirectory(projectRoot)
					);
				}
			}

			if (Ludus::UI::Scope::MenuScope buildMenu("Build"); buildMenu)
			{
				Ludus::UI::Scope::DisabledScope disabledScope(
					context.Shell.State.Execution.ExecutionMode != Ludus::Editor::Core::ExecutionMode::Stop
				);

				if (Ludus::UI::Widgets::MenuItem("Build"))
				{
					context.Shell.State.Commands.AddRequestCommand(Ludus::Editor::Commands::RequestCommand::BuildScript { Ludus::Editor::Build::BuildCommand::Build });
				}

				if (Ludus::UI::Widgets::MenuItem("Rebuild"))
				{
					context.Shell.State.Commands.AddRequestCommand(Ludus::Editor::Commands::RequestCommand::BuildScript { Ludus::Editor::Build::BuildCommand::Rebuild });
				}

				if (Ludus::UI::Widgets::MenuItem("Clean"))
				{
					context.Shell.State.Commands.AddRequestCommand(Ludus::Editor::Commands::RequestCommand::BuildScript { Ludus::Editor::Build::BuildCommand::Clean });
				}
			}

			// View.
			if (Ludus::UI::Scope::MenuScope viewMenu("View"); viewMenu)
			{
				if (Ludus::UI::Widgets::MenuItem("Add viewport"))
				{
					context.Shell.State.Commands.AddRequestCommand(Ludus::Editor::Commands::RequestCommand::AddViewport { });
				}

				if (Ludus::UI::Scope::MenuScope panelsMenu("Panels"); panelsMenu)
				{
					Ludus::UI::Widgets::Checkbox("Console", &context.Shell.State.ActivePanelState.ShowConsolePanel);
					Ludus::UI::Widgets::Checkbox("Hierarchy", &context.Shell.State.ActivePanelState.ShowHierarchyPanel);
					Ludus::UI::Widgets::Checkbox("ImGuiDemo", &context.Shell.State.ActivePanelState.ShowImGuiDemoPanel);
					Ludus::UI::Widgets::Checkbox("Inspector", &context.Shell.State.ActivePanelState.ShowInspectorPanel);
					Ludus::UI::Widgets::Checkbox("Content", &context.Shell.State.ActivePanelState.ShowContentPanel);
				}
			}
		}
	}

	void DockPanel::DrawToolBar(Ludus::Editor::Core::ProjectSessionContext& context)
	{
		const auto buttonWidth = Ludus::Editor::Core::Constants::ToolbarButtonExtent;
		const auto spacing = Ludus::Editor::Core::Constants::StandardInlineSpacing;
		const auto buttonCount = 3;

		const auto totalWidth = buttonCount * buttonWidth + (buttonCount - 1) * spacing;
		const auto availableWidth = Ludus::UI::Context::WindowContext::GetContentRegionAvailable().X;
		Ludus::UI::Context::WindowContext::SetCursorPositionX((availableWidth - totalWidth) * 0.5f);

		const auto activeColor = Ludus::UI::Scope::GetStyleColor(Ludus::UI::Scope::Color::ButtonHovered);

		auto drawToolbarButton = [&](const std::string& label, bool isActive, bool isEnabled, auto&& onClick) -> bool
		{
			if (!isEnabled)
			{
				Ludus::UI::Scope::DisabledScope disabledScope(true);
				return Ludus::UI::Widgets::Button(label.c_str(), { buttonWidth, buttonWidth });
			}

			if (isActive)
			{
				Ludus::UI::Scope::StyleColorScope colorScope({
					{ Ludus::UI::Scope::Color::Button, activeColor },
					{ Ludus::UI::Scope::Color::ButtonHovered, activeColor },
					{ Ludus::UI::Scope::Color::ButtonActive, activeColor },
					});

				if (Ludus::UI::Widgets::Button(label.c_str(), { buttonWidth, buttonWidth }))
				{
					onClick();
					return true;
				}

				return false;
			}

			if (Ludus::UI::Widgets::Button(label.c_str(), { buttonWidth, buttonWidth }))
			{
				onClick();
				return true;
			}

			return false;
		};

		auto& mode = context.Shell.State.Execution.ExecutionMode;
		const bool isRunning = (mode != Ludus::Editor::Core::ExecutionMode::Stop);
		const bool isPaused = (mode == Ludus::Editor::Core::ExecutionMode::Pause);

		const auto playOrStopLabel = isRunning
			? Ludus::UI::CreateLabel(ICON_STOP, "Stop")
			: Ludus::UI::CreateLabel(ICON_PLAY, "Start");
		const auto pauseLabel = Ludus::UI::CreateLabel(ICON_PAUSE, "Pause");

		drawToolbarButton(playOrStopLabel, isRunning, true, [&]
		{
			const auto next = isRunning
				? Ludus::Editor::Core::ExecutionMode::Stop
				: Ludus::Editor::Core::ExecutionMode::Start;

			context.Shell.State.Commands.AddRequestCommand(
				Ludus::Editor::Commands::RequestCommand::SetExecutionMode { next }
			);
		});

		Ludus::UI::Context::LayoutContext::SameLine(0.0f, spacing);

		drawToolbarButton(pauseLabel, isPaused, isRunning, [&]
		{
			const auto next = isPaused
				? Ludus::Editor::Core::ExecutionMode::Start   // Resume.
				: Ludus::Editor::Core::ExecutionMode::Pause;

			context.Shell.State.Commands.AddRequestCommand(
				Ludus::Editor::Commands::RequestCommand::SetExecutionMode { next }
			);
		});
	}

	bool DockPanel::UpdateImpl(Ludus::Editor::Core::ProjectSessionContext& context)
	{
		const auto viewport = Ludus::UI::Context::ViewportContext::GetMainViewport();
		Ludus::UI::Context::WindowContext::SetNextWindowPosition(viewport.WorkPosition);
		Ludus::UI::Context::WindowContext::SetNextWindowSize(viewport.WorkSize);
		Ludus::UI::Context::WindowContext::SetNextWindowViewport(viewport.Id);

		Ludus::UI::Scope::StyleVarScope styleVar(
			{
				Ludus::UI::Scope::StyleVar::Float(Ludus::UI::Scope::Variable::WindowRounding, 0.0f),
				Ludus::UI::Scope::StyleVar::Float(Ludus::UI::Scope::Variable::WindowBorderSize, 0.0f)
			}
		);

		// The dock panel should never close, as it enables docking for all other panels.
		auto windowTitle = CreateWindowTitle("DockPanel");
		if (Ludus::UI::Scope::WindowScope window(windowTitle.c_str(), nullptr, Ludus::Editor::Core::Constants::DockPanelWindowFlags); window)
		{
			DrawMenuBar(context);
			DrawToolBar(context);

			Ludus::UI::Context::DockingContext::CreateDockSpace(windowTitle.c_str(), { 0.0f, 0.0f }, Ludus::UI::Flags::DockNode::None | Ludus::UI::Flags::DockNodeInternal::NoWindowMenuButton);
		}

		return true;
	}
}
