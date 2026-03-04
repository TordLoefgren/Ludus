#include "pch.h"

#include <Ludus/Editor/Core/Constants.h>
#include <Ludus/Editor/Core/EditorMode.h>
#include <Ludus/Editor/Panels/WelcomePanel.h>
#include <Ludus/Editor/Persistence/Paths.h>
#include <Ludus/Engine/Graphics/Texture.h>
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

namespace Ludus::Editor::Panels
{
	bool WelcomePanel::UpdateImpl(Ludus::Editor::Panels::PanelContext& context)
	{
		const auto viewport = Ludus::UI::Context::ViewportContext::GetMainViewport();
		Ludus::UI::Context::WindowContext::SetNextWindowPosition(viewport.WorkPosition);
		Ludus::UI::Context::WindowContext::SetNextWindowSize(viewport.WorkSize);
		Ludus::UI::Context::WindowContext::SetNextWindowViewport(viewport.Id);

		Ludus::UI::Scope::StyleVarScope styleVar(
			{
				Ludus::UI::Scope::StyleVar::Float(Ludus::UI::Scope::Variable::WindowRounding, 0.0f),
				Ludus::UI::Scope::StyleVar::Float(Ludus::UI::Scope::Variable::WindowBorderSize, 0.0f),
				Ludus::UI::Scope::StyleVar::Vector(Ludus::UI::Scope::Variable::WindowPadding, { 0.0f, 0.0f })
			}
		);

		Ludus::UI::Scope::StyleColorScope colorScope({
			{ Ludus::UI::Scope::Color::WindowBg, Ludus::Engine::Graphics::Colors::Black }
			});

		// The welcome panel should never close, as it serves as the entry point to the editor.
		auto windowTitle = CreateWindowTitle("##Welcome_Panel");
		if (Ludus::UI::Scope::WindowScope window(windowTitle.c_str(), nullptr, Ludus::Editor::Core::Constants::WelcomePanelWindowFlags); window)
		{
			const auto availableSpace = Ludus::UI::Context::WindowContext::GetContentRegionAvailable();

			{
				const auto rectSize = availableSpace * 0.5f;
				const auto rectPosition = (availableSpace - rectSize) * 0.5f;
				Ludus::UI::Context::LayoutContext::DrawRect(
					rectPosition,
					rectSize,
					Ludus::Engine::Graphics::Colors::DarkGray
				);
			}

			{
				Ludus::UI::Scope::FontScope font(80.0f);

				const auto title = "LUDUS";

				const auto itemSize = Ludus::UI::Context::LayoutContext::CalculateTextSize(title);
				auto position = (availableSpace - itemSize) * 0.5f;
				position.Y -= 150.0f;

				Ludus::UI::Context::WindowContext::SetCursorPosition(position);

				Ludus::UI::Widgets::TextUnformatted(title);
			}

			{
				Ludus::Engine::Math::Vector2D buttonSize(100.0f, 50.0f);
				const auto position = (availableSpace - buttonSize) * 0.5f;

				Ludus::UI::Context::WindowContext::SetCursorPosition(position);

				if (Ludus::UI::Widgets::Button("Create New", buttonSize))
				{
					context.EditorContext.State.Commands.AddUICommand(Ludus::Editor::Commands::UICommand::OpenCreateProjectDialog { });
				}

				Ludus::UI::Context::WindowContext::SetCursorPosition({ position.X, position.Y + 75.0f });

				if (Ludus::UI::Widgets::Button("Open", buttonSize))
				{
					std::string path;
					if (Ludus::Engine::Platform::Modals::OpenFileDialog(
						path,
						"ludus.app",
						Ludus::Editor::Persistence::Paths::ProjectsRoot()
					))
					{
						context.EditorContext.State.Commands.AddRequestCommand(Ludus::Editor::Commands::RequestCommand::OpenProject { path });
					}
				}
			}
		}

		return true;
	}
}
