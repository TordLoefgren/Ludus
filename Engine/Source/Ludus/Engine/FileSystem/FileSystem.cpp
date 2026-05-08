#include "pch.h"

#include <atomic>
#include <chrono>
#include <fstream>
#include <stdexcept>
#include <system_error>

#include <Ludus/Engine/FileSystem/FileSystem.h>

namespace Ludus::Engine::FileSystem
{
	FileDeleteScope::~FileDeleteScope() noexcept
	{
		if (!Path.empty())
		{
			std::error_code _;
			std::filesystem::remove(Path, _);
		}
	}

	DirectoryDeleteScope::~DirectoryDeleteScope() noexcept
	{
		if (!Path.empty())
		{
			std::error_code _;
			std::filesystem::remove_all(Path, _);
		}
	}

	std::string GenerateUniqueName(std::string_view prefix, std::string_view suffix)
	{
		static std::atomic_uint64_t counter { 0 };

		const auto t = std::chrono::high_resolution_clock::now().time_since_epoch().count();
		const auto c = counter.fetch_add(1, std::memory_order_relaxed);

		return std::string(prefix) + std::to_string(t) + "_" + std::to_string(c) + std::string(suffix);
	}

	std::filesystem::path CreateTempFilePath(const std::filesystem::path& path)
	{
		const auto directory = path.parent_path();
		const auto name = path.filename().string();
		return directory / GenerateUniqueName(name + ".", ".tmp");
	}

	std::vector<std::filesystem::path> GetFilePaths(const std::filesystem::path& path)
	{
		auto filePaths = std::vector<std::filesystem::path>();

		for (const auto& p : std::filesystem::recursive_directory_iterator(path))
		{
			if (!p.is_regular_file())
			{
				continue;
			}

			filePaths.push_back(p.path());
		}

		return filePaths;
	}

	std::vector<std::string> GetFileNames(const std::filesystem::path& path)
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

	std::string ReadAllText(const std::filesystem::path& path)
	{
		std::ifstream file(path, std::ios::binary);
		if (!file)
		{
			throw std::runtime_error("Could not open file: " + path.string());
		}

		return std::string(
			std::istreambuf_iterator<char>(file),
			std::istreambuf_iterator<char>()
		);
	}

	std::vector<std::byte> ReadAllBytes(const std::filesystem::path& path)
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

		std::ifstream file(path, std::ios::binary);
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

	void WriteAllText(const std::filesystem::path& path, std::string_view text)
	{
		std::ofstream file(path, std::ios::binary | std::ios::trunc);
		if (!file)
		{
			throw std::runtime_error("Could not write file: " + path.string());
		}

		file.write(text.data(), static_cast<std::streamsize>(text.size()));
		if (!file)
		{
			throw std::runtime_error("Could not write all text: " + path.string());
		}
	}

	void WriteAllBytes(const std::filesystem::path& path, std::span<const std::byte> data)
	{
		std::ofstream file(path, std::ios::binary | std::ios::trunc);
		if (!file)
		{
			throw std::runtime_error("Could not write file: " + path.string());
		}

		file.write(reinterpret_cast<const char*>(data.data()), static_cast<std::streamsize>(data.size_bytes()));
		if (!file)
		{
			throw std::runtime_error("Could not write all bytes: " + path.string());
		}
	}

	void CopyFileOverwrite(const std::filesystem::path& source, const std::filesystem::path& destination)
	{
		std::error_code errorCode;
		std::filesystem::copy_file(source, destination, std::filesystem::copy_options::overwrite_existing, errorCode);
		if (errorCode)
		{
			throw std::runtime_error(
				"Failed to copy file from '" + source.string() + "' to '" + destination.string() + "'. " + errorCode.message()
			);
		}
	}

	void RemoveIfExists(const std::filesystem::path& path)
	{
		std::error_code errorCode;
		std::filesystem::remove_all(path, errorCode);
		if (errorCode)
		{
			throw std::runtime_error("Failed to remove path: " + path.string() + ". " + errorCode.message());
		}
	}

	void ReplaceDirectory(const std::filesystem::path& source, const std::filesystem::path& destination)
	{
		std::error_code errorCode;
		std::filesystem::remove_all(destination, errorCode);
		if (errorCode)
		{
			throw std::runtime_error("Failed to clean directory: " + destination.string() + ". " + errorCode.message());
		}

		std::filesystem::copy(source, destination, std::filesystem::copy_options::recursive, errorCode);
		if (errorCode)
		{
			throw std::runtime_error(
				"Failed to copy directory from '" + source.string() + "' to '" + destination.string() + "'. " + errorCode.message()
			);
		}
	}

	void RemoveDirectoryIfEmpty(const std::filesystem::path& path)
	{
		std::error_code errorCode;
		const bool exists = std::filesystem::exists(path, errorCode);
		if (errorCode)
		{
			throw std::runtime_error("Failed to inspect directory: " + path.string() + ". " + errorCode.message());
		}

		if (!exists || !std::filesystem::is_empty(path, errorCode))
		{
			if (errorCode)
			{
				throw std::runtime_error("Failed to inspect directory emptiness: " + path.string() + ". " + errorCode.message());
			}

			return;
		}

		std::filesystem::remove(path, errorCode);
		if (errorCode)
		{
			throw std::runtime_error("Failed to remove directory: " + path.string() + ". " + errorCode.message());
		}
	}

	std::string ToPortablePathString(const std::filesystem::path& path)
	{
		return path.lexically_normal().generic_string();
	}

	std::filesystem::path NormalizePortablePath(const std::filesystem::path& path)
	{
		return std::filesystem::path(ToPortablePathString(path));
	}

	bool ArePathsEqual(const std::filesystem::path& left, const std::filesystem::path& right)
	{
		std::error_code errorCode;
		if (std::filesystem::equivalent(left, right, errorCode))
		{
			return true;
		}

		return left.lexically_normal() == right.lexically_normal();
	}
}
