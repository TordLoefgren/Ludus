#pragma once

#include <exception>
#include <string>

namespace Ludus::Engine::Persistance
{
	class PersistanceException : public std::exception
	{
	private:
		std::string m_Message;

	public:
		PersistanceException(char* message) : m_Message(message) { }
		PersistanceException(const char* message) : m_Message(message) { }
		PersistanceException(const std::string message) : m_Message(message) { }

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
