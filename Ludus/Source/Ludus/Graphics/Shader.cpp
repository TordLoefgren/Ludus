#include <glad/glad.h>

#include <Ludus/Engine/Utilities.h>
#include <Ludus/Graphics/Shader.h>

namespace Ludus::Graphics
{
	using Ludus::Engine::Utilities::WriteLine;
	using Ludus::Engine::Utilities::ReadFile;

	Shader::Shader(std::filesystem::path path)
	{
		m_Handle = LoadShaders(path);
	}

	Shader::~Shader()
	{
		glDeleteProgram(m_Handle);
	}

	void Shader::Bind() const
	{
		if (!glIsProgram(m_Handle))
		{
			return;
		}
		glUseProgram(m_Handle);
	}

	void Shader::Unbind() const
	{
		glUseProgram(0);
	}

	void Shader::SetSamplers(int numSamplers)
	{
		Bind();

		auto maxUnits = 0;
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxUnits);
		numSamplers = std::min(numSamplers, maxUnits);

		std::vector<int> samplers(numSamplers);
		std::iota(samplers.begin(), samplers.end(), 0);

		SetUniformiv("u_Textures[0]", numSamplers, samplers.data());
	}

	void Shader::SetStrokeThickness(float thickness)
	{
		SetUniform1f("u_StrokeThickness", thickness);
	}

	unsigned int Shader::CompileShader(std::tuple<unsigned int, std::string> shaderInfo)
	{
		auto& [shaderType, shaderSource] = shaderInfo;
		auto shaderHandle = glCreateShader(shaderType);
		auto src = shaderSource.c_str();
		glShaderSource(shaderHandle, 1, &src, nullptr);
		glCompileShader(shaderHandle);

		int status;
		glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE)
		{
			WriteLine(
				"Failed to compile the " +
				std::string(shaderType == GL_VERTEX_SHADER ? "vertex shader" : "fragment shader")
			);

			int logLength;
			glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &logLength);

			std::string logMessage(logLength, '\0');
			glGetShaderInfoLog(shaderHandle, logLength, nullptr, &logMessage[0]);

			WriteLine(logMessage);
		}

		return shaderHandle;
	}

	unsigned int Shader::CompileShaders(std::vector<std::tuple<unsigned int, std::string>> shaders)
	{
		auto compiledShaders = std::vector<unsigned int>();
		auto programHandle = glCreateProgram();

		for (size_t i = 0; i < shaders.size(); i++)
		{
			auto shader = CompileShader(shaders[i]);
			glAttachShader(programHandle, shader);
			compiledShaders.push_back(shader);
		}

		glLinkProgram(programHandle);

		int status;
		glGetProgramiv(programHandle, GL_LINK_STATUS, &status);
		if (status == GL_FALSE)
		{
			WriteLine("Failed to link the program.");
		}

		for (size_t i = 0; i < shaders.size(); i++)
		{
			glDeleteShader(compiledShaders[i]);
		}

		return programHandle;
	}

	unsigned int Shader::LoadShaders(std::filesystem::path path)
	{
		auto vertexSource = ReadFile((path / "shader.vert").string().data());
		auto fragmentSource = ReadFile((path / "shader.frag").string().data());

		auto shaders = std::vector<std::tuple<unsigned int, std::string>>();
		shaders.push_back({ GL_VERTEX_SHADER, std::string(vertexSource) });
		shaders.push_back({ GL_FRAGMENT_SHADER, std::string(fragmentSource) });

		auto programHandle = CompileShaders(shaders);
		return programHandle;
	}

	void Shader::SetUniform1i(const char* name, int value)
	{
		Bind();
		glUniform1i(GetUniformLocation(name), value);
	}

	void Shader::SetUniform1f(const char* name, float value)
	{
		Bind();
		glUniform1f(GetUniformLocation(name), value);
	}

	void Shader::SetUniform4f(const char* name, float v1, float v2, float v3, float v4)
	{
		Bind();
		glUniform4f(GetUniformLocation(name), v1, v2, v3, v4);
	}

	void Shader::SetUniformMat4fv(const char* name, const glm::mat4& matrix)
	{
		Bind();
		glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void Shader::SetUniformiv(const char* name, unsigned int count, const int* values)
	{
		Bind();
		glUniform1iv(GetUniformLocation(name), count, values);
	}

	int Shader::GetUniformLocation(const char* name)
	{
		auto [it, inserted] = m_UniformCache.try_emplace(name, 0);
		if (inserted)
		{
			it->second = glGetUniformLocation(m_Handle, name);
		}

		return it->second;
	}
}
