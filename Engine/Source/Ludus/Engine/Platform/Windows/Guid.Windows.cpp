#include "pch.h"

#include <array>
#include <compare>
#include <cstdint>
#include <cstring>
#include <functional>
#include <iomanip>
#include <optional>
#include <ostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <combaseapi.h>

#include <Ludus/Engine/Platform/Guid.h>

namespace Ludus::Engine::Platform
{
	namespace
	{
		std::optional<std::uint8_t> ParseHexChar(char c)
		{
			if (c >= '0' && c <= '9')
			{
				return static_cast<std::uint8_t>(c - '0');
			}

			if (c >= 'a' && c <= 'f')
			{
				return static_cast<std::uint8_t>(10 + (c - 'a'));
			}

			if (c >= 'A' && c <= 'F')
			{
				return static_cast<std::uint8_t>(10 + (c - 'A'));
			}

			return std::nullopt;
		}

		std::optional<std::uint8_t> ParseByte(char high, char low)
		{
			const auto highNibble = ParseHexChar(high);
			const auto lowNibble = ParseHexChar(low);
			if (!highNibble || !lowNibble)
			{
				return std::nullopt;
			}

			return static_cast<std::uint8_t>((*highNibble << 4) | *lowNibble);
		}
	}

	Guid CreateGuid()
	{
		GUID guid;
		auto hr = CoCreateGuid(&guid);
		if (FAILED(hr))
		{
			throw std::runtime_error("Failed to generate GUID.");
		}

		static_assert(sizeof(::GUID) == 16);
		static_assert(sizeof(((::GUID*)nullptr)->Data1) == 4);
		static_assert(sizeof(((::GUID*)nullptr)->Data2) == 2);
		static_assert(sizeof(((::GUID*)nullptr)->Data3) == 2);
		static_assert(sizeof(((::GUID*)nullptr)->Data4) == 8);

		Guid result { };

		size_t offset = 0;
		std::memcpy(&result.Bytes[offset], &guid.Data1, sizeof(guid.Data1));
		offset += sizeof(guid.Data1);

		std::memcpy(&result.Bytes[offset], &guid.Data2, sizeof(guid.Data2));
		offset += sizeof(guid.Data2);

		std::memcpy(&result.Bytes[offset], &guid.Data3, sizeof(guid.Data3));
		offset += sizeof(guid.Data3);

		std::memcpy(&result.Bytes[offset], &guid.Data4, sizeof(guid.Data4));

		return result;
	}

	std::optional<Guid> TryParse(std::string_view text)
	{
		if (text.size() != 36)
		{
			return std::nullopt;
		}

		if (text[8] != '-' || text[13] != '-' || text[18] != '-' || text[23] != '-')
		{
			return std::nullopt;
		}

		Guid result { };

		const auto b0 = ParseByte(text[6], text[7]);
		const auto b1 = ParseByte(text[4], text[5]);
		const auto b2 = ParseByte(text[2], text[3]);
		const auto b3 = ParseByte(text[0], text[1]);

		const auto b4 = ParseByte(text[11], text[12]);
		const auto b5 = ParseByte(text[9], text[10]);

		const auto b6 = ParseByte(text[16], text[17]);
		const auto b7 = ParseByte(text[14], text[15]);

		const auto b8 = ParseByte(text[19], text[20]);
		const auto b9 = ParseByte(text[21], text[22]);

		const auto b10 = ParseByte(text[24], text[25]);
		const auto b11 = ParseByte(text[26], text[27]);
		const auto b12 = ParseByte(text[28], text[29]);
		const auto b13 = ParseByte(text[30], text[31]);
		const auto b14 = ParseByte(text[32], text[33]);
		const auto b15 = ParseByte(text[34], text[35]);

		if (!b0 || !b1 || !b2 || !b3 || !b4 || !b5 || !b6 || !b7 ||
			!b8 || !b9 || !b10 || !b11 || !b12 || !b13 || !b14 || !b15)
		{
			return std::nullopt;
		}

		result.Bytes[0] = *b0;
		result.Bytes[1] = *b1;
		result.Bytes[2] = *b2;
		result.Bytes[3] = *b3;
		result.Bytes[4] = *b4;
		result.Bytes[5] = *b5;
		result.Bytes[6] = *b6;
		result.Bytes[7] = *b7;
		result.Bytes[8] = *b8;
		result.Bytes[9] = *b9;
		result.Bytes[10] = *b10;
		result.Bytes[11] = *b11;
		result.Bytes[12] = *b12;
		result.Bytes[13] = *b13;
		result.Bytes[14] = *b14;
		result.Bytes[15] = *b15;

		return result;
	}

	bool Guid::IsEmpty() const
	{
		return Bytes == std::array<std::uint8_t, 16> {};
	}

	std::string Guid::ToString() const
	{
		// See https://devblogs.microsoft.com/oldnewthing/20220928-00/?p=107221 for GUID format and endianness.

		std::ostringstream os;
		os << std::hex << std::setfill('0') << std::nouppercase;

		os << std::setw(2) << static_cast<int>(Bytes[3]);
		os << std::setw(2) << static_cast<int>(Bytes[2]);
		os << std::setw(2) << static_cast<int>(Bytes[1]);
		os << std::setw(2) << static_cast<int>(Bytes[0]);
		os << '-';

		os << std::setw(2) << static_cast<int>(Bytes[5]);
		os << std::setw(2) << static_cast<int>(Bytes[4]);
		os << '-';

		os << std::setw(2) << static_cast<int>(Bytes[7]);
		os << std::setw(2) << static_cast<int>(Bytes[6]);
		os << '-';

		os << std::setw(2) << static_cast<int>(Bytes[8]);
		os << std::setw(2) << static_cast<int>(Bytes[9]);
		os << '-';

		os << std::setw(2) << static_cast<int>(Bytes[10]);
		os << std::setw(2) << static_cast<int>(Bytes[11]);
		os << std::setw(2) << static_cast<int>(Bytes[12]);
		os << std::setw(2) << static_cast<int>(Bytes[13]);
		os << std::setw(2) << static_cast<int>(Bytes[14]);
		os << std::setw(2) << static_cast<int>(Bytes[15]);

		return os.str();
	}
}
