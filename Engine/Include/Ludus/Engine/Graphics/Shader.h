#pragma once

#include <filesystem>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <numeric>
#include <unordered_map>

#include <Ludus/Engine/Core/Utilities.h>
#include <Ludus/Engine/Graphics/Texture.h>

namespace Ludus::Engine::Graphics
{
	class Shader
	{
	private:
		unsigned int m_Handle;
		std::unordered_map<std::string, int> m_UniformCache;

		unsigned int CompileShader(std::tuple<unsigned int, std::string> shaderInfo);
		unsigned int CompileShaders(std::vector<std::tuple<unsigned int, std::string>> shaders);
		unsigned int LoadShaders(std::filesystem::path path);

	public:
		Shader(std::filesystem::path path = "Resources/Shaders");
		~Shader();

		void Bind() const;
		void Unbind() const;

		void SetUniform1i(const char* name, int value);
		void SetUniform1f(const char* name, float value);
		void SetUniform4f(const char* name, float v1, float v2, float v3, float v4);
		void SetUniformMat4fv(const char* name, const glm::mat4& matrix);
		void SetUniformiv(const char* name, unsigned int count, const int* values);

		void SetSamplers(int numSamplers);
		void SetStrokeThickness(float thickness);

		int GetUniformLocation(const char* name);
	};
}