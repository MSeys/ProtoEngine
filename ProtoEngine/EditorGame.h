#pragma once
#include "BaseGame.h"

class EditorGame : public BaseGame
{
public:
	void InitializeCommands() override {}
	void InitializeControls() override {}
	void InitializeScene() override;
};
