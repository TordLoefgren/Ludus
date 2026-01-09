#pragma once

#include <cstdio>
#include <source_location>
#include <string>
#include <string_view>
#include <utility>   
#include <vector>


#pragma region Build-time enable

#ifndef LUDUS_DEBUG_BUILD
#if defined(_DEBUG) && !defined(NDEBUG)
#define LUDUS_DEBUG_BUILD 1
#elif defined(NDEBUG)
#define LUDUS_DEBUG_BUILD 0
#else
#define LUDUS_DEBUG_BUILD 1
#endif
#endif

#ifndef LUDUS_RELEASE_BUILD
#define LUDUS_RELEASE_BUILD (!LUDUS_DEBUG_BUILD)
#endif

#ifndef LUDUS_ENABLE_ASSERTS
#define LUDUS_ENABLE_ASSERTS LUDUS_DEBUG_BUILD
#endif

// 0 = Trace, 1 = Debug, 2 = Info, 3 = Warn, 4 = Error, 5 = Critical, 6 = Off.
#ifndef LUDUS_LOG_LEVEL
#if LUDUS_DEBUG_BUILD
#define LUDUS_LOG_LEVEL 1
#else
#define LUDUS_LOG_LEVEL 3
#endif
#endif

#pragma endregion

namespace Ludus::Engine::Debug
{

#pragma region Logging and Asserts

	enum class LogLevel : int { Trace = 0, Debug, Info, Warn, Error, Critical, Off };

	inline constexpr const char* ToString(LogLevel level) noexcept
	{
		switch (level)
		{
			case LogLevel::Trace:		return "TRACE";
			case LogLevel::Debug:		return "DEBUG";
			case LogLevel::Info:		return "INFO";
			case LogLevel::Warn:		return "WARN";
			case LogLevel::Error:		return "ERROR";
			case LogLevel::Critical:	return "CRITICAL";
			default:					return "OFF";
		}
	}

	struct LogEntry
	{
		LogLevel Level;
		std::string Message;
		std::string File;
		unsigned int Line;
		std::string Tag = "";
	};

	inline std::vector<LogEntry>& GetLogEntries()
	{
		static std::vector<LogEntry> entries;
		return entries;
	}

	inline void LogLine(
		LogLevel level,
		std::string_view message,
		std::source_location location = std::source_location::current()
	)
	{
		GetLogEntries().push_back({ level, message.data(), location.file_name(), location.line() });
	}

	inline void LogLineTagged(
		LogLevel level,
		std::string_view tag,
		std::string_view message,
		std::source_location location = std::source_location::current()
	)
	{
		GetLogEntries().push_back({ level, message.data(), location.file_name(), location.line(), tag.data() });
	}

	inline void Fail(
		std::string_view expression,
		std::string_view message,
		std::source_location location = std::source_location::current()
	)
	{
		char buffer[1024];
		const char* logMessage = message.empty() ? "assertion failed" : message.data();

		std::snprintf(
			buffer,
			sizeof(buffer),
			"%s | expr: %.*s",
			logMessage,
			static_cast<int>(expression.size()),
			expression.data()
		);

		// Make sure that failure is also recorded with stderr. This is essential for testing failure behaviour.
		std::fputs(buffer, stderr);
		std::fputc('\n', stderr);
		std::fflush(stderr);

		LogLine(LogLevel::Critical, buffer, location);

		std::terminate();
	}

#pragma endregion

}

#pragma region Filtering

#if LUDUS_LOG_LEVEL <= 0
#define LUDUS_LOG_TRACE(message) Ludus::Engine::Debug::LogLine(Ludus::Engine::Debug::LogLevel::Trace, (message))
#else
#define LUDUS_LOG_TRACE(message) ((void)0)
#endif

#if LUDUS_LOG_LEVEL <= 1
#define LUDUS_LOG_DEBUG(message) Ludus::Engine::Debug::LogLine(Ludus::Engine::Debug::LogLevel::Debug, (message))
#else
#define LUDUS_LOG_DEBUG(message) ((void)0)
#endif

#if LUDUS_LOG_LEVEL <= 2
#define LUDUS_LOG_INFO(message)  Ludus::Engine::Debug::LogLine(Ludus::Engine::Debug::LogLevel::Info,  (message))
#else
#define LUDUS_LOG_INFO(message)  ((void)0)
#endif

#if LUDUS_LOG_LEVEL <= 3
#define LUDUS_LOG_WARN(message)  Ludus::Engine::Debug::LogLine(Ludus::Engine::Debug::LogLevel::Warn,  (message))
#else
#define LUDUS_LOG_WARN(message)  ((void)0)
#endif

#if LUDUS_LOG_LEVEL <= 4
#define LUDUS_LOG_ERROR(message) Ludus::Engine::Debug::LogLine(Ludus::Engine::Debug::LogLevel::Error, (message))
#else
#define LUDUS_LOG_ERROR(message) ((void)0)
#endif

#if LUDUS_LOG_LEVEL <= 5
#define LUDUS_LOG_CRITICAL(message)  Ludus::Engine::Debug::LogLine(Ludus::Engine::Debug::LogLevel::Critical, (message))
#else
#define LUDUS_LOG_CRITICAL(message)  ((void)0)
#endif

#if LUDUS_ENABLE_ASSERTS
#define LUDUS_ASSERT(expression, message) \
      do { if(!(expression)) Ludus::Engine::Debug::Fail(#expression, (message)); } while(0)
#define LUDUS_VERIFY(expression, message) \
      do { if(!(expression)) Ludus::Engine::Debug::Fail(#expression, (message)); } while(0)
#else
#define LUDUS_ASSERT(expression, message) ((void)0)
#define LUDUS_VERIFY(expression, message) ((void)(expression))
#endif

#pragma endregion
