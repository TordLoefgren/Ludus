#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

#include <Ludus/Engine/Persistance/Archives/IArchiveDiagnostics.h>

namespace Ludus::Engine::Persistance::Archives
{
	class ILoadArchive
	{
	public:
		virtual ~ILoadArchive() = default;

		virtual std::string GetCurrentPath(std::string_view currentKey) const = 0;

		virtual bool TryBeginObject(std::string_view key) = 0;
		virtual void BeginObject(std::string_view key) = 0;
		virtual void EndObject() = 0;

		virtual bool TryBeginArray(std::string_view key, size_t& outCount) = 0;
		virtual size_t BeginArray(std::string_view key) = 0;
		virtual void EndArray() = 0;

		virtual void BeginArrayElement(size_t index) = 0;
		virtual void EndArrayElement() = 0;

		virtual bool Has(std::string_view key) const = 0;

		virtual void ReadBool(std::string_view key, bool& outValue) const = 0;
		virtual void ReadDouble(std::string_view key, double& outValue) const = 0;
		virtual void ReadFloat(std::string_view key, float& outValue) const = 0;
		virtual void ReadInt(std::string_view key, int& outValue) const = 0;
		virtual void ReadString(std::string_view key, std::string& outValue) const = 0;
		virtual void ReadUint8(std::string_view key, uint8_t& outValue) const = 0;
		virtual void ReadUint32(std::string_view key, uint32_t& outValue) const = 0;

		virtual bool TryReadBool(std::string_view key, bool& outValue) const = 0;
		virtual bool TryReadDouble(std::string_view key, double& outValue) const = 0;
		virtual bool TryReadFloat(std::string_view key, float& outValue) const = 0;
		virtual bool TryReadInt(std::string_view key, int& outValue) const = 0;
		virtual bool TryReadString(std::string_view key, std::string& outValue) const = 0;
		virtual bool TryReadUint8(std::string_view key, uint8_t& outValue) const = 0;
		virtual bool TryReadUint32(std::string_view key, uint32_t& outValue) const = 0;

		void ReadBoolOrDefault(std::string_view key, bool& outValue, bool defaultValue)
		{
			if (!TryReadBool(key, outValue))
			{
				outValue = defaultValue;
			}
		}

		void ReadDoubleOrDefault(std::string_view key, double& outValue, double defaultValue)
		{
			if (!TryReadDouble(key, outValue))
			{
				outValue = defaultValue;
			}
		}

		void ReadFloatOrDefault(std::string_view key, float& outValue, float defaultValue)
		{
			if (!TryReadFloat(key, outValue))
			{
				outValue = defaultValue;
			}
		}

		void ReadIntOrDefault(std::string_view key, int& outValue, int defaultValue)
		{
			if (!TryReadInt(key, outValue))
			{
				outValue = defaultValue;
			}
		}

		void ReadStringOrDefault(std::string_view key, std::string& outValue, const std::string& defaultValue)
		{
			if (!TryReadString(key, outValue))
			{
				outValue = defaultValue;
			}
		}

		void ReadUint8OrDefault(std::string_view key, uint8_t& outValue, uint8_t defaultValue)
		{
			if (!TryReadUint8(key, outValue))
			{
				outValue = defaultValue;
			}
		}

		void ReadUint32OrDefault(std::string_view key, uint32_t& outValue, uint32_t defaultValue)
		{
			if (!TryReadUint32(key, outValue))
			{
				outValue = defaultValue;
			}
		}

		IArchiveDiagnostics* GetDiagnostics() { return m_Diagnostics; }

	protected:
		IArchiveDiagnostics* m_Diagnostics = nullptr;
	};
}
