#pragma once
#include "BaseGame.h"
class SimpleGame : public BaseGame
{
public:
	SimpleGame(const std::string& gameName, unsigned int maxFPS);

	void Initialize() override;
};

