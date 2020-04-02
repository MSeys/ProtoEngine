#include "ProtoEnginePCH.h"

#include "SimpleGame.h"

#include "StartScene.h"

void SimpleGame::Initialize()
{
	ProtoScenes.AddGameScene(new StartScene(L"StartScene"));
}
