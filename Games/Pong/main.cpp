#include <Ludus/Engine/Core/Application.h>
#include <Ludus/Engine/Core/State.h>
#include <Ludus/Engine/Core/SystemPhase.h>
#include <Ludus/Engine/Core/SystemPredicate.h>
#include <Ludus/Engine/Graphics/Color.h>
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
	auto application = Ludus::Engine::Core::Application::Create(windowOptions, renderingOptions);

	auto gameState = Ludus::Engine::Core::State<Ludus::Pong::Core::GameState>(Ludus::Pong::Core::GameState::MainMenu);
	application->AddResource(std::move(gameState));

	auto gameInfo = Ludus::Pong::Core::GameInfo();
	auto pongInfo = Ludus::Pong::Core::PongInfo();

	application->AddSystem(
		{
			Ludus::Engine::Core::SystemPhase::Update,
			Ludus::Engine::Core::RunIfInState<Ludus::Pong::Core::GameState>(Ludus::Pong::Core::GameState::MainMenu)
		},
		std::make_unique<Ludus::Pong::Systems::MainMenuSystem>(gameInfo, pongInfo)
	);

	application->AddSystem(
		{
			Ludus::Engine::Core::SystemPhase::Update,
			Ludus::Engine::Core::RunIfInState<Ludus::Pong::Core::GameState>(Ludus::Pong::Core::GameState::PauseMenu)
		},
		std::make_unique<Ludus::Pong::Systems::PauseMenuSystem>(gameInfo, pongInfo)
	);

	application->AddSystem(
		{
			Ludus::Engine::Core::SystemPhase::Update,
			Ludus::Engine::Core::RunIfInState<Ludus::Pong::Core::GameState>(Ludus::Pong::Core::GameState::Playing)
		},
		std::make_unique<Ludus::Pong::Systems::PlayingSystem>(gameInfo, pongInfo)
	);

	application->AddSystem(
		{
			Ludus::Engine::Core::SystemPhase::Update,
			Ludus::Engine::Core::RunIfInState<Ludus::Pong::Core::GameState>(Ludus::Pong::Core::GameState::ScoreMenu)
		},
		std::make_unique<Ludus::Pong::Systems::ScoreMenuSystem>(gameInfo, pongInfo)
	);

	application->Run();

	return 0;
}
