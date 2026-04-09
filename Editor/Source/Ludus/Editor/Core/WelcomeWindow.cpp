#include "pch.h"

#include <string>

#include <Ludus/Editor/Commands/RequestCommand.h>
#include <Ludus/Editor/Commands/UICommand.h>
#include <Ludus/Editor/Core/Constants.h>
#include <Ludus/Editor/Core/WelcomeWindow.h>
#include <Ludus/Editor/Persistence/ProjectPaths.h>
#include <Ludus/Engine/Math/Vector2D.h>
#include <Ludus/Engine/Platform/Modals.h>
#include <Ludus/UI/Context/LayoutContext.h>
#include <Ludus/UI/Context/ThemeContext.h>
#include <Ludus/UI/Context/ViewportContext.h>
#include <Ludus/UI/Context/WindowContext.h>
#include <Ludus/UI/Scope/FontScope.h>
#include <Ludus/UI/Scope/StyleScope.h>
#include <Ludus/UI/Scope/WindowScope.h>
#include <Ludus/UI/Widgets/Buttons.h>
#include <Ludus/UI/Widgets/Text.h>

namespace Ludus::Editor::Core
{
	std::optional<Ludus::Editor::Commands::CommandSet> WelcomeWindow::Update()
	{
		const auto viewport = Ludus::UI::Context::ViewportContext::GetMainViewport();

		Ludus::UI::Context::WindowContext::SetNextWindowPosition(viewport.Position);
		Ludus::UI::Context::WindowContext::SetNextWindowSize(viewport.Size);
		Ludus::UI::Context::WindowContext::SetNextWindowViewport(viewport.Id);

		Ludus::UI::Scope::StyleVarScope backgroundStyle({
			Ludus::UI::Scope::StyleVar::Float(Ludus::UI::Scope::Variable::WindowRounding, 0.0f),
			Ludus::UI::Scope::StyleVar::Float(Ludus::UI::Scope::Variable::WindowBorderSize, 0.0f),
			Ludus::UI::Scope::StyleVar::Vector(Ludus::UI::Scope::Variable::WindowPadding, { 0.0f, 0.0f })
			});

		Ludus::UI::Scope::StyleColorScope backgroundColor({
			{ Ludus::UI::Scope::Color::WindowBg, Ludus::UI::Context::ThemeContext::OverlayBackground() }
			});

		Ludus::UI::Scope::WindowScope backgroundWindow(
			"WelcomeBackground##Background",
			nullptr,
			Ludus::Editor::Core::Constants::WelcomeBackgroundWindowFlags
		);

		const auto windowSize = Ludus::Editor::Core::Constants::WelcomeWindowSize;

		const Ludus::Engine::Math::Vector2D windowPosition {
			viewport.Position.X + (viewport.Size.X - windowSize.X) * 0.5f,
			viewport.Position.Y + (viewport.Size.Y - windowSize.Y) * 0.5f
		};

		Ludus::UI::Context::WindowContext::SetNextWindowPosition(windowPosition);
		Ludus::UI::Context::WindowContext::SetNextWindowSize(windowSize);
		Ludus::UI::Context::WindowContext::SetNextWindowViewport(viewport.Id);

		Ludus::UI::Scope::StyleVarScope welcomeWindowStyle({
			Ludus::UI::Scope::StyleVar::Float(Ludus::UI::Scope::Variable::WindowBorderSize, 1.0f),
			Ludus::UI::Scope::StyleVar::Float(Ludus::UI::Scope::Variable::WindowRounding, 6.0f)
			});

		Ludus::UI::Scope::StyleColorScope welcomeWindowColors({
			{ Ludus::UI::Scope::Color::WindowBg, Ludus::UI::Context::ThemeContext::PanelBackground() },
			{ Ludus::UI::Scope::Color::Border, Ludus::UI::Context::ThemeContext::BorderSubtle() }
			});

		Ludus::UI::Scope::WindowScope window(
			"Welcome to Ludus",
			nullptr,
			Ludus::Editor::Core::Constants::WelcomeWindowFlags
		);

		if (!window)
		{
			return std::nullopt;
		}

		const std::string title = "LUDUS";
		const std::string subtitle = "Create or open a project";
		const std::string version = "v0.2.0";
		const auto buttonSize = Ludus::Editor::Core::Constants::WelcomeActionButtonSize;

		const auto centerTextAtY = [&](const std::string& text, float y)
		{
			const auto textSize = Ludus::UI::Context::LayoutContext::CalculateTextSize(text);
			Ludus::UI::Context::WindowContext::SetCursorPosition({
				(windowSize.X - textSize.X) * 0.5f,
				y
				});
		};

		const auto centerButtonAtY = [&](float y)
		{
			Ludus::UI::Context::WindowContext::SetCursorPosition({
				(windowSize.X - buttonSize.X) * 0.5f,
				y
				});
		};

		// Title
		{
			Ludus::UI::Scope::FontScope font(42.0f);
			centerTextAtY(title, 28.0f);
			Ludus::UI::Widgets::TextUnformatted(title);
		}

		// Subtitle
		{
			Ludus::UI::Scope::StyleColorScope subtitleColor({
				{ Ludus::UI::Scope::Color::Text, Ludus::UI::Context::ThemeContext::TextSecondary() }
				});

			centerTextAtY(subtitle, 84.0f);
			Ludus::UI::Widgets::TextUnformatted(subtitle);
		}

		// Primary action
		{
			Ludus::UI::Scope::StyleColorScope primaryButtonColors({
				{ Ludus::UI::Scope::Color::Button, Ludus::UI::Context::ThemeContext::Accent() },
				{ Ludus::UI::Scope::Color::ButtonHovered, Ludus::UI::Context::ThemeContext::AccentHover() },
				{ Ludus::UI::Scope::Color::ButtonActive, Ludus::UI::Context::ThemeContext::AccentActive() }
				});

			centerButtonAtY(138.0f);

			if (Ludus::UI::Widgets::Button("Create Project", buttonSize))
			{
				Ludus::Editor::Commands::CommandSet out;
				out.UICommands.emplace_back(
					Ludus::Editor::Commands::UICommand::OpenCreateProjectDialog { }
				);
				return out;
			}
		}

		// Secondary action
		centerButtonAtY(208.0f);

		if (Ludus::UI::Widgets::Button("Open Project", buttonSize))
		{
			std::filesystem::path path;

			if (Ludus::Engine::Platform::Modals::OpenFileDialog(
				path,
				"project.ludus",
				Ludus::Editor::Persistence::ProjectPaths::ProjectsRoot()))
			{
				Ludus::Editor::Commands::CommandSet out;
				out.RequestCommands.emplace_back(
					Ludus::Editor::Commands::RequestCommand::OpenProject { path }
				);
				return out;
			}
		}

		// Footer
		{
			Ludus::UI::Scope::StyleColorScope footerColor({
				{ Ludus::UI::Scope::Color::Text, Ludus::UI::Context::ThemeContext::TextDisabled() }
				});

			centerTextAtY(version, windowSize.Y - 28.0f);
			Ludus::UI::Widgets::TextUnformatted(version);
		}

		return std::nullopt;
	}
}
