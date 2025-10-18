#pragma once

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace Ludus::Engine::Utilities
{
#pragma region Error Handling

	void __stdcall ErrorMessageCallback(
		unsigned int source,
		unsigned int type,
		unsigned int id,
		unsigned int severity,
		int length,
		const char* message,
		const void* userParam
	);
#pragma endregion

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
