#pragma once
#include "ProtoEnginePCH.h"

class BaseGame
{
public:
	virtual ~BaseGame() = default;
	
	void RootInitialize();
	
	virtual void InitializeCommands() = 0;
	virtual void InitializeControls() = 0;
	virtual void InitializeScene() = 0;
};

inline void BaseGame::RootInitialize()
{
	InitializeCommands();
	InitializeControls();
	InitializeScene();
}