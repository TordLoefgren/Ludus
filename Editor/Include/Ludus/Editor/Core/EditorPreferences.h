#pragma once

#include <algorithm>
#include <filesystem>
#include <iterator>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <Ludus/Editor/Core/RecentlyOpenedProject.h>
#include <Ludus/UI/Theme/ThemeId.h>

namespace Ludus::Editor::Core
{
	inline constexpr size_t MaxRecentlyOpenedProjects = 10;

	struct EditorPreferences
	{
		Ludus::UI::Theme::ThemeId ActiveThemeId = Ludus::UI::Theme::ThemeId::LudusDark;
		std::vector<RecentlyOpenedProject> RecentlyOpenedProjects;

		void AddRecentlyOpenedProject(std::string_view name, const std::filesystem::path& path)
		{
			std::erase_if(RecentlyOpenedProjects, [&](const RecentlyOpenedProject& project)
			{
				return project.Path == path;
			});

			if (RecentlyOpenedProjects.size() == MaxRecentlyOpenedProjects)
			{
				RecentlyOpenedProjects.pop_back();
			}

			RecentlyOpenedProjects.insert(RecentlyOpenedProjects.begin(), { std::string(name), path });
		}

		static EditorPreferences Default()
		{
			EditorPreferences editorPreferences;
			return editorPreferences;
		}
	};
}
