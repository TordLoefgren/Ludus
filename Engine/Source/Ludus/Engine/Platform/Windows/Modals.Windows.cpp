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

#include "WinText.h"

// Windows classic samples - Common File Dialogs.
// https://github.com/microsoft/Windows-classic-samples/blob/main/Samples/Win7Samples/winui/shell/appplatform/commonfiledialog/CommonFileDialogApp.cpp
// See also
// https://learn.microsoft.com/en-us/windows/win32/shell/common-file-dialog.

namespace Ludus::Engine::Platform::Modals
{
	static constexpr std::array<COMDLG_FILTERSPEC, 4> fileTypes =
	{ { { L"Ludus Project File (*.project.ludus)", L"*.project.ludus" },
		{ L"Ludus Runtime File (*.runtime.ludus)", L"*.runtime.ludus" },
		{ L"Ludus Scene File (*.scene.ludus)", L"*.scene.ludus" },
		{ L"All Files (*.*)", L"*.*" } }
	};

	static constexpr UINT INDEX_LUDUSPROJECT = 1;
	static constexpr UINT INDEX_LUDUSRUNTIME = 2;
	static constexpr UINT INDEX_LUDUSSCENE = 3;

	static UINT FileTypeIndexFromExtension(std::string_view extension)
	{
		// Normalize extension parameter.
		if (!extension.empty() && extension.front() == '.')
		{
			extension.remove_prefix(1);
		}

		if (extension == "project.ludus")
		{
			return INDEX_LUDUSPROJECT;
		}
		if (extension == "runtime.ludus")
		{
			return INDEX_LUDUSRUNTIME;
		}
		if (extension == "scene.ludus")
		{
			return INDEX_LUDUSSCENE;
		}

		// Fallback to project file.
		return INDEX_LUDUSPROJECT;
	}

	static bool SetDialogStartupFolder(IFileDialog* dialog, const std::filesystem::path& folderPath)
	{
		if (!dialog || folderPath.empty())
		{
			return false;
		}

		std::filesystem::path absolutePath = std::filesystem::absolute(folderPath);
		std::wstring folderWide = absolutePath.wstring();
		if (folderWide.empty())
		{
			return false;
		}


		IShellItem* folderItem = nullptr;
		auto hr = SHCreateItemFromParsingName(folderWide.c_str(), nullptr, IID_PPV_ARGS(&folderItem));
		if (FAILED(hr) || !folderItem)
		{
			return false;
		}

		hr = dialog->SetFolder(folderItem);
		folderItem->Release();

		return SUCCEEDED(hr);
	}


	bool OpenFileDialog(std::filesystem::path& filenameOut, std::string_view defaultExtension, std::optional<std::filesystem::path> defaultStartupPath = std::nullopt)
	{
		bool isFileSelected = false;

		IFileDialog* openDialog = nullptr;
		auto hr = CoCreateInstance(CLSID_FileOpenDialog,
			nullptr,
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

				if (defaultStartupPath)
				{
					SetDialogStartupFolder(openDialog, *defaultStartupPath);
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
							auto defaultExtensionWide = Ludus::Engine::Platform::Windows::Detail::Utf8ToWide(defaultExtension);
							if (!defaultExtensionWide.empty())
							{
								hr = openDialog->SetDefaultExtension(defaultExtensionWide.c_str());
							}

							if (SUCCEEDED(hr))
							{
								// Show the dialog
								hr = openDialog->Show(nullptr);
								if (SUCCEEDED(hr))
								{
									// Get the result object from clicking the "Open" button.
									IShellItem* psiResult;
									hr = openDialog->GetResult(&psiResult);
									if (SUCCEEDED(hr))
									{
										PWSTR allocatedPathWide = nullptr;
										hr = psiResult->GetDisplayName(SIGDN_FILESYSPATH, &allocatedPathWide);
										if (SUCCEEDED(hr))
										{
											filenameOut = std::filesystem::path(allocatedPathWide);
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
		std::filesystem::path& filenameOut,
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

				if (defaultStartupPath)
				{
					SetDialogStartupFolder(saveDialog, *defaultStartupPath);
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
							std::wstring defExtW = Ludus::Engine::Platform::Windows::Detail::Utf8ToWide(defaultExtension);
							if (!defExtW.empty())
							{
								hr = saveDialog->SetDefaultExtension(defExtW.c_str());
							}

							if (SUCCEEDED(hr))
							{
								if (defaultFileName && !defaultFileName->empty())
								{
									std::wstring fileNameWide = Ludus::Engine::Platform::Windows::Detail::Utf8ToWide(*defaultFileName);
									saveDialog->SetFileName(fileNameWide.c_str());
								}

								hr = saveDialog->Show(nullptr);
								if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED))
								{
									saveDialog->Release();
									return false;
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
											filenameOut = std::filesystem::path(allocatedPathWide);
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
