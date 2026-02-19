#pragma once

#include <string>
#include <tuple>

#include <Ludus/Engine/Debug/Debug.h>
#include <Ludus/Engine/Graphics/GL.h>

namespace Ludus::Engine::Debug
{

#if !defined(NDEBUG)

	inline constexpr const char* GLSourceName(GLenum source) noexcept
	{
		switch (source)
		{
			case GL_DEBUG_SOURCE_API:             return "API";
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   return "WINDOW";
			case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER";
			case GL_DEBUG_SOURCE_THIRD_PARTY:     return "3RD";
			case GL_DEBUG_SOURCE_APPLICATION:     return "APP";
			case GL_DEBUG_SOURCE_OTHER:           return "OTHER";
			default:                              return "UNKNOWN";
		}
	}

	inline constexpr const char* GLTypeName(GLenum type) noexcept
	{
		switch (type)
		{
			case GL_DEBUG_TYPE_ERROR:               return "ERROR";
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED";
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:  return "UNDEFINED";
			case GL_DEBUG_TYPE_PORTABILITY:         return "PORTABILITY";
			case GL_DEBUG_TYPE_PERFORMANCE:         return "PERF";
			case GL_DEBUG_TYPE_MARKER:              return "MARKER";
			case GL_DEBUG_TYPE_PUSH_GROUP:          return "PUSH";
			case GL_DEBUG_TYPE_POP_GROUP:           return "POP";
			case GL_DEBUG_TYPE_OTHER:               return "OTHER";
			default:                                return "UNKNOWN";
		}
	}

	inline constexpr LogLevel GLSeverityToLevel(GLenum sev) noexcept
	{
		switch (sev)
		{
			case GL_DEBUG_SEVERITY_HIGH:         return LogLevel::Critical;
			case GL_DEBUG_SEVERITY_MEDIUM:       return LogLevel::Error;
			case GL_DEBUG_SEVERITY_LOW:          return LogLevel::Warn;
			case GL_DEBUG_SEVERITY_NOTIFICATION: return LogLevel::Debug;
			default:                             return LogLevel::Info;
		}
	}

	using CallbackParams = std::tuple<GLenum, GLenum, GLuint, GLenum, std::string>;

	inline CallbackParams g_LastGLCallbackParams {};

	inline void APIENTRY ErrorMessageCallback(
		GLenum source,
		GLenum type,
		GLuint id,
		GLenum severity,
		GLsizei length,
		const GLchar* message,
		const void* userParams
	)
	{
		// Ignore non-significant spam (from LearnOpenGL).
		if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
			return;

		CallbackParams params {
			source,
			type,
			id,
			severity,
			std::string(message, static_cast<size_t>(length))
		};

		if (g_LastGLCallbackParams == params)
		{
			return;
		}

		g_LastGLCallbackParams = std::move(params);

		char buffer[1024];

		std::snprintf(
			buffer,
			sizeof(buffer),
			"Source=%s, Type=%s, Id=%u, Severity=%s, Message=%.*s",
			GLSourceName(source),
			GLTypeName(type),
			id,
			ToString(GLSeverityToLevel(severity)),
			static_cast<int>(length),
			message
		);

		LogLineTagged(GLSeverityToLevel(severity), "OpenGL", buffer);
	}

#endif

	inline void EnableOpenGLDebug()
	{

#if !defined(NDEBUG)

		if (glDebugMessageCallback == nullptr)
		{
			return;
		}

		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

		glDebugMessageCallback(ErrorMessageCallback, nullptr);

#endif
	}
}
