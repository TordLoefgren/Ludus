#pragma once

#include <cstddef>
#include <cstdint>
#include <string_view>

#include <Ludus/Engine/Persistance/Archives/IArchiveDiagnostics.h>

namespace Ludus::Engine::Persistance::Archives
{
	class ISaveArchive
	{
	public:
		virtual ~ISaveArchive() = default;

		virtual void BeginObject(std::string_view key) = 0;
		virtual void EndObject() = 0;

		virtual void BeginArray(std::string_view key) = 0;
		virtual void EndArray() = 0;

		virtual void BeginArrayElement(size_t index) = 0;
		virtual void BeginArrayElement() = 0;
		virtual void EndArrayElement() = 0;

		virtual void WriteBool(std::string_view key, bool value) = 0;
		virtual void WriteDouble(std::string_view key, double value) = 0;
		virtual void WriteFloat(std::string_view key, float value) = 0;
		virtual void WriteInt(std::string_view key, int value) = 0;
		virtual void WriteString(std::string_view key, std::string_view value) = 0;
		virtual void WriteUint8(std::string_view key, uint8_t value) = 0;
		virtual void WriteUint32(std::string_view key, uint32_t value) = 0;

	protected:
		IArchiveDiagnostics* m_Diagnostics = nullptr;
	};
}
