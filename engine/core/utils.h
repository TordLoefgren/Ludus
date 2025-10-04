#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#pragma region Error Handling

using CallbackParams = std::tuple<unsigned int, unsigned int, unsigned int, unsigned int, std::string>;

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

void Write(const std::string& message);

void WriteLine(const std::string& message);
