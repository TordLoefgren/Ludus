#pragma once

#include <optional>

#include <Ludus/Editor/Commands/ProjectSessionCommandContext.h>
#include <Ludus/Editor/Commands/StartupCommandContext.h>
#include <Ludus/Editor/Core/EditorConfiguration.h>
#include <Ludus/Editor/Core/EditorSession.h>
#include <Ludus/Editor/Core/EditorShell.h>
#include <Ludus/Editor/Core/EditorStartupOptions.h>
#include <Ludus/Editor/Core/ProjectSession.h>
#include <Ludus/Editor/Core/WelcomeWindow.h>
#include <Ludus/Editor/Panels/PanelRegistry.h>
#include <Ludus/Editor/Persistence/LmlProjectManifestPersistence.h>
#include <Ludus/Editor/Persistence/ProjectSessionLoader.h>
#include <Ludus/Engine/Events/EventHandler.h>
#include <Ludus/Engine/Persistence/LmlRuntimeLaunchSettingsPersistence.h>
#include <Ludus/Engine/Persistence/LmlRuntimeManifestPersistence.h>
#include <Ludus/Engine/Persistence/LmlScenePersistence.h>
#include <Ludus/Engine/Runtime/IHostContext.h>
#include <Ludus/Engine/Runtime/ISystem.h>

namespace Ludus::Editor::Core
{
	class EditorSystem final : public Ludus::Engine::Runtime::ISystem, public Ludus::Engine::Events::EventHandler
	{
	private:
		EditorShell m_Shell;
		std::optional<ProjectSession> m_ProjectSession;
		Ludus::Engine::Runtime::IHostContext& m_HostContext;
		Ludus::Engine::Persistence::LmlScenePersistence m_ScenePersistence;
		Ludus::Engine::Persistence::LmlRuntimeManifestPersistence m_RuntimeManifestPersistence;
		Ludus::Engine::Persistence::LmlRuntimeLaunchSettingsPersistence m_RuntimeLaunchSettingsPersistence;
		Ludus::Editor::Persistence::LmlProjectManifestPersistence m_ProjectManifestPersistence;
		Ludus::Editor::Persistence::ProjectSessionLoader m_ProjectSessionLoader;
		Ludus::Editor::Core::EditorSession m_Session;
		Ludus::Editor::Panels::PanelRegistry m_PanelRegistry;
		WelcomeWindow m_WelcomeWindow;

		EditorConfiguration m_EditorConfiguration;
		EditorStartupOptions m_EditorStartupOptions;

		Ludus::Editor::Commands::StartupCommandContext CreateStartupCommandContext();
		Ludus::Editor::Commands::ProjectSessionCommandContext CreateProjectSessionCommandContext();

		void DelegateUICommands();
		void DelegateEditCommands();
		void DelegateRequestCommands();

		void ApplyStartupOptions();
		void RegisterPanels();

		void FlushCommands();
		void UpdateDialogs();
		void UpdatePanels();
		void UpdateWindowTitle();

		void UpdateStartup();
		void UpdateProjectSession();

	public:
		EditorSystem(
			Ludus::Engine::Runtime::IHostContext& hostContext,
			EditorConfiguration editorConfiguration,
			EditorStartupOptions editorStartupOptions
		);
		~EditorSystem() = default;

		virtual void OnAttachImpl() override;
		virtual void OnDetachImpl() override;

		virtual void UpdateImpl(float deltaTime) override;

		virtual bool ProcessEvent(const Ludus::Engine::Events::Event& event) override;
	};
}
