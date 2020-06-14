#include "ProtoEnginePCH.h"

#if(_DEBUG)
// ReSharper disable once CppUnusedIncludeDirective
#include <vld.h>
#endif

#include "BubbleBobbleGame.h"
#include "Engine.h"
#include "SDL.h"

int main(int, char* [])
{
	BaseGame* pGame{};
	BaseGame* pRefGame{ new BubbleBobbleGame() };
	EditorSettings editorSettings;
	WindowSettings windowSettings;

#ifdef _WIN64
	pGame = pRefGame;
	windowSettings.Title = "Bubble Bobble";
#endif

	windowSettings.GameWindowSize = { 640, 640 };
	windowSettings.FPSState = FPSState::PROTO_CAPPED;
	windowSettings.FPSRate = FPSRate::PROTO_FPS_VERY_HIGH;

	editorSettings.EditorWindowSize = { 1920, 1080 };
	
	const Proto::Engine engine{ pGame, pRefGame, editorSettings, windowSettings };

	engine.Run();
	return 0;
}

// TODO: fix AddChild | GameObject ID (resets??? Doesn't re-obtain highest ID???)