#include "pch.h"

#include <string>

#include <Ludus/Editor/Commands/RequestCommand.h>
#include <Ludus/Editor/Commands/UICommand.h>
#include <Ludus/Editor/Core/Constants.h>
#include <Ludus/Editor/Core/WelcomeWindow.h>
#include <Ludus/Editor/Persistence/Paths.h>
#include <Ludus/Engine/Math/Vector2D.h>
#include <Ludus/Engine/Platform/Modals.h>
#include <Ludus/UI/Context/LayoutContext.h>
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

		Ludus::UI::Scope::StyleVarScope backgroundStyle(
			{
				Ludus::UI::Scope::StyleVar::Float(Ludus::UI::Scope::Variable::WindowRounding, 0.0f),
				Ludus::UI::Scope::StyleVar::Float(Ludus::UI::Scope::Variable::WindowBorderSize, 0.0f),
				Ludus::UI::Scope::StyleVar::Vector(Ludus::UI::Scope::Variable::WindowPadding, { 0.0f, 0.0f })
			}
		);
		Ludus::UI::Scope::StyleColorScope backgroundColor(
			{
				{ Ludus::UI::Scope::Color::WindowBg, Ludus::Engine::Graphics::Colors::Black }
			}
		);

		Ludus::UI::Scope::WindowScope backgroundWindow(
			"WelcomeBackground##Background",
			nullptr,
			Ludus::Editor::Core::Constants::WelcomeBackgroundWindowFlags
		);

		const auto startupWindowSize = Ludus::Editor::Core::Constants::WelcomeWindowSize;
		const Ludus::Engine::Math::Vector2D startupWindowPosition
		{
			viewport.Position.X + (viewport.Size.X - startupWindowSize.X) * 0.5f,
			viewport.Position.Y + (viewport.Size.Y - startupWindowSize.Y) * 0.5f
		};

		Ludus::UI::Context::WindowContext::SetNextWindowPosition(startupWindowPosition);
		Ludus::UI::Context::WindowContext::SetNextWindowSize(startupWindowSize);
		Ludus::UI::Context::WindowContext::SetNextWindowViewport(viewport.Id);

		Ludus::UI::Scope::StyleVarScope welcomeWindowStyle(
			{
				Ludus::UI::Scope::StyleVar::Float(Ludus::UI::Scope::Variable::WindowBorderSize, 1.0f),
				Ludus::UI::Scope::StyleVar::Float(Ludus::UI::Scope::Variable::WindowRounding, 4.0f)
			}
		);
		Ludus::UI::Scope::StyleColorScope welcomeWindowColors(
			{
				{ Ludus::UI::Scope::Color::WindowBg, Ludus::Engine::Graphics::Colors::DarkGray },
			{ Ludus::UI::Scope::Color::Border, Ludus::Engine::Graphics::Colors::Gray }
			}
		);

		Ludus::UI::Scope::WindowScope window(
			"Welcome to Ludus",
			nullptr,
			Ludus::Editor::Core::Constants::WelcomeWindowFlags
		);
		if (!window)
		{
			return std::nullopt;
		}

		{
			Ludus::UI::Scope::FontScope font(42.0f);
			const std::string title = "LUDUS";
			const auto titleSize = Ludus::UI::Context::LayoutContext::CalculateTextSize(title);
			Ludus::UI::Context::WindowContext::SetCursorPosition({
				(startupWindowSize.X - titleSize.X) * 0.5f,
				24.0f
				});
			Ludus::UI::Widgets::TextUnformatted(title);
		}

		const auto buttonSize = Ludus::Editor::Core::Constants::WelcomeActionButtonSize;
		const Ludus::Engine::Math::Vector2D position { 140.0f, 120.0f };
		Ludus::UI::Context::WindowContext::SetCursorPosition(position);

		if (Ludus::UI::Widgets::Button("Create New", buttonSize))
		{
			Ludus::Editor::Commands::CommandSet out;
			out.UICommands.emplace_back(Ludus::Editor::Commands::UICommand::OpenCreateProjectDialog { });
			return out;
		}

		Ludus::UI::Context::WindowContext::SetCursorPosition({ position.X, position.Y + 75.0f });
		if (Ludus::UI::Widgets::Button("Open", buttonSize))
		{
			std::filesystem::path path;
			if (Ludus::Engine::Platform::Modals::OpenFileDialog(
				path,
				"ludus.project",
				Ludus::Editor::Persistence::Paths::ProjectsRoot()
			))
			{
				Ludus::Editor::Commands::CommandSet out;
				out.RequestCommands.emplace_back(Ludus::Editor::Commands::RequestCommand::OpenProject { path });
				return out;
			}
		}

		return std::nullopt;
	}
}
