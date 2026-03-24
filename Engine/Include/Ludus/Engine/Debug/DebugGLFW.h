#pragma once

#include <stdexcept>
#include <string>

#include <GLFW/glfw3.h>

#include <Ludus/Engine/Debug/Debug.h>

namespace Ludus::Engine::Debug::DebugGLFW
{

#if !defined(NDEBUG)

	inline constexpr const char* GLFWErrorCode(int code)
	{
		switch (code)
		{
			case GLFW_NO_ERROR:             return "NO_ERROR";
			case GLFW_NO_CURRENT_CONTEXT:   return "NO_CURRENT_CONTEXT";
			case GLFW_INVALID_ENUM:			return "INVALID_ENUM";
			case GLFW_INVALID_VALUE:		return "INVALID_VALUE";
			case GLFW_OUT_OF_MEMORY:		return "OUT_OF_MEMORY";
			case GLFW_API_UNAVAILABLE:      return "API_UNAVAILABLE";
			case GLFW_VERSION_UNAVAILABLE:  return "VERSION_UNAVAILABLE";
			case GLFW_PLATFORM_ERROR:       return "PLATFORM_ERROR";
			case GLFW_FORMAT_UNAVAILABLE:   return "FORMAT_UNAVAILABLE";
			case GLFW_NO_WINDOW_CONTEXT:    return "NO_WINDOW_CONTEXT";
			default:                        throw std::runtime_error("Unexpected enum.");
		}
	}

	inline constexpr LogLevel GLFWErrorCodeToLevel(int code) noexcept
	{
		switch (code)
		{
			case GLFW_NO_ERROR:             return LogLevel::Info;
			case GLFW_NO_CURRENT_CONTEXT:   return LogLevel::Critical;
			case GLFW_INVALID_ENUM:			return LogLevel::Error;
			case GLFW_INVALID_VALUE:		return LogLevel::Error;
			case GLFW_OUT_OF_MEMORY:		return LogLevel::Critical;
			case GLFW_API_UNAVAILABLE:      return LogLevel::Error;
			case GLFW_VERSION_UNAVAILABLE:  return LogLevel::Error;
			case GLFW_PLATFORM_ERROR:       return LogLevel::Error;
			case GLFW_FORMAT_UNAVAILABLE:   return LogLevel::Error;
			case GLFW_NO_WINDOW_CONTEXT:    return LogLevel::Critical;
			default:                        return LogLevel::Info;
		}
	}

	inline void ErrorMessageCallback(int code, const char* description)
	{
		char buffer[1024];

		std::snprintf(
			buffer,
			sizeof(buffer),
			GLFWErrorCode(code),
			description
		);

		LogLineTagged(GLFWErrorCodeToLevel(code), "GLFW", buffer);
	}
#endif
}
