#include "pch.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <string>
#include <string_view>

#include "WinText.h"

namespace Ludus::Engine::Platform::Windows::Detail
{
	// See https://stackoverflow.com/questions/6691555/converting-narrow-string-to-wide-string
	std::wstring Utf8ToWide(std::string_view string)
	{
		if (string.empty())
		{
			return {};
		}

		auto length = MultiByteToWideChar(CP_UTF8, 0, string.data(), (int)string.size(), nullptr, 0);
		if (length <= 0)
		{
			return {};
		}

		std::wstring out((size_t)length, L'\0');
		MultiByteToWideChar(CP_UTF8, 0, string.data(), (int)string.size(), out.data(), length);

		return out;
	}

	std::string WideToUtf8(const wchar_t* wideString)
	{
		if (!wideString || *wideString == L'\0')
		{
			return {};
		}

		int length = WideCharToMultiByte(CP_UTF8, 0, wideString, -1, nullptr, 0, nullptr, nullptr);
		if (length <= 0)
		{
			return {};
		}

		// Length includes the null terminator.
		std::string out((size_t)length, '\0');
		WideCharToMultiByte(CP_UTF8, 0, wideString, -1, out.data(), length, nullptr, nullptr);

		// Remove null terminator.
		out.pop_back();

		return out;
	}
}
