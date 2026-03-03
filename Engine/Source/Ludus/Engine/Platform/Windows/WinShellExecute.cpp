#include "pch.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <shellapi.h>

#include "WinShellExecute.h"
#include "WinText.h"

namespace Ludus::Engine::Platform::Windows::Detail
{
	// https://learn.microsoft.com/en-us/windows/win32/api/shellapi/nf-shellapi-shellexecutea
	std::string ShellExecuteErrorMessage(INT_PTR code)
	{
		switch (code)
		{
			case 0:
				return "Out of memory or system resources.";

			case ERROR_FILE_NOT_FOUND:
				return "File not found.";

			case ERROR_BAD_FORMAT:
				return "Invalid executable format.";

			case SE_ERR_ACCESSDENIED:
				return "Access denied.";

			case SE_ERR_ASSOCINCOMPLETE:
				return "File association incomplete or invalid.";

			case SE_ERR_DDEBUSY:
				return "DDE busy.";

			case SE_ERR_DDEFAIL:
				return "DDE failure.";

			case SE_ERR_DDETIMEOUT:
				return "DDE timeout.";

			case SE_ERR_DLLNOTFOUND:
				return "Required DLL not found.";

			case SE_ERR_NOASSOC:
				return "No application associated with this file type.";

			case SE_ERR_OOM:
				return "Out of memory.";

			case SE_ERR_SHARE:
				return "Sharing violation.";

			default:
				return "Unknown ShellExecute error (" + std::to_string(code) + ")";
		}
	}

	void ShellOpen(const std::filesystem::path& application, std::string_view args)
	{
		const std::wstring paramsW = Utf8ToWide(args);

		const auto h = ShellExecuteW(
			nullptr,
			L"open",
			application.c_str(),
			args.empty() ? nullptr : paramsW.c_str(),
			nullptr,
			SW_SHOWNORMAL
		);

		const auto code = (INT_PTR)h;
		if (code <= 32)
		{
			throw std::runtime_error(
				"ShellExecute failed: " + ShellExecuteErrorMessage(code)
			);
		}
	}
}