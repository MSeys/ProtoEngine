#pragma once
#include "BaseGame.h"
class SimpleGame : public BaseGame
{
public:
	void InitializeCommands() override;
	void InitializeControls() override;
	void InitializeScene() override;

	void DrawAddComponent() override;
	void LoadComponents(rapidxml::xml_node<>* pComponents, GameObject* pCurr) override;
	
private:
	std::string m_JumpDataExample{ "Boing!" };
	AudioServices m_AudioServices{};
};