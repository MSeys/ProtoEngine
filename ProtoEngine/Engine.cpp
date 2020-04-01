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
	
	m_pGame->Initialize();
	ProtoTime.SetMaxFPS(m_pGame->GetMaxFPS());
}

Proto::Engine::~Engine()
{
	Cleanup();
}

void Proto::Engine::Run()
{
	ProtoTime.SetStartTime(std::chrono::steady_clock::now());
	while(!m_Exit)
	{
		ProtoTime.SetCurrTime(std::chrono::steady_clock::now());
		ProtoTime.UpdateTime();
		ProtoTime.SetStartTime(ProtoTime.GetCurrTime());

		ProtoInput.UpdateStates();
		ProtoInput.ProcessStates();
		
		ProtoScenes.Update();
		ProtoScenes.Draw();

		ProtoCommands.ResetInputData();
	}
}

void Proto::Engine::Initialize()
{
	InitializeSDL();
	InitializeEngineParts();

	

	ProtoInput.AddKey(SDLK_ESCAPE);
	ProtoInput.GetKey(SDLK_ESCAPE)->SetCommand(ButtonState::Pressed, COMMAND_EXIT);
	ProtoCommands.GetCommand(COMMAND_EXIT)->SetExecuteData(&m_Exit);
}

void Proto::Engine::InitializeSDL()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());

	m_Window = SDL_CreateWindow(
		m_WindowSettings.title.c_str(),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		m_WindowSettings.windowWidth,
		m_WindowSettings.windowHeight,
		SDL_WINDOW_OPENGL
	);

	if (!m_Window)
		throw std::runtime_error(std::string("SDL_CreateWindow Error: ") + SDL_GetError());
}

void Proto::Engine::InitializeEngineParts() const
{
	// ProtoRenderer
	ProtoRenderer.Init(m_Window);

	// ProtoInput
	ProtoInput.Init();

	// ProtoCommands
	ProtoCommands.Init();
	
	// ProtoResource
	ProtoResources.Init("../Data/");

	// ProtoAudio
	ProtoAudio::Init();
}

void Proto::Engine::Cleanup()
{
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
	
	SafeDelete(m_pGame);
}
