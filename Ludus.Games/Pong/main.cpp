#include <Ludus/Graphics/GLContext.h>
#include <Ludus/Platform/Window.h>
#include <Ludus/Platform/WindowOptions.h>

#include <Pong/Core/GameInfo.h>
#include <Pong/Core/PongInfo.h>
#include <Pong/States/GameState.h>
#include <Pong/States/MainMenuState.h>
#include <Pong/States/PauseMenuState.h>
#include <Pong/States/PlayingState.h>
#include <Pong/States/ScoreMenuState.h>

const int WIDTH = 1024;
const int HEIGHT = 768;

Pong::States::GameState GameState = Pong::States::GameState::MainMenu;

int main()
{

#pragma region Initialization

	auto windowOptions = Ludus::Platform::WindowOptions(WIDTH, HEIGHT, "Pong (1972)", false);
	auto window = Ludus::Platform::Window(windowOptions);

	Ludus::Graphics::GLContext::Init();
	Ludus::Graphics::GLContext::EnableBlending();
	Ludus::Graphics::GLContext::SetBlendAlpha();

	Pong::Core::GameInfo gameInfo(window, windowOptions);
	Pong::Core::PongInfo pongInfo;

	auto menuState = Pong::States::MainMenuState(gameInfo, pongInfo);
	auto pauseMenuState = Pong::States::PauseMenuState(gameInfo, pongInfo);
	auto playingState = Pong::States::PlayingState(gameInfo, pongInfo);
	auto scoreMenuState = Pong::States::ScoreMenuState(gameInfo, pongInfo);

	menuState.Init();
	pauseMenuState.Init();
	playingState.Init();
	scoreMenuState.Init();

#pragma endregion

	while (!window.WindowShouldClose())
	{
		gameInfo.Timer.Step();

		const auto stateAtFrameStart = GameState;
		auto nextState = GameState;

		// Input Handling.
		auto& input = window.GetInput();

		switch (stateAtFrameStart)
		{
			case Pong::States::GameState::Exit:
				nextState = Pong::States::GameState::Exit;
				break;
			case Pong::States::GameState::MainMenu:
				nextState = menuState.HandleInput(input, gameInfo.Timer);
				break;
			case Pong::States::GameState::Paused:
				nextState = pauseMenuState.HandleInput(input, gameInfo.Timer);
				break;
			case Pong::States::GameState::Playing:
				nextState = playingState.HandleInput(input, gameInfo.Timer);
				break;
			case Pong::States::GameState::Score:
				nextState = scoreMenuState.HandleInput(input, gameInfo.Timer);
				break;
			default:
				nextState = Pong::States::GameState::Exit;
				break;
		}

		// Movement Integration / Collision Handling.
		if (stateAtFrameStart == Pong::States::GameState::Playing && nextState == stateAtFrameStart)
		{
			nextState = playingState.Update(gameInfo.Timer);
		}

		// Rendering.
		gameInfo.Renderer.BeginScene(gameInfo.Camera);
		gameInfo.Renderer.Clear();

		switch (stateAtFrameStart)
		{
			case Pong::States::GameState::MainMenu:
				menuState.Render(gameInfo.Timer);
				break;
			case Pong::States::GameState::Paused:
				pauseMenuState.Render(gameInfo.Timer);
				break;
			case Pong::States::GameState::Playing:
				playingState.Render(gameInfo.Timer);
				break;
			case Pong::States::GameState::Score:
				scoreMenuState.Render(gameInfo.Timer);
				break;
			default:
				break;
		}

		gameInfo.Renderer.EndScene();

		GameState = nextState;
		if (GameState == Pong::States::GameState::Exit)
		{
			window.SetWindowShouldClose();
		}

		window.SwapBuffers();
		window.PollEvents();
	}

	return 0;
}
