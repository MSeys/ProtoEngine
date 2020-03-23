#include "ProtoEnginePCH.h"

#include "BaseGame.h"

BaseGame::BaseGame(std::string gameName, unsigned int maxFPS)
	: m_GameName(std::move(gameName)), m_MaxFPS(maxFPS)
{
}

const std::string& BaseGame::GetGameName() const
{
	return m_GameName;
}

const unsigned& BaseGame::GetMaxFPS() const
{
	return m_MaxFPS;
}
