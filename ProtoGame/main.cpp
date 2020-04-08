#include "ProtoEnginePCH.h"

#if(_DEBUG)
// ReSharper disable once CppUnusedIncludeDirective
#include <vld.h>
#endif

#include "Engine.h"
#include "SDL.h"

#define Simple

#ifdef Simple
#include "SimpleGame.h"
#endif

int main(int, char* [])
{
	BaseGame* pGame{};
	RenderSettings renderSettings{};
	WindowSettings windowSettings{};

#ifdef _DEBUG
	renderSettings.RenderMode = RenderMode::EDITOR;
#else
	renderSettings.RenderMode = RenderMode::GAME;
#endif
	
#ifdef Simple
	pGame = new SimpleGame();

	windowSettings.Title = "SimpleGame";
	windowSettings.WindowSize = { 640, 480 };
	windowSettings.FPSState = FPSState::PROTO_CAPPED;
	windowSettings.FPSRate = FPSRate::PROTO_FPS_VERY_HIGH;
	
	renderSettings.WindowSizeOffset = { 1920 - windowSettings.WindowSize.x, 1080 - windowSettings.WindowSize.y };
	renderSettings.GameRenderOffset = { 1920 / 3 , 50 };
#endif
	
	const Proto::Engine engine{ pGame, renderSettings, windowSettings };
	
	engine.Run();
	return 0;
}