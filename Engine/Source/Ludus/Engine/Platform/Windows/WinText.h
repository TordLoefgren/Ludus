#pragma once

#include <string>
#include <string_view>

namespace Ludus::Engine::Platform::Windows::Detail
{
	std::wstring Utf8ToWide(std::string_view string);
	std::string WideToUtf8(const wchar_t* wideString);
}
