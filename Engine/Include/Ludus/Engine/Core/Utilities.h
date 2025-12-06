#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace Ludus::Engine::Core::Utilities
{
	std::vector<std::string> GetFileNames(std::string filePath);

	std::string ReadFile(std::filesystem::path path);

	template<typename T>
	void Write(const T& message)
	{
		std::cout << message;
	}

	template<typename T>
	void WriteLine(const T& message)
	{
		std::cout << message << "\n";
	}
}
