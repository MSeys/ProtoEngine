#include "ProtoEnginePCH.h"

#if(_DEBUG)
// ReSharper disable once CppUnusedIncludeDirective
#include <vld.h>
#endif

#include "Engine.h"
#include "SDL.h"

#include "EditorGame.h"
#include "SimpleGame.h"

int main(int, char* [])
{
	BaseGame* pGame{};
	RenderSettings renderSettings{};
	WindowSettings windowSettings{};

#ifdef _DEBUG
	renderSettings.RenderMode = RenderMode::EDITOR;
	pGame = new EditorGame();
	windowSettings.Title = "Editor";
#else
	renderSettings.RenderMode = RenderMode::GAME;
	pGame = new SimpleGame();
	windowSettings.Title = "SimpleGame";
#endif

	windowSettings.WindowSize = { 640, 480 };
	windowSettings.FPSState = FPSState::PROTO_CAPPED;
	windowSettings.FPSRate = FPSRate::PROTO_FPS_VERY_HIGH;
	
	renderSettings.WindowSizeOffset = { 1920 - windowSettings.WindowSize.x, 1080 - windowSettings.WindowSize.y };
	renderSettings.GameRenderOffset = { 1920 / 3 , 50 };
	
	const Proto::Engine engine{ pGame, renderSettings, windowSettings };
	
	engine.Run();
	return 0;
}

// TODO: List of features to make
// Editor Camera	> glm::vec2 offset, changed in editor mode, reset on play.
// Game Camera		> Separate GameObject in Scene? Has target gameobject that can be given using ID?
// Unique ID		> int > Given on Start / file, Max found ID + 1 (Stored in ProtoSettings?)
// Edit / Play mode > On play -> call Awake (which might include controls?)
//					> On Edit -> reload scene (make everything reset correctly)
// Find GameObject with ID
// When to Awake? Fix!