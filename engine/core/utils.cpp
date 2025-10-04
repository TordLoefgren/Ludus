#include "utils.h"
#include <glad/glad.h>

#pragma region ErrorMessageCallback

CallbackParams lastCallbackParameters;

void APIENTRY ErrorMessageCallback(
	unsigned int source,
	unsigned int type,
	unsigned int id,
	unsigned int severity,
	int length,
	const char* message,
	const void* userParam
)
{
	// Ignore non-significant error/warning codes. See https://learnopengl.com/In-Practice/Debugging.
	if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
	{
		return;
	}

	CallbackParams callbackParameters = { source, type, id, severity, std::string(message, length) };

	if (lastCallbackParameters == callbackParameters)
	{
		return;
	}

	lastCallbackParameters = std::move(callbackParameters);

	std::string output;
	output += "[OpenGL Error] ";
	output += "Source: " + std::to_string(source);
	output += ", Type: " + std::to_string(type);
	output += ", Id: " + std::to_string(id);
	output += ", Severity: " + std::to_string(severity);
	output += ", Message: " + std::string(message, length);

	WriteLine(output);
}

#pragma endregion

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
		throw std::runtime_error("Could not open file");
	}

	return { std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>() };
}

void Write(const std::string& message)
{
	std::cout << message;
}

void WriteLine(const std::string& message)
{
	std::cout << message << std::endl;
}
