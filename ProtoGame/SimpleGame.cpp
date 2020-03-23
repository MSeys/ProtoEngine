#include "ProtoEnginePCH.h"

#include "SimpleGame.h"

#include "StartScene.h"

SimpleGame::SimpleGame(const std::string& gameName, unsigned int maxFPS)
	: BaseGame(gameName, maxFPS)
{
}

void SimpleGame::Initialize()
{
	ProtoScenes.AddGameScene(new StartScene(L"StartScene"));
}
