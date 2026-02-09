#pragma once

#include <atomic>
#include <chrono>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

namespace Ludus::Engine::IO
{

#pragma region Utility

	struct FileDeleteScope
	{
		std::filesystem::path Path;

		~FileDeleteScope() noexcept
		{
			if (!Path.empty())
			{
				// Safe removal.
				std::error_code _;
				std::filesystem::remove(Path, _);
			}
		}
	};

	struct DirectoryDeleteScope
	{
		std::filesystem::path Path;
		~DirectoryDeleteScope() noexcept
		{
			if (!Path.empty())
			{
				std::error_code _;
				std::filesystem::remove_all(Path, _);
			}
		}
	};

	inline std::string GenerateUniqueName(
		const std::string_view prefix,
		const std::string_view suffix
	)
	{
		// Create unique path. Can be replaced with GUIDs when they are needed more broadly.
		static std::atomic_uint64_t counter { 0 };

		const auto t = std::chrono::high_resolution_clock::now().time_since_epoch().count();
		const auto c = counter.fetch_add(1, std::memory_order_relaxed);

		return std::string(prefix) + std::to_string(t) + "_" + std::to_string(c) + std::string(suffix);
	}

	inline std::filesystem::path CreateTempFilePath(const std::filesystem::path& path)
	{
		const auto directory = path.parent_path();
		const auto name = path.filename().string();

		return directory / GenerateUniqueName(name + ".", ".tmp");
	}

	inline void CommitTempFile(
		const std::filesystem::path& temp,
		const std::filesystem::path& destination
	)
	{
		std::error_code errorCode;

#ifdef _WIN32

		// Platform specific: Windows is not completely atomic.
		const auto exists = std::filesystem::exists(destination, errorCode);
		if (errorCode)
		{
			throw std::runtime_error("Could not find file '" + destination.string() + "': " + errorCode.message());
		}

		if (exists)
		{
			std::filesystem::remove(destination, errorCode);
			if (errorCode)
			{
				throw std::runtime_error("Removing '" + destination.string() + "' failed: " + errorCode.message());
			}
		}

#endif

		std::filesystem::rename(temp, destination, errorCode);
		if (errorCode)
		{
			throw std::runtime_error("Renaming '" + destination.string() + "' failed: " + errorCode.message());
		}
	}

	inline void CommitTempFile(FileDeleteScope& temp, const std::filesystem::path& destination)
	{
		CommitTempFile(temp.Path, destination);
		temp.Path.clear();
	}

	inline std::vector<std::filesystem::path> GetFilePaths(const std::filesystem::path& path)
	{
		auto filePaths = std::vector<std::filesystem::path>();

		for (const auto& p : std::filesystem::recursive_directory_iterator(path))
		{
			if (!p.is_regular_file())
			{
				// Skip directories.
				continue;
			}

			filePaths.push_back(p.path());
		}

		return filePaths;
	}

	inline std::vector<std::string> GetFileNames(const std::filesystem::path& path)
	{
		const auto filePaths = GetFilePaths(path);
		auto fileNames = std::vector<std::string>();
		fileNames.reserve(filePaths.size());

		for (const auto& p : filePaths)
		{
			fileNames.push_back(p.filename().string());
		}

		return fileNames;
	}

#pragma endregion

	inline std::vector<std::byte> ReadAllBytes(const std::filesystem::path& path)
	{
		std::error_code errorCode;
		if (!std::filesystem::exists(path, errorCode) || errorCode)
		{
			throw std::runtime_error("File does not exist.");
		}

		const auto length = std::filesystem::file_size(path, errorCode);
		if (errorCode)
		{
			throw std::runtime_error("Error while attempting to get file size.");
		}

		std::vector<std::byte> buffer(length);

		std::ifstream file(path, std::ios_base::binary);
		if (!file)
		{
			throw std::runtime_error("Could not open file.");
		}

		if (length > 0)
		{
			file.read(reinterpret_cast<char*>(buffer.data()), static_cast<std::streamsize>(length));
			if (!file || file.gcount() != static_cast<std::streamsize>(length))
			{
				throw std::runtime_error("Could not read all bytes.");
			}
		}

		return buffer;
	}

	inline std::string ReadAllText(const std::filesystem::path& path)
	{
		std::error_code errorCode;
		if (!std::filesystem::exists(path, errorCode) || errorCode)
		{
			throw std::runtime_error("File does not exist.");
		}

		const auto length = std::filesystem::file_size(path, errorCode);
		if (errorCode)
		{
			throw std::runtime_error("Error while attempting to get file size.");
		}

		std::string buffer;
		buffer.resize(length);

		std::ifstream file(path, std::ios::binary);
		if (!file)
		{
			throw std::runtime_error("Could not open file.");
		}

		if (length > 0)
		{
			file.read(buffer.data(), static_cast<std::streamsize>(length));
			if (!file || file.gcount() != static_cast<std::streamsize>(length))
			{
				throw std::runtime_error("Could not read all text.");
			}
		}

		return buffer;
	}

	inline void WriteAllBytes(const std::filesystem::path& path, std::span<const std::byte> data)
	{
		const auto temp = CreateTempFilePath(path);
		FileDeleteScope tempScoped { temp };

		std::ofstream file(tempScoped.Path, std::ios::binary | std::ios::trunc);
		if (!file)
		{
			throw std::runtime_error("Could not create temporary file.");
		}

		file.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size_bytes()));
		if (!file)
		{
			throw std::runtime_error("Could not write all bytes.");
		}

		file.flush();
		file.close();

		CommitTempFile(tempScoped, path);
	}

	inline void WriteAllText(const std::filesystem::path& path, std::string_view data)
	{
		const auto temp = CreateTempFilePath(path);
		FileDeleteScope tempScoped { temp };

		std::ofstream file(tempScoped.Path, std::ios::binary | std::ios::trunc);
		if (!file)
		{
			throw std::runtime_error("Could not create temporary file.");
		}

		file.write(data.data(), static_cast<std::streamsize>(data.size()));
		if (!file)
		{
			throw std::runtime_error("Could not write all text.");
		}

		file.flush();
		file.close();

		CommitTempFile(tempScoped, path);
	}
}
