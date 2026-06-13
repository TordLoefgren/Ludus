#include "pch.h"

#include <filesystem>
#include <string>

#include <Ludus/Editor/Commands/RequestCommand.h>
#include <Ludus/Editor/Commands/UICommand.h>
#include <Ludus/Editor/Core/Constants.h>
#include <Ludus/Editor/Core/WelcomeWindow.h>
#include <Ludus/Editor/Persistence/ProjectPaths.h>
#include <Ludus/Editor/Widgets/Buttons.h>
#include <Ludus/Editor/Widgets/Layout.h>
#include <Ludus/Engine/Core/Version.h>
#include <Ludus/Engine/Math/Vector2D.h>
#include <Ludus/Engine/Platform/Modals.h>
#include <Ludus/UI/Context/ThemeContext.h>
#include <Ludus/UI/Context/ViewportContext.h>
#include <Ludus/UI/Context/WindowContext.h>
#include <Ludus/UI/Scope/ChildScope.h>
#include <Ludus/UI/Scope/FontScope.h>
#include <Ludus/UI/Scope/StyleScope.h>
#include <Ludus/UI/Scope/WindowScope.h>
#include <Ludus/UI/Widgets/Buttons.h>
#include <Ludus/UI/Widgets/Text.h>

namespace Ludus::Editor::Core
{
	namespace
	{
		Ludus::Editor::Commands::CommandSet CreateProjectCommandSet()
		{
			Ludus::Editor::Commands::CommandSet out;
			out.UICommands.emplace_back(
				Ludus::Editor::Commands::UICommand::OpenCreateProjectDialog { }
			);
			return out;
		}

		Ludus::Editor::Commands::CommandSet OpenProjectCommandSet(const std::filesystem::path& path)
		{
			Ludus::Editor::Commands::CommandSet out;
			out.RequestCommands.emplace_back(
				Ludus::Editor::Commands::RequestCommand::OpenProject { path }
			);
			return out;
		}

		void DrawWelcomeHeader(const Ludus::Engine::Math::Vector2D& windowSize)
		{
			const std::string title = "LUDUS";
			const std::string subtitle = "Create or open a project";

			{
				Ludus::UI::Scope::FontScope font(Ludus::Editor::Core::Constants::Welcome::Layout::TitleFontSize);
				Ludus::UI::Context::WindowContext::SetCursorPosition(
					Ludus::Editor::Widgets::Layout::GetCenteredTextPositionAtY(
						title,
						Ludus::Editor::Core::Constants::Welcome::Layout::TitleY,
						windowSize.X
					)
				);
				Ludus::UI::Widgets::TextUnformatted(title);
			}

			{
				const auto subtitleTextColor = Ludus::UI::Scope::StyleColor(
					Ludus::UI::Scope::Color::Text,
					Ludus::UI::Context::ThemeContext::TextSecondary()
				);

				Ludus::UI::Scope::StyleColorScope subtitleColor({ subtitleTextColor });

				Ludus::UI::Context::WindowContext::SetCursorPosition(
					Ludus::Editor::Widgets::Layout::GetCenteredTextPositionAtY(
						subtitle,
						Ludus::Editor::Core::Constants::Welcome::Layout::SubtitleY,
						windowSize.X
					)
				);
				Ludus::UI::Widgets::TextUnformatted(subtitle);
			}
		}

