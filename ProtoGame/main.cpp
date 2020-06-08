#include "ProtoEnginePCH.h"

#if(_DEBUG)
// ReSharper disable once CppUnusedIncludeDirective
#include <vld.h>
#endif

#include "Engine.h"
#include "SDL.h"

#include "SimpleGame.h"

int main(int, char* [])
{
	BaseGame* pGame{};
	BaseGame* pRefGame{ new SimpleGame() };
	EditorSettings editorSettings;
	WindowSettings windowSettings;

#ifndef _DEBUG
#ifdef _WIN64
	pGame = pRefGame;
	windowSettings.Title = "SimpleGame";
#endif
#endif

	windowSettings.GameWindowSize = { 640, 480 };
	windowSettings.FPSState = FPSState::PROTO_CAPPED;
	windowSettings.FPSRate = FPSRate::PROTO_FPS_VERY_HIGH;

	editorSettings.EditorWindowSize = { 1920, 1080 };
	
	const Proto::Engine engine{ pGame, pRefGame, editorSettings, windowSettings };

	engine.Run();
	return 0;
}

// TODO: List of features to make
// SpriteSheet Component + Animations (A day work)