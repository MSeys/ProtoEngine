#pragma once
#include "ProtoEnginePCH.h"

class BaseGame
{
public:
	virtual ~BaseGame() = default;
	virtual void Initialize() = 0;
};