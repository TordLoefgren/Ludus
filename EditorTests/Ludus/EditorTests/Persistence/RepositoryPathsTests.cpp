#include "pch.h"

#include <filesystem>

#include <Ludus/Editor/Persistence/RepositoryPaths.h>

namespace Ludus::EditorTests::Persistence
{
	namespace RepositoryPaths = Ludus::Editor::Persistence::RepositoryPaths;

	TEST(RepositoryPaths, ResolveRepositoryRoot_Should_ReturnRepositoryRoot)
	{
		const auto path = RepositoryPaths::ResolveRepositoryRoot();
		ASSERT_TRUE(std::filesystem::exists(path / "Ludus.sln"));
	}

	TEST(RepositoryPaths, EditorDirectory_Should_ReturnEditorDirectory)
	{
		const auto path = RepositoryPaths::EditorDirectory();
		ASSERT_EQ(path, RepositoryPaths::ResolveRepositoryRoot() / "Editor");
	}

	TEST(RepositoryPaths, EditorIncludeDirectory_Should_ReturnEditorIncludeDirectory)
	{
		const auto path = RepositoryPaths::EditorIncludeDirectory();
		ASSERT_EQ(path, RepositoryPaths::EditorDirectory() / "Include");
	}

	TEST(RepositoryPaths, EditorResourcesDirectory_Should_ReturnEditorResourcesDirectory)
	{
		const auto path = RepositoryPaths::EditorResourcesDirectory();
		ASSERT_EQ(path, RepositoryPaths::EditorDirectory() / "Resources");
	}

	TEST(RepositoryPaths, EngineDirectory_Should_ReturnEngineDirectory)
	{
		const auto path = RepositoryPaths::EngineDirectory();
		ASSERT_EQ(path, RepositoryPaths::ResolveRepositoryRoot() / "Engine");
	}

	TEST(RepositoryPaths, EngineIncludeDirectory_Should_ReturnEngineIncludeDirectory)
	{
		const auto path = RepositoryPaths::EngineIncludeDirectory();
		ASSERT_EQ(path, RepositoryPaths::EngineDirectory() / "Include");
	}

	TEST(RepositoryPaths, EngineResourcesDirectory_Should_ReturnEngineResourcesDirectory)
	{
		const auto path = RepositoryPaths::EngineResourcesDirectory();
		ASSERT_EQ(path, RepositoryPaths::EngineDirectory() / "Resources");
	}

	TEST(RepositoryPaths, EngineVendorsDirectory_Should_ReturnEngineVendorsDirectory)
	{
		const auto path = RepositoryPaths::EngineVendorsDirectory();
		ASSERT_EQ(path, RepositoryPaths::EngineDirectory() / "Vendors");
	}

	TEST(RepositoryPaths, ScriptingDirectory_Should_ReturnScriptingDirectory)
	{
		const auto path = RepositoryPaths::ScriptingDirectory();
		ASSERT_EQ(path, RepositoryPaths::ResolveRepositoryRoot() / "Scripting");
	}

	TEST(RepositoryPaths, ScriptingIncludeDirectory_Should_ReturnScriptingIncludeDirectory)
	{
		const auto path = RepositoryPaths::ScriptingIncludeDirectory();
		ASSERT_EQ(path, RepositoryPaths::ScriptingDirectory() / "Include");
	}

	TEST(RepositoryPaths, ScriptingAPIIncludeDirectory_Should_ReturnAPIIncludeDirectory)
	{
		const auto path = RepositoryPaths::ScriptingAPIIncludeDirectory();
		ASSERT_EQ(path, RepositoryPaths::ScriptingIncludeDirectory() / "Ludus" / "Scripting" / "API");
	}

	TEST(RepositoryPaths, TemplatesDirectory_Should_ReturnRequestedTemplateDirectory)
	{
		const auto path = RepositoryPaths::TemplatesDirectory("RuntimeHost");
		ASSERT_EQ(path, RepositoryPaths::EditorResourcesDirectory() / "Templates" / "RuntimeHost");
	}

	TEST(RepositoryPaths, RuntimeHostTemplatesDirectory_Should_ReturnRuntimeHostTemplatesDirectory)
	{
		const auto path = RepositoryPaths::RuntimeHostTemplatesDirectory();
		ASSERT_EQ(path, RepositoryPaths::EditorResourcesDirectory() / "Templates" / "RuntimeHost");
	}

	TEST(RepositoryPaths, ScriptTemplatesDirectory_Should_ReturnScriptTemplatesDirectory)
	{
		const auto path = RepositoryPaths::ScriptTemplatesDirectory();
		ASSERT_EQ(path, RepositoryPaths::EditorResourcesDirectory() / "Templates" / "Scripting");
	}
}
