#include "ProtoEnginePCH.h"

#include "Engine.h"

#include <chrono>
#include <thread>
#include <SDL.h>

#include <utility>

#include "BaseGame.h"

Proto::Engine::Engine(BaseGame* pGame, WindowSettings windowSettings)
	: m_pGame(pGame), m_WindowSettings(std::move(windowSettings))
{
	Initialize();
}

Proto::Engine::~Engine()
{
	Cleanup();
}

void Proto::Engine::Run() const
{
	ProtoTime.SetStartTime(std::chrono::steady_clock::now());
	while(!m_Exit)
	{
		ProtoTime.SetCurrTime(std::chrono::steady_clock::now());
		ProtoTime.UpdateTime();
		ProtoTime.SetStartTime(ProtoTime.GetCurrTime());

		ProtoInput.Update();
		ProtoInput.Process();
		
		ProtoScenes.Update();
		ProtoScenes.Draw();

		ProtoCommands.ResetInputData();
	}
}

void Proto::Engine::Initialize()
{
	InitializeSDL();
	InitializeEngineParts();

	// Game
	m_pGame->Initialize();
}

void Proto::Engine::InitializeSDL()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());

	m_Window = SDL_CreateWindow(
		m_WindowSettings.Title.c_str(),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		m_WindowSettings.WindowWidth,
		m_WindowSettings.WindowHeight,
		SDL_WINDOW_OPENGL
	);

	if (!m_Window)
		throw std::runtime_error(std::string("SDL_CreateWindow Error: ") + SDL_GetError());
}

void Proto::Engine::InitializeEngineParts()
{
	// ProtoSettings
	ProtoSettings.Init(m_WindowSettings);
	
	// ProtoRenderer
	ProtoRenderer.Init(m_Window);

	// ProtoInput
	ProtoInput.Init();
	ProtoInput.AddKey(SDLK_ESCAPE);
	ProtoInput.GetKey(SDLK_ESCAPE).SetCommand(ButtonState::Pressed, COMMAND_EXIT);
	
	// ProtoCommands
	ProtoCommands.Init();
	ProtoCommands.GetCommand(COMMAND_EXIT)->SetExecuteData(&m_Exit);
	
	// ProtoResource
	ProtoResources.Init("../Data/");

	// ProtoAudio
	ProtoAudio::Init();
}

void Proto::Engine::Cleanup()
{
	// Game
	SafeDelete(m_pGame);
	
	CleanupEngineParts();
	CleanupSDL();
}

void Proto::Engine::CleanupSDL()
{
	SDL_DestroyWindow(m_Window);
	m_Window = nullptr;
	SDL_Quit();
}

void Proto::Engine::CleanupEngineParts()
{
	// ProtoRenderer
	ProtoRenderer.Destroy();

	// ProtoInput
	ProtoInput.Destroy();

	// ProtoCommands
	ProtoCommands.Destroy();
}
