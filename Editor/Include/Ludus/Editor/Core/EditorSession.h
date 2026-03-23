#pragma once

#include <optional>

#include <Ludus/Editor/Core/EditorShell.h>
#include <Ludus/Editor/Core/PendingProjectTransition.h>
#include <Ludus/Editor/Core/ProjectSession.h>
#include <Ludus/Editor/Persistence/ProjectSessionLoader.h>
#include <Ludus/Engine/Runtime/IHostContext.h>

namespace Ludus::Editor::Core
{
	class EditorSession
	{
	private:
		Ludus::Engine::Runtime::IHostContext& m_HostContext;
		Ludus::Editor::Core::EditorShell& m_Shell;
		Ludus::Editor::Persistence::ProjectSessionLoader& m_ProjectSessionLoader;

		Ludus::Editor::Core::ProjectSession LoadProjectSession(Ludus::Editor::Core::ProjectManifest projectManifest);

	public:
		EditorSession(
			Ludus::Engine::Runtime::IHostContext& hostContext,
			Ludus::Editor::Core::EditorShell& shell,
			Ludus::Editor::Persistence::ProjectSessionLoader& projectSessionLoader
		);

		void ApplyTransitions(
			Ludus::Editor::Core::PendingProjectTransition& pendingTransition,
			std::optional<Ludus::Editor::Core::ProjectSession>& out
		);
	};
}
