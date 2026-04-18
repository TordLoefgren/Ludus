#include "pch.h"

#include <memory>
#include <utility>
#include <variant>

#include <Ludus/Engine/Core/Scene.h>
#include <Ludus/Engine/Core/SceneRegistry.h>
#include <Ludus/Engine/Persistence/IScenePersistence.h>
#include <Ludus/Engine/Runtime/PendingSceneTransition.h>
#include <Ludus/Engine/Runtime/RuntimeManifest.h>
#include <Ludus/Engine/Runtime/SceneRuntimeState.h>
#include <Ludus/Engine/Runtime/SceneSystem.h>
#include <Ludus/Engine/Scripting/ScriptBindings.h>
#include <Ludus/Engine/Windowing/Input.h>
#include <Ludus/Scripting/ABI/ScriptContext.h>
#include <Ludus/Scripting/API/Scripting.h>

namespace Ludus::EngineTests::Runtime
{
	using Scene = Ludus::Engine::Core::Scene;
	using SceneId = Ludus::Engine::Core::SceneId;
	using SceneRegistry = Ludus::Engine::Core::SceneRegistry;
	using IScenePersistence = Ludus::Engine::Persistence::IScenePersistence;
	using PendingSceneTransition = Ludus::Engine::Runtime::PendingSceneTransition;
	using RuntimeManifest = Ludus::Engine::Runtime::RuntimeManifest;
	using SceneRuntimeState = Ludus::Engine::Runtime::SceneRuntimeState;
	using SceneSystem = Ludus::Engine::Runtime::SceneSystem;
	using ScriptBindingsState = Ludus::Engine::Scripting::ScriptBindingsState;
	using ScriptContext = Ludus::Scripting::ABI::ScriptContext;
	using Input = Ludus::Engine::Windowing::Input;

	namespace Scripts = Ludus::Scripting::API;
	namespace ScriptABI = Ludus::Scripting::ABI;

	class TestScenePersistence final : public IScenePersistence
	{
	public:
		Scene LoadedScene;

		// IScenePersistence is a const interface, which means we have to make the properties mutable explicitly.
		mutable std::filesystem::path LastLoadedPath;
		mutable std::uint32_t LoadCallCount = 0;

		explicit TestScenePersistence(Scene loadedScene)
			: LoadedScene(std::move(loadedScene))
		{}

		virtual void Save(const Scene& scene, const std::filesystem::path& path) const override
		{
			(void)scene;
			(void)path;
			FAIL() << "Save should not be called by SceneSystem transition tests.";
		}

		virtual Scene Load(const std::filesystem::path& path) const override
		{
			LastLoadedPath = path;
			++LoadCallCount;
			return Scene::Clone(LoadedScene);
		}
	};

	static RuntimeManifest MakeRuntimeManifest()
	{
		return RuntimeManifest::Create(
			SceneId { 1 },
			{ { .Id = SceneId { 1 }, .Name = "MainMenu", .Path = "Scenes/MainMenu.scene.ludus" },
			{ .Id = SceneId { 2 }, .Name = "Gameplay", .Path = "Scenes/Gameplay.scene.ludus" } }
		);
	}

	TEST(SceneSystem, BeginFrame_Should_LoadPendingScene_AndUpdatePresentedScene)
	{
		// Arrange.
		SceneRegistry sceneRegistry;
		const auto initialSceneId = sceneRegistry.AddScene(Scene { SceneId { 10 }, "Initial" });

		SceneRuntimeState sceneRuntimeState;
		sceneRuntimeState.Presentation.CurrentSceneId = initialSceneId;
		sceneRuntimeState.PendingTransition = PendingSceneTransition::LoadScene("Scenes/Gameplay.scene.ludus");

		TestScenePersistence testPersistence(Scene { SceneId { 42 }, "Gameplay" });

		SceneSystem sceneSystem(testPersistence, sceneRegistry, sceneRuntimeState);

		// Act.
		sceneSystem.BeginFrame();

		// Assert.
		ASSERT_EQ(testPersistence.LoadCallCount, 1u);
		ASSERT_EQ(testPersistence.LastLoadedPath, std::filesystem::path("Scenes/Gameplay.scene.ludus"));
		ASSERT_TRUE(std::holds_alternative<PendingSceneTransition::None>(sceneRuntimeState.PendingTransition.Data));
		ASSERT_EQ(sceneRuntimeState.Presentation.CurrentSceneId, SceneId { 42 });
		ASSERT_EQ(sceneRegistry.View().size(), 1u);
		ASSERT_EQ(sceneRegistry.GetScene(SceneId { 42 }).Name, "Gameplay");
	}

	TEST(ScriptBindings, LoadSceneByName_Should_RequestTransition_WhenSceneExists)
	{
		// Arrange.
		const auto runtimeManifest = MakeRuntimeManifest();
		SceneRegistry sceneRegistry;
		SceneRuntimeState sceneRuntimeState;
		const auto currentSceneId = sceneRegistry.AddScene(Scene { SceneId { 11 }, "Current" });
		sceneRuntimeState.Presentation.CurrentSceneId = currentSceneId;

		Input input;
		ScriptBindingsState* bindings = Ludus::Engine::Scripting::CreateScriptBindingsState(
			runtimeManifest,
			sceneRegistry,
			sceneRuntimeState,
			input
		);
		ScriptContext* context = ScriptABI::CreateScriptContext(
			Ludus::Engine::Scripting::GetScriptAPI(bindings),
			Ludus::Engine::Scripting::GetScriptHost(bindings)
		);

		// Act.
		const bool loaded = Scripts::LoadSceneByName(context, "Gameplay");

		// Assert.
		ASSERT_TRUE(loaded);
		ASSERT_TRUE(std::holds_alternative<PendingSceneTransition::Load>(sceneRuntimeState.PendingTransition.Data));

		const auto& transition = std::get<PendingSceneTransition::Load>(sceneRuntimeState.PendingTransition.Data);
		ASSERT_EQ(transition.Path, std::filesystem::path("Scenes/Gameplay.scene.ludus"));

		ScriptABI::DestroyScriptContext(context);
		Ludus::Engine::Scripting::DestroyScriptBindingsState(bindings);
	}

	TEST(ScriptBindings, LoadSceneByName_Should_ReturnFalse_WhenSceneDoesNotExist)
	{
		// Arrange.
		const auto runtimeManifest = MakeRuntimeManifest();
		SceneRegistry sceneRegistry;
		SceneRuntimeState sceneRuntimeState;
		const auto currentSceneId = sceneRegistry.AddScene(Scene { SceneId { 11 }, "Current" });
		sceneRuntimeState.Presentation.CurrentSceneId = currentSceneId;

		Input input;
		ScriptBindingsState* bindings = Ludus::Engine::Scripting::CreateScriptBindingsState(
			runtimeManifest,
			sceneRegistry,
			sceneRuntimeState,
			input
		);
		ScriptContext* context = ScriptABI::CreateScriptContext(
			Ludus::Engine::Scripting::GetScriptAPI(bindings),
			Ludus::Engine::Scripting::GetScriptHost(bindings)
		);

		// Act.
		const bool loaded = Scripts::LoadSceneByName(context, "Missing");

		// Assert.
		ASSERT_FALSE(loaded);
		ASSERT_TRUE(std::holds_alternative<PendingSceneTransition::None>(sceneRuntimeState.PendingTransition.Data));

		ScriptABI::DestroyScriptContext(context);
		Ludus::Engine::Scripting::DestroyScriptBindingsState(bindings);
	}
}
