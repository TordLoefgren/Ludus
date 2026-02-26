#include "pch.h"

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <shobjidl_core.h> 
#include <objbase.h>       
#include <shlobj.h>

#include <string>
#include <array>
#include <filesystem>
#include <string_view>

// Windows classic samples - Common File Dialogs.
// https://github.com/microsoft/Windows-classic-samples/blob/main/Samples/Win7Samples/winui/shell/appplatform/commonfiledialog/CommonFileDialogApp.cpp
// See also
// https://learn.microsoft.com/en-us/windows/win32/shell/common-file-dialog.

namespace Ludus::Engine::Platform::Modals
{
	static constexpr std::array<COMDLG_FILTERSPEC, 6> fileTypes =
	{ {
		{ L"Word Document (*.doc)",          L"*.doc" },
		{ L"Web Page (*.htm; *.html)",       L"*.htm;*.html" },
		{ L"Text Document (*.txt)",          L"*.txt" },
		{ L"Ludus Project File (*.lproj)",   L"*.lproj" },
		{ L"Ludus Scene File (*.lscene)",   L"*.lscene" },
		{ L"All Documents (*.*)",            L"*.*" }
	} };

	static constexpr UINT INDEX_WORDDOC = 1;
	static constexpr UINT INDEX_WEBPAGE = 2;
	static constexpr UINT INDEX_TEXTDOC = 3;
	static constexpr UINT INDEX_LUDUSPROJ = 4;
	static constexpr UINT INDEX_LUDUSSCENE = 5;

	static UINT FileTypeIndexFromExtension(std::string_view extension)
	{
		// Normalize extension parameter.
		if (!extension.empty() && extension.front() == '.')
		{
			extension.remove_prefix(1);
		}

		if (extension == "doc")
		{
			return INDEX_WORDDOC;
		}
		if (extension == "htm" || extension == "html")
		{
			return INDEX_WEBPAGE;
		}
		if (extension == "txt")
		{
			return INDEX_TEXTDOC;
		}
		if (extension == "lproj")
		{
			return INDEX_LUDUSPROJ;
		}
		if (extension == "lscene")
		{
			return INDEX_LUDUSSCENE;
		}

		// Fallback to project file.
		return INDEX_LUDUSPROJ;
	}

	// Inspiration from Geeks For Geeks implementation.
	// https://www.geeksforgeeks.org/cpp/convert-lpcwstr-to-std_string-in-cpp/
	static std::wstring Utf8ToWide(std::string_view string)
	{
		if (string.empty())
		{
			return {};
		}

		auto length = MultiByteToWideChar(CP_UTF8, 0, string.data(), static_cast<int>(string.size()), nullptr, 0);
		if (length <= 0)
		{
			return {};
		}

		std::wstring out(static_cast<size_t>(length), L'\0');
		MultiByteToWideChar(CP_UTF8, 0, string.data(), static_cast<int>(string.size()), out.data(), length);

		return out;
	}

	static std::string WideToUtf8(const wchar_t* wideString)
	{
		if (!wideString || *wideString == L'\0')
		{
			return {};
		}

		int len = WideCharToMultiByte(CP_UTF8, 0, wideString, -1, nullptr, 0, nullptr, nullptr);
		if (len <= 0)
		{
			return {};
		}

		// Length includes the null terminator.
		std::string out(static_cast<size_t>(len), '\0');
		WideCharToMultiByte(CP_UTF8, 0, wideString, -1, out.data(), len, nullptr, nullptr);

		// Remove null terminator.
		out.pop_back();

		return out;
	}

	static bool SetDialogStartupFolder(IFileDialog* dialog, std::string_view folderUtf8)
	{
		if (!dialog || folderUtf8.empty())
		{
			return false;
		}

		std::wstring folderW = Utf8ToWide(folderUtf8);
		if (folderW.empty())
		{
			return false;
		}

		IShellItem* folderItem = nullptr;
		auto hr = SHCreateItemFromParsingName(folderW.c_str(), nullptr, IID_PPV_ARGS(&folderItem));
		if (FAILED(hr) || !folderItem)
		{
			return false;
		}

		hr = dialog->SetFolder(folderItem);
		folderItem->Release();

		return SUCCEEDED(hr);
	}


	bool OpenFileDialog(std::string& filenameOut, std::string_view defaultExtension, std::optional<std::filesystem::path> defaultStartupPath = std::nullopt)
	{
		bool isFileSelected = false;

		IFileDialog* openDialog = NULL;
		auto hr = CoCreateInstance(CLSID_FileOpenDialog,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&openDialog)
		);

