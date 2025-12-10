#include <memory>

#include <Ludus/Engine/Core/ApplicationBuilder.h>
#include <Ludus/Engine/Core/State.h>
#include <Ludus/Engine/Core/SystemPhase.h>
#include <Ludus/Engine/Graphics/Color.h>
#include <Ludus/Engine/Physics/Core/PhysicsContext2D.h>
#include <Ludus/Engine/Platform/Window.h>
#include <Ludus/Engine/Platform/WindowOptions.h>

#include <Ludus/Pong/Core/GameInfo.h>
#include <Ludus/Pong/Core/GameState.h>
#include <Ludus/Pong/Core/PongInfo.h>
#include <Ludus/Pong/Systems/MainMenuSystem.h>
#include <Ludus/Pong/Systems/PauseMenuSystem.h>
#include <Ludus/Pong/Systems/PlayingSystem.h>
#include <Ludus/Pong/Systems/ScoreMenuSystem.h>

int main()
{
	auto windowOptions = Ludus::Engine::Platform::WindowOptions(1024, 768, "Pong (1972)", false);
	auto renderingOptions = Ludus::Engine::Graphics::RenderingOptions(Ludus::Engine::Graphics::Colors::Black);
	auto physicsContext = Ludus::Engine::Physics::Core::PhysicsContext2D();

	auto gameInfo = std::make_shared<Ludus::Pong::Core::GameInfo>();
	auto pongInfo = std::make_shared<Ludus::Pong::Core::PongInfo>();
	auto gameState = Ludus::Engine::Core::State<Ludus::Pong::Core::GameState>(Ludus::Pong::Core::GameState::MainMenu);

	auto application = Ludus::Engine::Core::ApplicationBuilder::Create()
		.WithWindowOptions(windowOptions)
		.WithRenderingOptions(renderingOptions)
		.WithPhysicsContext(std::move(physicsContext))
		.UseDefaultPhysics2D()
		.UseDefaultRendering2D()
		.AddSystem<Ludus::Pong::Systems::MainMenuSystem>(
			{
				Ludus::Engine::Core::SystemPhase::Update,
				Ludus::Engine::Core::RunIfInState<Ludus::Pong::Core::GameState>(Ludus::Pong::Core::GameState::MainMenu)
			},
			gameInfo, pongInfo
		)
		.AddSystem<Ludus::Pong::Systems::PauseMenuSystem>(
			{
				Ludus::Engine::Core::SystemPhase::Update,
				Ludus::Engine::Core::RunIfInState<Ludus::Pong::Core::GameState>(Ludus::Pong::Core::GameState::PauseMenu)
			},
			gameInfo, pongInfo
		)
		.AddSystem<Ludus::Pong::Systems::PlayingSystem>(
			{
				Ludus::Engine::Core::SystemPhase::Update,
				Ludus::Engine::Core::RunIfInState<Ludus::Pong::Core::GameState>(Ludus::Pong::Core::GameState::Playing)
			},
			gameInfo, pongInfo
		)
		.AddSystem<Ludus::Pong::Systems::ScoreMenuSystem>(
			{
				Ludus::Engine::Core::SystemPhase::Update,
				Ludus::Engine::Core::RunIfInState<Ludus::Pong::Core::GameState>(Ludus::Pong::Core::GameState::ScoreMenu)
			},
			gameInfo, pongInfo
		)
		.AddResource<Ludus::Engine::Core::State<Ludus::Pong::Core::GameState>>(std::move(gameState))
		.Build();

	application->Run();

	return 0;
}
