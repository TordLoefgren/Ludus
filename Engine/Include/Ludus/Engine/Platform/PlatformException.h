#pragma once

#include <exception>
#include <string>

namespace Ludus::Engine::Platform
{
	class PlatformException : public std::exception
	{
	private:
		std::string m_Message;

	public:
		PlatformException(char* message) : m_Message(message) { }
		PlatformException(const char* message) : m_Message(message) { }
		PlatformException(const std::string message) : m_Message(message) { }

		const char* what()
		{
			return m_Message.c_str();
		}

		const char* what() const
		{
			return m_Message.c_str();
		}
	};
}
