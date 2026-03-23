#pragma once

#include <filesystem>
#include <optional>

#include <Ludus/Editor/Commands/CommandSet.h>

namespace Ludus::Editor::Core
{
	class WelcomeWindow
	{
	public:
		WelcomeWindow() = default;
		~WelcomeWindow() = default;

		std::optional<Ludus::Editor::Commands::CommandSet> Update();
	};
}
