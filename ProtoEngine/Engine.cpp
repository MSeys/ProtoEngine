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
	bool exit{ false };

	ProtoTime.SetStartTime(std::chrono::steady_clock::now());
	while(!exit)
	{
		ProtoTime.SetCurrTime(std::chrono::steady_clock::now());
		ProtoTime.UpdateTime();
		ProtoTime.SetStartTime(ProtoTime.GetCurrTime());
		
		exit = !ProtoInput.ProcessInput(); // temporary
		ProtoScenes.Update();
		ProtoScenes.Draw();
	}
}

void Proto::Engine::Initialize()
{
	if(SDL_Init(SDL_INIT_VIDEO) != 0)
		throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());

	m_Window = SDL_CreateWindow(
		m_WindowSettings.title.c_str(),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		m_WindowSettings.windowWidth,
		m_WindowSettings.windowHeight,
		SDL_WINDOW_OPENGL
	);

	if(!m_Window)
		throw std::runtime_error(std::string("SDL_CreateWindow Error: ") + SDL_GetError());

	ProtoRenderer.Init(m_Window);
	ProtoResources.Init("../Data/");
}

void Proto::Engine::Cleanup()
{
	ProtoRenderer.Destroy();
	SafeDelete(m_pGame);
	
	SDL_DestroyWindow(m_Window);
	m_Window = nullptr;
	SDL_Quit();
}