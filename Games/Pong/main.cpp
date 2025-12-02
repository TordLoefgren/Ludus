#include <Ludus/Core/Application.h>
#include <Ludus/Core/State.h>
#include <Ludus/Core/SystemPhase.h>
#include <Ludus/Core/SystemPredicate.h>
#include <Ludus/Graphics/Color.h>
#include <Ludus/Platform/Window.h>
#include <Ludus/Platform/WindowOptions.h>

#include <Ludus/Pong/Core/GameInfo.h>
#include <Ludus/Pong/Core/GameState.h>
#include <Ludus/Pong/Core/PongInfo.h>
#include <Ludus/Pong/Systems/MainMenuSystem.h>
#include <Ludus/Pong/Systems/PauseMenuSystem.h>
#include <Ludus/Pong/Systems/PlayingSystem.h>
#include <Ludus/Pong/Systems/ScoreMenuSystem.h>

int main()
{
	auto windowOptions = Ludus::Platform::WindowOptions(1024, 768, "Pong (1972)", false);
	auto renderingOptions = Ludus::Graphics::RenderingOptions(Ludus::Graphics::Colors::Black);
	auto application = Ludus::Core::Application::Create(windowOptions, renderingOptions);

	auto gameState = Ludus::Core::State<Ludus::Pong::Core::GameState>(Ludus::Pong::Core::GameState::MainMenu);
	application->AddResource(std::move(gameState));

	auto gameInfo = Ludus::Pong::Core::GameInfo();
	auto pongInfo = Ludus::Pong::Core::PongInfo();

	application->AddSystem(
		{
			Ludus::Core::SystemPhase::Update,
			Ludus::Core::RunIfInState<Ludus::Pong::Core::GameState>(Ludus::Pong::Core::GameState::MainMenu)
		},
		std::make_unique<Ludus::Pong::Systems::MainMenuSystem>(gameInfo, pongInfo)
	);

	application->AddSystem(
		{
			Ludus::Core::SystemPhase::Update,
			Ludus::Core::RunIfInState<Ludus::Pong::Core::GameState>(Ludus::Pong::Core::GameState::PauseMenu)
		},
		std::make_unique<Ludus::Pong::Systems::PauseMenuSystem>(gameInfo, pongInfo)
	);

	application->AddSystem(
		{
			Ludus::Core::SystemPhase::Update,
			Ludus::Core::RunIfInState<Ludus::Pong::Core::GameState>(Ludus::Pong::Core::GameState::Playing)
		},
		std::make_unique<Ludus::Pong::Systems::PlayingSystem>(gameInfo, pongInfo)
	);

	application->AddSystem(
		{
			Ludus::Core::SystemPhase::Update,
			Ludus::Core::RunIfInState<Ludus::Pong::Core::GameState>(Ludus::Pong::Core::GameState::ScoreMenu)
		},
		std::make_unique<Ludus::Pong::Systems::ScoreMenuSystem>(gameInfo, pongInfo)
	);

	application->Run();

	return 0;
}
