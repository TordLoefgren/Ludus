#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

#include <Ludus/Editor/Commands/CommandSet.h>
#include <Ludus/Editor/Core/RecentlyOpenedProject.h>

namespace Ludus::Editor::Core
{
	class WelcomeWindow
	{
	private:
		std::vector<RecentlyOpenedProject>& m_RecentlyOpenedProjects;
		std::string m_Error;

	public:
		WelcomeWindow(
			std::vector<RecentlyOpenedProject>& recentlyOpenedProjects
		);
		~WelcomeWindow() = default;

		std::optional<Ludus::Editor::Commands::CommandSet> Update();
	};
}
