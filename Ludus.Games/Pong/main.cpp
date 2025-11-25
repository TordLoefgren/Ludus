#include <Ludus/Core/Application.h>
#include <Ludus/Core/Phase.h>
#include <Ludus/Core/State.h>
#include <Ludus/Core/SystemPredicate.h>
#include <Ludus/Graphics/Color.h>
#include <Ludus/Platform/Window.h>
#include <Ludus/Platform/WindowOptions.h>

#include <Pong/Core/GameInfo.h>
#include <Pong/Core/GameState.h>
#include <Pong/Core/PongInfo.h>
#include <Pong/Systems/MainMenuSystem.h>
#include <Pong/Systems/PauseMenuSystem.h>
#include <Pong/Systems/PlayingSystem.h>
#include <Pong/Systems/ScoreMenuSystem.h>

int main()
{
	auto windowOptions = Ludus::Platform::WindowOptions(1024, 768, "Pong (1972)", false);
	auto renderingOptions = Ludus::Graphics::RenderingOptions(Ludus::Graphics::Colors::Black);
	auto application = Ludus::Core::Application::Create(windowOptions, renderingOptions);

	auto gameState = Ludus::Core::State<Pong::Core::GameState>(Pong::Core::GameState::MainMenu);
	application->AddResource(std::move(gameState));

	auto gameInfo = Pong::Core::GameInfo();
	auto pongInfo = Pong::Core::PongInfo();

	application->AddSystem(
		Ludus::Core::Phase::Update,
		std::make_unique<Pong::Systems::MainMenuSystem>(gameInfo, pongInfo),
		Ludus::Core::RunIfInState<Pong::Core::GameState>(Pong::Core::GameState::MainMenu)
	);

	application->AddSystem(
		Ludus::Core::Phase::Update,
		std::make_unique<Pong::Systems::PauseMenuSystem>(gameInfo, pongInfo),
		Ludus::Core::RunIfInState<Pong::Core::GameState>(Pong::Core::GameState::PauseMenu)
	);

	application->AddSystem(
		Ludus::Core::Phase::Update,
		std::make_unique<Pong::Systems::PlayingSystem>(gameInfo, pongInfo),
		Ludus::Core::RunIfInState<Pong::Core::GameState>(Pong::Core::GameState::Playing)
	);

	application->AddSystem(
		Ludus::Core::Phase::Update,
		std::make_unique<Pong::Systems::ScoreMenuSystem>(gameInfo, pongInfo),
		Ludus::Core::RunIfInState<Pong::Core::GameState>(Pong::Core::GameState::ScoreMenu)
	);

	application->Run();

	return 0;
}
