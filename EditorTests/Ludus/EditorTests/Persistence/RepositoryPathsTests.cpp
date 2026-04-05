#include "pch.h"

#include <filesystem>

#include <Ludus/Editor/Persistence/RepositoryPaths.h>

namespace Ludus::EditorTests::Persistence
{
	namespace RepositoryPaths = Ludus::Editor::Persistence::RepositoryPaths;

	TEST(RepositoryPaths, ResolveRepositoryRoot_Should_ReturnRepositoryRoot)
	{
		// Arrange & Act.
		const auto path = RepositoryPaths::ResolveRepositoryRoot();

		// Assert.
		ASSERT_TRUE(std::filesystem::exists(path / "Ludus.sln"));
	}

	TEST(RepositoryPaths, EditorDirectory_Should_ReturnEditorDirectory)
	{
		// Arrange & Act.
		const auto path = RepositoryPaths::EditorDirectory();

		// Assert.
		ASSERT_EQ(path, RepositoryPaths::ResolveRepositoryRoot() / "Editor");
	}

	TEST(RepositoryPaths, EditorIncludeDirectory_Should_ReturnEditorIncludeDirectory)
	{
		// Arrange & Act.
		const auto path = RepositoryPaths::EditorIncludeDirectory();

		// Assert.
		ASSERT_EQ(path, RepositoryPaths::EditorDirectory() / "Include");
	}

	TEST(RepositoryPaths, EditorResourcesDirectory_Should_ReturnEditorResourcesDirectory)
	{
		// Arrange & Act.
		const auto path = RepositoryPaths::EditorResourcesDirectory();

		// Assert.
		ASSERT_EQ(path, RepositoryPaths::EditorDirectory() / "Resources");
	}

	TEST(RepositoryPaths, EngineDirectory_Should_ReturnEngineDirectory)
	{
		// Arrange & Act.
		const auto path = RepositoryPaths::EngineDirectory();

		// Assert.
		ASSERT_EQ(path, RepositoryPaths::ResolveRepositoryRoot() / "Engine");
	}

	TEST(RepositoryPaths, EngineIncludeDirectory_Should_ReturnEngineIncludeDirectory)
	{
		// Arrange & Act.
		const auto path = RepositoryPaths::EngineIncludeDirectory();

		// Assert.
		ASSERT_EQ(path, RepositoryPaths::EngineDirectory() / "Include");
	}

	TEST(RepositoryPaths, EngineResourcesDirectory_Should_ReturnEngineResourcesDirectory)
	{
		// Arrange & Act.
		const auto path = RepositoryPaths::EngineResourcesDirectory();

		// Assert.
		ASSERT_EQ(path, RepositoryPaths::EngineDirectory() / "Resources");
	}

	TEST(RepositoryPaths, EngineVendorsDirectory_Should_ReturnEngineVendorsDirectory)
	{
		// Arrange & Act.
		const auto path = RepositoryPaths::EngineVendorsDirectory();

		// Assert.
		ASSERT_EQ(path, RepositoryPaths::EngineDirectory() / "Vendors");
	}

	TEST(RepositoryPaths, ScriptingDirectory_Should_ReturnScriptingDirectory)
	{
		// Arrange & Act.
		const auto path = RepositoryPaths::ScriptingDirectory();

		// Assert.
		ASSERT_EQ(path, RepositoryPaths::ResolveRepositoryRoot() / "Scripting");
	}

	TEST(RepositoryPaths, ScriptingIncludeDirectory_Should_ReturnScriptingIncludeDirectory)
	{
		// Arrange & Act.
		const auto path = RepositoryPaths::ScriptingIncludeDirectory();

		// Assert.
		ASSERT_EQ(path, RepositoryPaths::ScriptingDirectory() / "Include");
	}

	TEST(RepositoryPaths, ScriptingAPIIncludeDirectory_Should_ReturnAPIIncludeDirectory)
	{
		// Arrange & Act.
		const auto path = RepositoryPaths::ScriptingAPIIncludeDirectory();

		// Assert.
		ASSERT_EQ(path, RepositoryPaths::ScriptingIncludeDirectory() / "Ludus" / "Scripting" / "API");
	}

	TEST(RepositoryPaths, TemplatesDirectory_Should_ReturnRequestedTemplateDirectory)
	{
		// Arrange & Act.
		const auto path = RepositoryPaths::TemplatesDirectory("RuntimeHost");

		// Assert.
		ASSERT_EQ(path, RepositoryPaths::EditorResourcesDirectory() / "Templates" / "RuntimeHost");
	}

	TEST(RepositoryPaths, RuntimeHostTemplatesDirectory_Should_ReturnRuntimeHostTemplatesDirectory)
	{
		// Arrange & Act.
		const auto path = RepositoryPaths::RuntimeHostTemplatesDirectory();

		// Assert.
		ASSERT_EQ(path, RepositoryPaths::EditorResourcesDirectory() / "Templates" / "RuntimeHost");
	}

	TEST(RepositoryPaths, ScriptTemplatesDirectory_Should_ReturnScriptTemplatesDirectory)
	{
		// Arrange & Act.
		const auto path = RepositoryPaths::ScriptTemplatesDirectory();

		// Assert.
		ASSERT_EQ(path, RepositoryPaths::EditorResourcesDirectory() / "Templates" / "Scripting");
	}
}
