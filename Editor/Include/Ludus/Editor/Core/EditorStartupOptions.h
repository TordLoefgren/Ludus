#pragma once

#include <filesystem>
#include <optional>

#include <Ludus/Engine/Debug/Debug.h>

namespace Ludus::Editor::Core
{
	struct EditorStartupOptions
	{
		std::optional<std::filesystem::path> StartupProjectPath;

		static EditorStartupOptions FromCommandLineArgs(int argc, char* argv[])
		{
			EditorStartupOptions options { };

			for (int i = 1; i < argc; ++i)
			{
				std::string_view arg = argv[i];

				if (arg == "--project")
				{
					if (i + 1 >= argc)
					{
						LUDUS_LOG_WARN("Missing value for --project.");
						continue;
					}

					std::filesystem::path startupProjectPath = argv[++i];

					if (std::filesystem::exists(startupProjectPath))
					{
						options.StartupProjectPath = std::move(startupProjectPath);
					}
					else
					{
						LUDUS_LOG_WARN("Startup project path does not exist: " + startupProjectPath.string());
					}
				}
			}

			return options;
		}
	};
}
