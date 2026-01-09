#pragma once

#include <string>

namespace Ludus::Engine::Persistance
{
	class ISaveArchive
	{
	public:
		virtual ~ISaveArchive() = default;

		virtual void BeginObject(std::string_view key) = 0;
		virtual void EndObject() = 0;

		virtual void BeginArray(std::string_view key) = 0;
		virtual void EndArray() = 0;

		virtual void BeginArrayElement() = 0;
		virtual void EndArrayElement() = 0;

		virtual void WriteBool(std::string_view key, bool value) = 0;
		virtual void WriteDouble(std::string_view key, double value) = 0;
		virtual void WriteFloat(std::string_view key, float value) = 0;
		virtual void WriteInt(std::string_view key, int value) = 0;
		virtual void WriteString(std::string_view key, std::string_view value) = 0;
	};
}
