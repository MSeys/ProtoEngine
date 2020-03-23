#pragma once
#include "ProtoEnginePCH.h"

class BaseGame
{
public:
	BaseGame(std::string gameName, unsigned int maxFPS = 60);
	virtual void Initialize() = 0;
	virtual ~BaseGame() = default;

	BaseGame(const BaseGame& other) = delete;
	BaseGame(BaseGame&& other) = delete;
	BaseGame& operator=(const BaseGame& other) = delete;
	BaseGame& operator=(BaseGame&& other) = delete;

	const std::string& GetGameName() const;
	const unsigned int& GetMaxFPS() const;
	
private:
	std::string m_GameName;
	unsigned int m_MaxFPS;
};