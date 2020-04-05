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
#ifdef Simple
	const Proto::Engine engine{ new SimpleGame(), WindowSettings{ "SimpleGame", 640, 680, FPSState::PROTO_CAPPED, FPSRate::PROTO_FPS_VERY_HIGH } };
#endif
	
	engine.Run();
	return 0;
}