		if (SUCCEEDED(hr))
		{
			// Set the options on the dialog. Make sure to not override existing options.
			DWORD flags;
			hr = openDialog->GetOptions(&flags);
			if (SUCCEEDED(hr))
			{
				// In this case, get shell items only for file system items.
				hr = openDialog->SetOptions(flags | FOS_FORCEFILESYSTEM);

				if (defaultStartupPath.has_value())
				{
					SetDialogStartupFolder(openDialog, defaultStartupPath.value().string());
				}

				if (SUCCEEDED(hr))
				{
					// Set the file types for display. 
					hr = openDialog->SetFileTypes(static_cast<UINT>(fileTypes.size()), fileTypes.data());
					if (SUCCEEDED(hr))
					{
						const auto filterIndex = FileTypeIndexFromExtension(defaultExtension);
						hr = openDialog->SetFileTypeIndex(filterIndex);
						if (SUCCEEDED(hr))
						{
							auto defaultExtensionWide = Utf8ToWide(defaultExtension);
							if (!defaultExtensionWide.empty())
							{
								hr = openDialog->SetDefaultExtension(defaultExtensionWide.c_str());
							}

							if (SUCCEEDED(hr))
							{
								// Show the dialog
								hr = openDialog->Show(NULL);
								if (SUCCEEDED(hr))
								{
									// Get the result object from clicking the "Open" button.
									IShellItem* psiResult;
									hr = openDialog->GetResult(&psiResult);
									if (SUCCEEDED(hr))
									{
										PWSTR allocatedPathWide = NULL;
										hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &allocatedPathWide);
										if (SUCCEEDED(hr))
										{
											filenameOut = WideToUtf8(allocatedPathWide);
											isFileSelected = true;

											CoTaskMemFree(allocatedPathWide);
										}
										psiResult->Release();
									}
								}
							}
						}
					}
				}
			}
		}

		if (openDialog)
		{
			openDialog->Release();
		}

		return isFileSelected;
	}

	bool SaveFileDialog(
		std::string& filenameOut,
		std::string_view defaultExtension,
		std::optional<std::filesystem::path> defaultStartupPath = std::nullopt,
		std::optional<std::string_view> defaultFileName = std::nullopt
	)
	{
		bool isFileSelected = false;

		IFileSaveDialog* saveDialog = nullptr;
		auto hr = CoCreateInstance(
			CLSID_FileSaveDialog,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&saveDialog)
		);

		if (SUCCEEDED(hr) && saveDialog)
		{
			DWORD flags = 0;
			hr = saveDialog->GetOptions(&flags);
			if (SUCCEEDED(hr))
			{
				// Options for saving:
				// - FOS_OVERWRITEPROMPT: Confirm overwrite.
				// - FOS_STRICTFILETYPES: Keep extension aligned with the selected filter.
				hr = saveDialog->SetOptions(flags | FOS_FORCEFILESYSTEM | FOS_OVERWRITEPROMPT | FOS_STRICTFILETYPES);

				if (defaultStartupPath.has_value())
				{
					SetDialogStartupFolder(saveDialog, defaultStartupPath.value().string());
				}

				if (SUCCEEDED(hr))
				{
					hr = saveDialog->SetFileTypes(static_cast<UINT>(fileTypes.size()), fileTypes.data());
					if (SUCCEEDED(hr))
					{
						const auto filterIndex = FileTypeIndexFromExtension(defaultExtension);
						hr = saveDialog->SetFileTypeIndex(filterIndex);
						if (SUCCEEDED(hr))
						{
							std::wstring defExtW = Utf8ToWide(defaultExtension);
							if (!defExtW.empty())
							{
								hr = saveDialog->SetDefaultExtension(defExtW.c_str());
							}

							if (SUCCEEDED(hr))
							{
								if (defaultFileName.has_value() && !defaultFileName->empty())
								{
									std::wstring fileNameWide = Utf8ToWide(*defaultFileName);
									saveDialog->SetFileName(fileNameWide.c_str());
								}

								hr = saveDialog->Show(nullptr);
								if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED))
								{
									LUDUS_ASSERT(false, "User cancellation not implemented.");
								}
								else if (SUCCEEDED(hr))
								{
									IShellItem* psiResult = nullptr;
									hr = saveDialog->GetResult(&psiResult);
									if (SUCCEEDED(hr) && psiResult)
									{
										PWSTR allocatedPathWide = nullptr;
										hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &allocatedPathWide);
										if (SUCCEEDED(hr) && allocatedPathWide)
										{
											filenameOut = WideToUtf8(allocatedPathWide);
											isFileSelected = true;

											CoTaskMemFree(allocatedPathWide);
										}
										psiResult->Release();
									}
								}
							}
						}
					}
				}
			}

			saveDialog->Release();
		}

		return isFileSelected;
	}
}
