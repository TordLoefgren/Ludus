#include "pch.h"

#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstddef>
#include <filesystem>
#include <numeric>
#include <span>
#include <string>
#include <system_error>
#include <vector>

#include <Ludus/Engine/FileSystem/FileSystem.h>

namespace Ludus::EngineTests::FileSystem
{
	namespace FileSystem = Ludus::Engine::FileSystem;

	static std::filesystem::path MakeUniqueTempDir()
	{
		return std::filesystem::temp_directory_path() / FileSystem::GenerateUniqueName("Ludus_IO_Tests_", "");
	}

	FileSystem::DirectoryDeleteScope CreateTestDirectory()
	{
		return FileSystem::DirectoryDeleteScope { MakeUniqueTempDir() };
	}

	TEST(IO, ReadAllBytes_Should_ReadExactBytes)
	{
		// Arrange.
		const auto tempDirectoryScoped = CreateTestDirectory();
		std::filesystem::create_directories(tempDirectoryScoped.Path);
		const auto tempFile = tempDirectoryScoped.Path / "data.bin";

		std::vector<std::uint8_t> data(256);
		std::iota(data.begin(), data.end(), 0);

		FileSystem::WriteAllBytes(tempFile, std::as_bytes(std::span { data }));

		// Act.
		const auto result = FileSystem::ReadAllBytes(tempFile);

		// Assert.
		ASSERT_EQ(result.size(), data.size());
		ASSERT_TRUE(std::equal(
			result.begin(),
			result.end(),
			std::as_bytes(std::span { data }).begin())
		);
	}

	TEST(IO, ReadAllBytes_Should_ReturnEmpty_When_FileEmpty)
	{
		// Arrange.
		const auto tempDirectoryScoped = CreateTestDirectory();
		std::filesystem::create_directories(tempDirectoryScoped.Path);
		const auto tempFile = tempDirectoryScoped.Path / "empty.bin";

		std::vector<std::uint8_t> data;

		FileSystem::WriteAllBytes(tempFile, std::as_bytes(std::span { data }));

		// Act.
		const auto result = FileSystem::ReadAllBytes(tempFile);

		// Assert.
		ASSERT_TRUE(result.empty());
	}

	TEST(IO, ReadAllBytes_Should_ThrowRuntimeError_When_FileMissing)
	{
		// Arrange.
		const auto tempDirectoryScoped = CreateTestDirectory();
		std::filesystem::create_directories(tempDirectoryScoped.Path);

		// Act & Assert.
		ASSERT_THROW(
			FileSystem::ReadAllBytes(tempDirectoryScoped.Path / "Missing.bin"),
			std::runtime_error
		);
	}

	TEST(IO, ReadAllText_Should_ReadExactText)
	{
		// Arrange.
		const auto tempDirectoryScoped = CreateTestDirectory();
		std::filesystem::create_directories(tempDirectoryScoped.Path);
		const auto tempFile = tempDirectoryScoped.Path / "text.txt";

		const auto text = std::string("\r\nHello there.\n");

		FileSystem::WriteAllText(tempFile, text);

		// Act.
		const auto result = FileSystem::ReadAllText(tempFile);

		// Assert.
		ASSERT_EQ(result.size(), text.size());
		ASSERT_EQ(result, text);
	}

	TEST(IO, ReadAllText_Should_ReturnEmpty_When_FileEmpty)
	{
		// Arrange.
		const auto tempDirectoryScoped = CreateTestDirectory();
		std::filesystem::create_directories(tempDirectoryScoped.Path);
		const auto tempFile = tempDirectoryScoped.Path / "empty.txt";

		const auto text = std::string("");

		FileSystem::WriteAllText(tempFile, text);

		// Act.
		const auto result = FileSystem::ReadAllText(tempFile);

		// Assert.
		ASSERT_TRUE(result.empty());
	}

	TEST(IO, ReadAllText_Should_ThrowRuntimeError_When_FileMissing)
	{
		// Arrange.
		const auto tempDirectoryScoped = CreateTestDirectory();
		std::filesystem::create_directories(tempDirectoryScoped.Path);

		// Act & Assert.
		ASSERT_THROW(
			FileSystem::ReadAllText(tempDirectoryScoped.Path / "Missing.txt"),
			std::runtime_error
		);
	}

	TEST(IO, WriteAllBytes_Should_OverwriteShorter)
	{
		// Arrange.
		const auto tempDirectoryScoped = CreateTestDirectory();
		std::filesystem::create_directories(tempDirectoryScoped.Path);
		const auto tempFile = tempDirectoryScoped.Path / "overwrite.bin";

		std::vector<std::uint8_t> first(100);
		std::iota(first.begin(), first.end(), 0);

		std::vector<std::uint8_t> second = { 7, 8 };

		FileSystem::WriteAllBytes(tempFile, std::as_bytes(std::span { first }));
		FileSystem::WriteAllBytes(tempFile, std::as_bytes(std::span { second }));

		// Act.
		const auto result = FileSystem::ReadAllBytes(tempFile);

		// Assert.
		ASSERT_EQ(result.size(), second.size());
		ASSERT_TRUE(std::equal(
			result.begin(),
			result.end(),
			std::as_bytes(std::span { second }).begin())
		);
	}

