#pragma once
#include "BaseGame.h"
class SimpleGame : public BaseGame
{
public:
	void InitializeCommands() override;
	void InitializeControls() override;
	void InitializeScene() override;

private:
	std::string m_JumpDataExample{ "Boing!" };
	AudioServices m_AudioServices{};
};

