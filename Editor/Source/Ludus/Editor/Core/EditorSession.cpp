#include "pch.h"

#include <optional>
#include <type_traits>
#include <utility>
#include <variant>

#include <Ludus/Editor/Core/EditorGridRenderPass.h>
#include <Ludus/Editor/Core/EditorSession.h>
#include <Ludus/Editor/Core/EditorShell.h>
#include <Ludus/Editor/Core/PendingProjectTransition.h>
#include <Ludus/Editor/Core/ProjectManifest.h>
#include <Ludus/Editor/Core/ProjectSession.h>
#include <Ludus/Editor/Persistence/Paths.h>
#include <Ludus/Editor/Persistence/ProjectSessionLoader.h>
#include <Ludus/Engine/Graphics/RenderingConfiguration2D.h>
#include <Ludus/Engine/Runtime/IHostContext.h>
#include <Ludus/Engine/Runtime/RuntimeInstanceBuilder.h>
#include <Ludus/Engine/Runtime/RuntimeManifest.h>

namespace Ludus::Editor::Core
{
	namespace
	{
		void ShutdownProjectSession(
			Ludus::Engine::Runtime::IHostContext& hostContext,
			std::optional<Ludus::Editor::Core::ProjectSession>& projectSession
		)
		{
			if (!projectSession.has_value())
			{
				return;
			}

			hostContext.DetachRuntime();
			projectSession->ShutdownRuntimes();
			projectSession = std::nullopt;
		}

		Ludus::Editor::Core::ProjectSession CreateProjectSession(
			Ludus::Engine::Runtime::IHostContext& hostContext,
			Ludus::Engine::Runtime::RuntimeEnvironment runtimeEnvironment,
			Ludus::Engine::Runtime::RuntimeManifest runtimeManifest,
			Ludus::Editor::Core::ProjectManifest projectManifest,
			Ludus::Engine::Core::Scene entryScene
		)
		{
			auto renderingConfiguration = Ludus::Engine::Graphics::RenderingConfiguration2D { };
			renderingConfiguration.AddPass(std::make_unique<Ludus::Editor::Core::EditorGridRenderPass>());

			const auto entrySceneHandle = runtimeManifest.EntrySceneHandle;

			// Build runtime instance.
			auto runtime = Ludus::Engine::Runtime::RuntimeInstanceBuilder::Create()
				.UseDefaultPhysics2D()
				.WithRenderingConfiguration(std::move(renderingConfiguration))
				.UseDefaultRendering2D()
				.UseDefaultScripting()
				.WithRuntimeManifest(std::move(runtimeManifest))
				.WithRuntimeEnvironment(std::move(runtimeEnvironment))
				.WithEntryScene(std::move(entryScene))
				.Build(hostContext);

			return Ludus::Editor::Core::ProjectSession::Create(
				std::move(projectManifest),
				std::move(runtime),
				entrySceneHandle
			);
		}
	}

	Ludus::Editor::Core::ProjectSession EditorSession::LoadProjectSession(
		Ludus::Editor::Core::ProjectManifest projectManifest
	)
	{
		auto runtimeEnvironment = Ludus::Engine::Runtime::RuntimeEnvironment
		{
			.RuntimeRootDirectory = projectManifest.ProjectRoot,
			.RuntimeManifestPath = projectManifest.RuntimeManifestPath,
			.ScriptModulePath = Ludus::Editor::Persistence::Paths::ScriptsDllFile(projectManifest.ProjectRoot)
		};

		auto loadedProject = m_ProjectSessionLoader.Load(std::move(projectManifest));

		auto projectSession = CreateProjectSession(
			m_HostContext,
			std::move(runtimeEnvironment),
			std::move(loadedProject.RuntimeManifest),
			std::move(loadedProject.ProjectManifest),
			std::move(loadedProject.EntryScene)
		);

		projectSession.GetEditorRuntime().Initialize();

		return projectSession;
	}

	EditorSession::EditorSession(
		Ludus::Engine::Runtime::IHostContext& hostContext,
		Ludus::Editor::Core::EditorShell& shell,
		Ludus::Editor::Persistence::ProjectSessionLoader& projectSessionLoader
	) :
		m_HostContext(hostContext),
		m_Shell(shell),
		m_ProjectSessionLoader(projectSessionLoader)
	{ }

	void EditorSession::ApplyTransitions(
		Ludus::Editor::Core::PendingProjectTransition& pendingTransition,
		std::optional<Ludus::Editor::Core::ProjectSession>& out
	)
	{
		std::visit([&](auto&& value)
		{
			using Alt = std::decay_t<decltype(value)>;

			if constexpr (std::is_same_v<Alt, typename Ludus::Editor::Core::PendingProjectTransition::None>)
			{
				// Do nothing.
			}
			else if constexpr (std::is_same_v<Alt, typename Ludus::Editor::Core::PendingProjectTransition::Close>)
			{
				ShutdownProjectSession(m_HostContext, out);
				m_Shell.State.Mode = Ludus::Editor::Core::EditorMode::Startup;
				pendingTransition = Ludus::Editor::Core::PendingProjectTransition::NoneState();
				m_HostContext.SetWindowTitle("Ludus Editor");
			}
			else if constexpr (std::is_same_v<Alt, typename Ludus::Editor::Core::PendingProjectTransition::Open>)
			{
				ShutdownProjectSession(m_HostContext, out);
				out = LoadProjectSession(value.ProjectManifest);
				m_HostContext.AttachRuntime(out->EditorRuntime.get());

				// Reset state.
				m_Shell.State.Mode = Ludus::Editor::Core::EditorMode::Session;
				pendingTransition = Ludus::Editor::Core::PendingProjectTransition::NoneState();
				m_HostContext.SetWindowTitle("Ludus Editor - " + out.value().ProjectManifest.ProjectRoot.string());
			}
		}, pendingTransition.Data);
	}
};
