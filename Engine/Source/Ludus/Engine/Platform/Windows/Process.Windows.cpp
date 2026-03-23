#include "pch.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

#include <string>
#include <string_view>
#include <utility>

#include <Ludus/Engine/Platform/Process.h>
#include <Ludus/Engine/Platform/PlatformException.h>
#include <Ludus/Engine/Core/Expected.h>

#include "WinText.h"

namespace Ludus::Engine::Platform::Process
{
	constexpr DWORD BufferSize = 4096;

	static Ludus::Engine::Core::Unexpected<PlatformException> GetError(
		const std::string_view functionName
	)
	{
		const auto code = GetLastError();
		const auto exception = PlatformException(
			std::string(functionName) + " failed with error code: " + std::to_string(code)
		);

		return Ludus::Engine::Core::Unexpected<PlatformException>::Create(exception);
	}

	/// <summary>
	/// Runs a process and blocks until it exits. Captures combined outputs.
	/// </summary>
	/// <param name="application">The application to execute.</param>
	/// <param name="args">The application arguments.</param>
	/// <returns>
	/// Expected containing ProcessResult (exit code & output),
	/// or PlatformException if an OS-level call fails.
	/// </returns>
	Ludus::Engine::Core::Expected<ProcessResult, PlatformException> Run(
		const std::filesystem::path& application,
		std::string_view args
	)
	{
		// Allow for the child processes to inherit from returned handles.
		SECURITY_ATTRIBUTES sa { };
		sa.nLength = sizeof(sa);				// nLength (size of struct)
		sa.bInheritHandle = TRUE;				// bInheritHandle (whether a child process inherits handle)
		sa.lpSecurityDescriptor = nullptr;		// lpSecurityDescriptor (unused pointer)

		HANDLE readPipe = nullptr;
		HANDLE writePipe = nullptr;

		if (!CreatePipe(&readPipe, &writePipe, &sa, 0))
		{
			return GetError("CreatePipe");
		}

		// Prevent the child from inheriting the read end. This is important to ensure clean exit.
		if (!SetHandleInformation(readPipe, HANDLE_FLAG_INHERIT, FALSE))
		{
			const auto error = GetError("SetHandleInformation");

			CloseHandle(readPipe);
			CloseHandle(writePipe);

			return error;
		}

		STARTUPINFOW si { };
		si.cb = sizeof(si);						// cb (size of struct)
		si.dwFlags = STARTF_USESTDHANDLES;		// dwFlags (sets specific members) (Use HstdInput, HStdOutput, hStdError)
		si.hStdInput = nullptr;					// hStdInput (defines standard input handle) (not using input here)
		si.hStdOutput = writePipe;				// hStdOutput (defines standard output handle)
		si.hStdError = writePipe;				// hStdError (defines standard error handle)

		PROCESS_INFORMATION pi { };

		// Build command line.
		std::wstring commandLine = L"\"";
		commandLine += application.native();
		commandLine += L"\"";

		if (!args.empty())
		{
			commandLine += L" ";
			commandLine += Ludus::Engine::Platform::Windows::Detail::Utf8ToWide(args);
		}

		// Build command line buffer that the process might modify.
		std::vector<wchar_t> commandLineBuffer(commandLine.begin(), commandLine.end());
		commandLineBuffer.push_back(L'\0');

		const std::wstring app = application.native();

		if (!CreateProcessW(
			app.c_str(),					// lpApplicationName (explicit module to execute - no args parse)
			commandLineBuffer.data(),		// lpCommandLine (app & args)
			nullptr,						// lpProcessAttributes (unused pointer)
			nullptr,						// lpThreadAttributes (unused pointer)
			TRUE,							// bInheritHandles (new processes inherits pipe handles)
			CREATE_NO_WINDOW,				// dwCreationFlags (creation flags) (no window)
			nullptr,						// lpEnvironment (unused pointer)
			nullptr,						// lpCurrentDirectory (current directory path)
			&si,							// lpStartupInfo (pointer to STARTUPINFOW)
			&pi))							// lpProcessInformation (pointer to PROCESS_INFORMATION)
		{
			const auto error = GetError("CreateProcessW");

			CloseHandle(readPipe);
			CloseHandle(writePipe);

			return error;
		}

		CloseHandle(writePipe);

		std::string output;
		char buffer[BufferSize];
		DWORD bytesRead = 0;

		BOOL success = TRUE;
		while (success = ReadFile(readPipe, buffer, BufferSize, &bytesRead, nullptr))
		{
			if (bytesRead == 0)
			{
				break;
			}

			output.append(buffer, bytesRead);
		}

		if (!success)
		{
			const auto err = GetLastError();
			if (err != ERROR_BROKEN_PIPE)
			{
				const auto error = GetError("ReadFile");

				CloseHandle(readPipe);
				CloseHandle(pi.hThread);
				CloseHandle(pi.hProcess);

				return error;
			}
		}

		// Ensure that the process is finished.
		if (WaitForSingleObject(pi.hProcess, INFINITE) == WAIT_FAILED)
		{
			const auto error = GetError("WaitForSingleObject");

			CloseHandle(readPipe);
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);

			return error;
		}

		DWORD exitCode = 0;
		if (!GetExitCodeProcess(pi.hProcess, &exitCode))
		{
			const auto error = GetError("GetExitCodeProcess");

			CloseHandle(readPipe);
			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);

			return error;
		}

		CloseHandle(readPipe);
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);

		ProcessResult result;
		result.ExitCode = static_cast<std::uint32_t>(exitCode);
		result.Output = std::move(output);

		return result;
	}
}
