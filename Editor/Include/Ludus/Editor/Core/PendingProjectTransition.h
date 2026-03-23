#pragma once

#include <utility>
#include <variant>

#include <Ludus/Editor/Core/ProjectManifest.h>

namespace Ludus::Editor::Core
{
	struct PendingProjectTransition
	{
		struct Close { };
		struct None { };
		struct Open { Ludus::Editor::Core::ProjectManifest ProjectManifest; };

		using Variant = std::variant<None, Open, Close>;

		Variant Data;

		static PendingProjectTransition CloseProject() { return PendingProjectTransition { Close { } }; }
		static PendingProjectTransition NoneState() { return PendingProjectTransition { None { } }; }
		static PendingProjectTransition OpenProject(Ludus::Editor::Core::ProjectManifest projectManifest) { return PendingProjectTransition { Open { std::move(projectManifest) } }; }
	};
}
