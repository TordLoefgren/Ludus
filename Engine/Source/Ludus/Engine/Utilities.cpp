#include <Ludus/Engine/Utilities.h>

namespace Ludus::Engine::Utilities
{
	std::vector<std::string> GetFileNames(std::string path)
	{
		auto fileNames = std::vector<std::string>();
		for (auto& p : std::filesystem::recursive_directory_iterator(path))
		{
			fileNames.push_back(p.path().string());
		}

		return fileNames;
	}

	std::string ReadFile(std::filesystem::path path)
	{
		std::ifstream file(path);
		if (!file)
		{
			throw std::runtime_error("Could not open file.");
		}

		return { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
	}
}
