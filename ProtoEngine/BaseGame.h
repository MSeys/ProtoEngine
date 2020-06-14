#pragma once
#include "ProtoEnginePCH.h"

#include "RapidXML/rapidxml.hpp"
#include "GameObject.h"

class BaseGame
{
public:
	virtual ~BaseGame() = default;
	
	void Initialize();
	
	virtual void DrawAddComponent(GameObject* pCurrentSelected) { UNREFERENCED_PARAMETER(pCurrentSelected); }
	virtual void LoadComponents(rapidxml::xml_node<>* pComponents, GameObject* pCurr)
	{
		UNREFERENCED_PARAMETER(pComponents);
		UNREFERENCED_PARAMETER(pCurr);
	}
	
protected:
	virtual void InitializeScene() = 0;
};

inline void BaseGame::Initialize()
{
	InitializeScene();
}