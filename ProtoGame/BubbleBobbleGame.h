#pragma once
#include "BaseGame.h"

class BubbleBobbleGame : public BaseGame
{
public:
	void InitializeScene() override;

	void DrawAddComponent(GameObject* pCurrentSelected) override;
	void LoadComponents(rapidxml::xml_node<>* pComponents, GameObject* pCurr) override;
};