	TEST(IO, WriteAllText_Should_OverwriteShorter)
	{
		// Arrange.
		const auto tempDirectoryScoped = CreateTestDirectory();
		std::filesystem::create_directories(tempDirectoryScoped.Path);
		const auto tempFile = tempDirectoryScoped.Path / "overwrite.txt";

		FileSystem::WriteAllText(tempFile, "Hello world");
		FileSystem::WriteAllText(tempFile, "Hi");

		// Act.
		const auto result = FileSystem::ReadAllText(tempFile);

		// Assert.
		ASSERT_EQ(result, "Hi");
	}

	TEST(IO, WriteAllBytes_Should_CreateFile_When_FileMissing)
	{
		// Arrange.
		const auto tempDirectoryScoped = CreateTestDirectory();
		std::filesystem::create_directories(tempDirectoryScoped.Path);
		const auto tempFile = tempDirectoryScoped.Path / "create.bin";

		std::vector<std::uint8_t> data(32);
		std::iota(data.begin(), data.end(), 0);

		// Act.
		FileSystem::WriteAllBytes(tempFile, std::as_bytes(std::span { data }));

		// Assert.
		ASSERT_TRUE(std::filesystem::exists(tempFile));
		ASSERT_EQ(std::filesystem::file_size(tempFile), data.size());

		const auto result = FileSystem::ReadAllBytes(tempFile);
		ASSERT_EQ(result.size(), data.size());
		ASSERT_TRUE(std::equal(
			result.begin(),
			result.end(),
			std::as_bytes(std::span { data }).begin())
		);
	}

	TEST(IO, WriteAllText_Should_PreserveEmbeddedNulls)
	{
		// Arrange.
		const auto tempDirectoryScoped = CreateTestDirectory();
		std::filesystem::create_directories(tempDirectoryScoped.Path);
		const auto tempFile = tempDirectoryScoped.Path / "nulls.txt";

		const std::string text("A\0B", 3);

		// Act.
		FileSystem::WriteAllText(tempFile, text);
		const auto result = FileSystem::ReadAllText(tempFile);

		// Assert.
		ASSERT_EQ(result.size(), text.size());
		ASSERT_EQ(result, text);
	}

	TEST(IO, GetFilePaths_Should_ReturnOnlyRegularFiles_AndIncludeNestedFiles)
	{
		// Arrange.
		const auto tempDirectoryScoped = CreateTestDirectory();
		std::filesystem::create_directories(tempDirectoryScoped.Path);

		const auto nested = tempDirectoryScoped.Path / "nested";
		std::filesystem::create_directories(nested);

		const auto fileA = tempDirectoryScoped.Path / "a.txt";
		const auto fileB = nested / "b.txt";

		FileSystem::WriteAllText(fileA, "A");
		FileSystem::WriteAllText(fileB, "B");

		// Act.
		const auto paths = FileSystem::GetFilePaths(tempDirectoryScoped.Path);

		// Assert.
		ASSERT_EQ(paths.size(), 2u);
		ASSERT_TRUE(std::find(paths.begin(), paths.end(), fileA) != paths.end());
		ASSERT_TRUE(std::find(paths.begin(), paths.end(), fileB) != paths.end());
	}

	TEST(IO, GetFileNames_Should_ReturnFilenames_AndAllowDuplicates)
	{
		// Arrange.
		const auto tempDirectoryScoped = CreateTestDirectory();
		std::filesystem::create_directories(tempDirectoryScoped.Path);

		const auto aDir = tempDirectoryScoped.Path / "a";
		const auto bDir = tempDirectoryScoped.Path / "b";
		std::filesystem::create_directories(aDir);
		std::filesystem::create_directories(bDir);

		const auto fileA = aDir / "config.json";
		const auto fileB = bDir / "config.json";

		FileSystem::WriteAllText(fileA, "{}");
		FileSystem::WriteAllText(fileB, "{}");

		// Act.
		const auto names = FileSystem::GetFileNames(tempDirectoryScoped.Path);

		// Assert.
		ASSERT_EQ(names.size(), 2u);
		ASSERT_EQ(std::count(names.begin(), names.end(), "config.json"), 2);
	}

	TEST(IO, ResolvePathFromRoot_Should_ReturnPathUnderRoot)
	{
		// Arrange.
		const auto root = std::filesystem::path("C:/Projects/Sandbox");

		// Act.
		const auto path = FileSystem::ResolvePathFromRoot(root, "Scenes/Main.scene.ludus");

		// Assert.
		ASSERT_EQ(path, root / "Scenes" / "Main.scene.ludus");
	}

	TEST(IO, IsRelativePathUnderDirectory_Should_ReturnTrue_WhenPathIsUnderDirectory)
	{
		// Arrange & Act.
		const auto isUnderDirectory = FileSystem::IsRelativePathUnderDirectory("Assets/Player.png", "Assets");

		// Assert.
		ASSERT_TRUE(isUnderDirectory);
	}

	TEST(IO, IsRelativePathUnderDirectory_Should_ReturnFalse_WhenPathIsAbsolute)
	{
		// Arrange & Act.
		const auto isUnderDirectory = FileSystem::IsRelativePathUnderDirectory("C:/Projects/Sandbox/Assets/Player.png", "Assets");

		// Assert.
		ASSERT_FALSE(isUnderDirectory);
	}

	TEST(IO, IsRelativePathUnderDirectory_Should_ReturnFalse_WhenPathIsOutsideDirectory)
	{
		// Arrange & Act.
		const auto isUnderDirectory = FileSystem::IsRelativePathUnderDirectory("Scenes/Main.scene.ludus", "Assets");

		// Assert.
		ASSERT_FALSE(isUnderDirectory);
	}
}
