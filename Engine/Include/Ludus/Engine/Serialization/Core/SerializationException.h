#pragma once

#include <exception>
#include <string>

namespace Ludus::Engine::Serialization::Core
{
	class SerializationException : public std::exception
	{
	private:
		std::string m_Message;

	public:
		SerializationException(char* message) : m_Message(message) { }
		SerializationException(const char* message) : m_Message(message) { }
		SerializationException(const std::string message) : m_Message(message) { }

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