		std::optional<Ludus::Editor::Commands::CommandSet> DrawWelcomeActionButtons(
			float windowWidth,
			const Ludus::Engine::Math::Vector2D& actionButtonSize
		)
		{
			const auto actionButtonsSpacing = Ludus::Editor::Core::Constants::Welcome::Layout::ActionButtonsSpacing;

			{
				const auto buttonColor = Ludus::UI::Scope::StyleColor(
					Ludus::UI::Scope::Color::Button,
					Ludus::UI::Context::ThemeContext::Accent()
				);
				const auto buttonHoveredColor = Ludus::UI::Scope::StyleColor(
					Ludus::UI::Scope::Color::ButtonHovered,
					Ludus::UI::Context::ThemeContext::AccentHover()
				);
				const auto buttonActiveColor = Ludus::UI::Scope::StyleColor(
					Ludus::UI::Scope::Color::ButtonActive,
					Ludus::UI::Context::ThemeContext::AccentActive()
				);

				Ludus::UI::Scope::StyleColorScope primaryButtonColors(
					{
						buttonColor,
						buttonHoveredColor,
						buttonActiveColor
					}
				);

				Ludus::UI::Context::WindowContext::SetCursorPosition(
					Ludus::Editor::Widgets::Layout::GetCenteredRowElementPositionAtY(
						Ludus::Editor::Core::Constants::Welcome::Layout::ActionButtonsY,
						windowWidth,
						actionButtonSize.X,
						actionButtonsSpacing,
						2,
						0
					)
				);

				if (Ludus::UI::Widgets::Button("Create Project", actionButtonSize))
				{
					return CreateProjectCommandSet();
				}
			}

			Ludus::UI::Context::WindowContext::SetCursorPosition(
				Ludus::Editor::Widgets::Layout::GetCenteredRowElementPositionAtY(
					Ludus::Editor::Core::Constants::Welcome::Layout::ActionButtonsY,
					windowWidth,
					actionButtonSize.X,
					actionButtonsSpacing,
					2,
					1
				)
			);

			if (Ludus::UI::Widgets::Button("Open Project", actionButtonSize))
			{
				std::filesystem::path path;
				if (Ludus::Engine::Platform::Modals::OpenFileDialog(
					path,
					Ludus::Editor::Persistence::ProjectPaths::Constants::ProjectManifestExtension,
					Ludus::Editor::Persistence::ProjectPaths::ProjectsRoot()
				))
				{
					return OpenProjectCommandSet(path);
				}
			}

			return std::nullopt;
		}

		std::optional<Ludus::Editor::Commands::CommandSet> DrawRecentProjectsSection(
			std::vector<RecentlyOpenedProject>& recentlyOpenedProjects,
			std::string& error,
			const Ludus::Engine::Math::Vector2D& windowSize,
			float recentListWidth,
			float recentListHeight
		)
		{
			const std::string recentProjectsLabel = "Recently Opened Projects";

			{
				const auto subtitleTextColor = Ludus::UI::Scope::StyleColor(
					Ludus::UI::Scope::Color::Text,
					Ludus::UI::Context::ThemeContext::TextSecondary()
				);

				Ludus::UI::Scope::StyleColorScope subtitleColor({ subtitleTextColor });

				Ludus::UI::Context::WindowContext::SetCursorPosition(
					Ludus::Editor::Widgets::Layout::GetCenteredTextPositionAtY(
						recentProjectsLabel,
						Ludus::Editor::Core::Constants::Welcome::Layout::RecentProjectsLabelY,
						windowSize.X
					)
				);

				Ludus::UI::Widgets::TextUnformatted(recentProjectsLabel);
			}

			const auto childBorderSize = Ludus::UI::Scope::StyleVar::Float(
				Ludus::UI::Scope::Variable::ChildBorderSize,
				Ludus::Editor::Core::Constants::Welcome::RecentProjects::ListBorderSize
			);
			const auto childRounding = Ludus::UI::Scope::StyleVar::Float(
				Ludus::UI::Scope::Variable::ChildRounding,
				Ludus::Editor::Core::Constants::Welcome::RecentProjects::ListRounding
			);
			const auto childPadding = Ludus::UI::Scope::StyleVar::Vector(
				Ludus::UI::Scope::Variable::WindowPadding,
				{
					Ludus::Editor::Core::Constants::Welcome::RecentProjects::ListPadding,
					Ludus::Editor::Core::Constants::Welcome::RecentProjects::ListPadding
				}
			);
			const auto childItemSpacing = Ludus::UI::Scope::StyleVar::Vector(
				Ludus::UI::Scope::Variable::ItemSpacing,
				{
					0.0f,
					Ludus::Editor::Core::Constants::Welcome::RecentProjectRow::Spacing
				}
			);

			Ludus::UI::Scope::StyleVarScope childStyle(
				{
					childBorderSize,
					childRounding,
					childPadding,
					childItemSpacing
				}
			);

			const auto childBackgroundColor = Ludus::UI::Scope::StyleColor(
				Ludus::UI::Scope::Color::ChildBg,
				Ludus::UI::Context::ThemeContext::PanelBackground()
			);
			const auto childBorderColor = Ludus::UI::Scope::StyleColor(
				Ludus::UI::Scope::Color::Border,
				Ludus::UI::Context::ThemeContext::BorderSubtle()
			);

			Ludus::UI::Scope::StyleColorScope childColors(
				{
					childBackgroundColor,
					childBorderColor
				}
			);

			Ludus::UI::Context::WindowContext::SetCursorPosition(
				{
					(windowSize.X - recentListWidth) * 0.5f,
					Ludus::Editor::Core::Constants::Welcome::Layout::RecentListY
				}
			);

			Ludus::UI::Scope::ChildScope child(
				"WelcomeRecentProjects",
				{ recentListWidth, recentListHeight },
				Ludus::UI::Flags::Child::AlwaysUseWindowPadding | Ludus::UI::Flags::Child::Borders
			);

			if (!child)
			{
				return std::nullopt;
			}

			if (recentlyOpenedProjects.empty())
			{
				const std::string emptyState = "No recent projects yet.";

				const auto emptyStateTextColor = Ludus::UI::Scope::StyleColor(
					Ludus::UI::Scope::Color::Text,
					Ludus::UI::Context::ThemeContext::TextDisabled()
				);

				Ludus::UI::Scope::StyleColorScope emptyStateColor({ emptyStateTextColor });

				Ludus::UI::Context::WindowContext::SetCursorPosition(
					Ludus::Editor::Widgets::Layout::GetCenteredTextPositionAtY(
						emptyState,
						Ludus::Editor::Core::Constants::Welcome::RecentProjects::ListEmptyStateY,
						Ludus::UI::Context::WindowContext::GetContentRegionAvailable().X
					)
				);

				Ludus::UI::Widgets::TextUnformatted(emptyState);
				return std::nullopt;
			}

			std::optional<std::size_t> missingProjectIndexToRemove;

			for (std::size_t projectIndex = 0; projectIndex < recentlyOpenedProjects.size(); ++projectIndex)
			{
				const auto& project = recentlyOpenedProjects[projectIndex];
				const auto pathExists = std::filesystem::exists(project.Path);
				const auto projectPath = project.Path.string();

				if (Ludus::Editor::Widgets::RecentProjectRow(
					{
						.Id = project.DisplayName,
						.DisplayName = project.DisplayName,
						.Path = projectPath,
						.IsPathMissing = !pathExists
					}
				))
				{
					if (pathExists)
					{
						error.clear();
						return OpenProjectCommandSet(project.Path);
					}

					missingProjectIndexToRemove = projectIndex;
				}
			}

			if (missingProjectIndexToRemove)
			{
				const auto removedProject = recentlyOpenedProjects[*missingProjectIndexToRemove];
				std::erase_if(recentlyOpenedProjects, [&](const RecentlyOpenedProject& project)
				{
					return project.Path == removedProject.Path;
				});
				error = "Removed missing project '" + removedProject.DisplayName + "' from recent projects.";
			}

			return std::nullopt;
		}

