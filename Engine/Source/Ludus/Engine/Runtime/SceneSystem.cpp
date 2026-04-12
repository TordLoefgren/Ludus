#include "pch.h"

#include <filesystem>
#include <memory>
#include <optional>
#include <type_traits>
#include <utility>
#include <variant>

#include <Ludus/Engine/Core/SceneRegistry.h>
#include <Ludus/Engine/Persistence/IScenePersistence.h>
#include <Ludus/Engine/Persistence/LmlScenePersistence.h>
#include <Ludus/Engine/Runtime/PendingSceneTransition.h>
#include <Ludus/Engine/Runtime/SceneRuntimeState.h>
#include <Ludus/Engine/Runtime/SceneSystem.h>

namespace Ludus::Engine::Runtime
{
	namespace
	{
		std::optional<std::filesystem::path> ResolveTransition(const PendingSceneTransition& pendingTransition)
		{
			return std::visit([&](auto&& value) -> std::optional<std::filesystem::path>
			{
				using Alt = std::decay_t<decltype(value)>;

				if constexpr (std::is_same_v<Alt, PendingSceneTransition::Load>)
				{
					return value.Path;
				}
				else
				{
					return std::nullopt;
				}
			}, pendingTransition.Data);
		}
	}

	SceneSystem::SceneSystem(
		Ludus::Engine::Core::SceneRegistry& sceneRegistry,
		Ludus::Engine::Runtime::SceneRuntimeState& sceneRuntimeState
	) :
		SceneSystem(
			sceneRegistry,
			sceneRuntimeState,
			std::make_unique<Ludus::Engine::Persistence::LmlScenePersistence>()
		)
	{ }

	SceneSystem::SceneSystem(
		Ludus::Engine::Core::SceneRegistry& sceneRegistry,
		Ludus::Engine::Runtime::SceneRuntimeState& sceneRuntimeState,
		std::unique_ptr<Ludus::Engine::Persistence::IScenePersistence> scenePersistence
	) :
		m_SceneRegistry(sceneRegistry),
		m_SceneRuntimeState(sceneRuntimeState),
		m_ScenePersistence(std::move(scenePersistence))
	{ }

	SceneSystem::~SceneSystem() = default;

	void SceneSystem::BeginFrameImpl()
	{
		auto path = ResolveTransition(m_SceneRuntimeState.PendingTransition);
		if (!path)
		{
			return;
		}

		auto scene = m_ScenePersistence->Load(*path);
		m_SceneRegistry.Clear();
		const auto sceneId = m_SceneRegistry.AddScene(std::move(scene));

		m_SceneRuntimeState.PendingTransition = Ludus::Engine::Runtime::PendingSceneTransition::NoneState();
		m_SceneRuntimeState.Presentation.CurrentSceneId = sceneId;
	}
}
