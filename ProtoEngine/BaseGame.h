#pragma once
#include "ProtoEnginePCH.h"

#include "RapidXML/rapidxml.hpp"
#include "GameObject.h"

class BaseGame
{
public:
	virtual ~BaseGame() = default;
	
	void Initialize();
	
	virtual void DrawAddComponent() {}
	virtual void LoadComponents(rapidxml::xml_node<>* pComponents, GameObject* pCurr)
	{
		UNREFERENCED_PARAMETER(pComponents);
		UNREFERENCED_PARAMETER(pCurr);
	}
	
protected:
	virtual void InitializeCommands() = 0;
	virtual void InitializeControls() = 0;
	virtual void InitializeScene() = 0;
};

inline void BaseGame::Initialize()
{
	InitializeCommands();
	InitializeControls();
	InitializeScene();
}