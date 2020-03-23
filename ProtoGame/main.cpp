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
	Proto::Engine engine{ new SimpleGame("SimpleGame", 60) };
#endif
	
	engine.Run();
	return 0;
}