		void DrawWelcomeFooter(const Ludus::Engine::Math::Vector2D& windowSize)
		{
			const std::string version = Ludus::Engine::Core::Version::ToString(
				Ludus::Editor::Core::Constants::CurrentVersion
			);

			const auto footerTextColor = Ludus::UI::Scope::StyleColor(
				Ludus::UI::Scope::Color::Text,
				Ludus::UI::Context::ThemeContext::TextDisabled()
			);

			Ludus::UI::Scope::StyleColorScope footerColor({ footerTextColor });

			Ludus::UI::Context::WindowContext::SetCursorPosition(
				Ludus::Editor::Widgets::Layout::GetCenteredTextPositionAtY(
					version,
					windowSize.Y - Ludus::Editor::Core::Constants::Welcome::Layout::FooterBottomOffset,
					windowSize.X
				)
			);

			Ludus::UI::Widgets::TextUnformatted(version);
		}
	}

	WelcomeWindow::WelcomeWindow(std::vector<RecentlyOpenedProject>& recentlyOpenedProjects) :
		m_RecentlyOpenedProjects(recentlyOpenedProjects)
	{}

	std::optional<Ludus::Editor::Commands::CommandSet> WelcomeWindow::Update()
	{
		const auto viewport = Ludus::UI::Context::ViewportContext::GetMainViewport();
		const auto windowSize = Ludus::Editor::Core::Constants::Welcome::Layout::WindowSize;
		const auto actionButtonSize = Ludus::Editor::Core::Constants::Welcome::Layout::ActionButtonSize;
		const auto recentListWidth = Ludus::Editor::Core::Constants::Welcome::RecentProjects::CalculateListWidth(actionButtonSize.X);
		const auto recentListHeight = Ludus::Editor::Core::Constants::Welcome::RecentProjects::CalculateListHeight();

		const Ludus::Engine::Math::Vector2D windowPosition {
			viewport.Position.X + (viewport.Size.X - windowSize.X) * 0.5f,
			viewport.Position.Y + (viewport.Size.Y - windowSize.Y) * 0.5f
		};

		// Fullscreen overlay background.
		Ludus::UI::Context::WindowContext::SetNextWindowPosition(viewport.Position);
		Ludus::UI::Context::WindowContext::SetNextWindowSize(viewport.Size);
		Ludus::UI::Context::WindowContext::SetNextWindowViewport(viewport.Id);

		const auto backgroundWindowRounding = Ludus::UI::Scope::StyleVar::Float(
			Ludus::UI::Scope::Variable::WindowRounding,
			0.0f
		);
		const auto backgroundWindowBorderSize = Ludus::UI::Scope::StyleVar::Float(
			Ludus::UI::Scope::Variable::WindowBorderSize,
			0.0f
		);
		const auto backgroundWindowPadding = Ludus::UI::Scope::StyleVar::Vector(
			Ludus::UI::Scope::Variable::WindowPadding,
			{ 0.0f, 0.0f }
		);

		Ludus::UI::Scope::StyleVarScope backgroundStyle(
			{
				backgroundWindowRounding,
				backgroundWindowBorderSize,
				backgroundWindowPadding
			}
		);

		const auto backgroundWindowColor = Ludus::UI::Scope::StyleColor(
			Ludus::UI::Scope::Color::WindowBg,
			Ludus::UI::Context::ThemeContext::OverlayBackground()
		);

		Ludus::UI::Scope::StyleColorScope backgroundColors({ backgroundWindowColor });

		Ludus::UI::Scope::WindowScope backgroundWindow(
			"WelcomeBackground##Background",
			nullptr,
			Ludus::Editor::Core::Constants::Flags::WelcomeBackgroundWindow
		);

		// Centered welcome window.
		Ludus::UI::Context::WindowContext::SetNextWindowPosition(windowPosition);
		Ludus::UI::Context::WindowContext::SetNextWindowSize(windowSize);
		Ludus::UI::Context::WindowContext::SetNextWindowViewport(viewport.Id);

		const auto welcomeWindowBorderSize = Ludus::UI::Scope::StyleVar::Float(
			Ludus::UI::Scope::Variable::WindowBorderSize,
			Ludus::Editor::Core::Constants::Welcome::Layout::WindowBorderSize
		);
		const auto welcomeWindowRounding = Ludus::UI::Scope::StyleVar::Float(
			Ludus::UI::Scope::Variable::WindowRounding,
			Ludus::Editor::Core::Constants::Welcome::Layout::WindowRounding
		);

		Ludus::UI::Scope::StyleVarScope welcomeWindowStyle(
			{
				welcomeWindowBorderSize,
				welcomeWindowRounding
			}
		);

		const auto panelBackgroundColor = Ludus::UI::Scope::StyleColor(
			Ludus::UI::Scope::Color::WindowBg,
			Ludus::UI::Context::ThemeContext::PanelBackground()
		);
		const auto panelBorderColor = Ludus::UI::Scope::StyleColor(
			Ludus::UI::Scope::Color::Border,
			Ludus::UI::Context::ThemeContext::BorderSubtle()
		);

		Ludus::UI::Scope::StyleColorScope welcomeWindowColors(
			{
				panelBackgroundColor,
				panelBorderColor
			}
		);

		Ludus::UI::Scope::WindowScope window(
			"Welcome to Ludus",
			nullptr,
			Ludus::Editor::Core::Constants::Flags::WelcomeWindow
		);

		if (!window)
		{
			return std::nullopt;
		}

		DrawWelcomeHeader(windowSize);

		if (auto commands = DrawWelcomeActionButtons(windowSize.X, actionButtonSize))
		{
			m_Error.clear();
			return commands;
		}

		{
			if (auto commands = DrawRecentProjectsSection(
				m_RecentlyOpenedProjects,
				m_Error,
				windowSize,
				recentListWidth,
				recentListHeight
			))
			{
				return commands;
			}

			// Error.
			if (!m_Error.empty())
			{
				Ludus::UI::Context::WindowContext::SetCursorPosition(
					Ludus::Editor::Widgets::Layout::GetCenteredTextPositionAtY(
						m_Error,
						windowSize.Y - Ludus::Editor::Core::Constants::Welcome::Layout::ErrorBottomOffset,
						windowSize.X
					)
				);

				Ludus::UI::Widgets::TextUnformattedColor(
					m_Error,
					Ludus::UI::Context::ThemeContext::Error()
				);
			}
		}

		DrawWelcomeFooter(windowSize);

		return std::nullopt;
	}
}
