#include "ProtoEnginePCH.h"
#include "CommandManager.h"

#include <iostream>

void Proto::CommandManager::Init()
{
	m_Commands[COMMAND_DEFAULT] = new DefaultCommand(); // Add default command | Useful for Debugging
	m_Commands[COMMAND_EXIT] = new ExitCommand(); // Add Exit command | Basic bool exit handling | Used in Force Exit for SDL_QUIT Event
}

void Proto::CommandManager::Destroy()
{
	for (std::pair<std::string, Command*> pair : m_Commands)
		SafeDelete(pair.second);
}

bool Proto::CommandManager::AddCommand(Command* command, const std::string& commandID)
{
	 if(m_Commands.find(commandID) != m_Commands.end())
	 {
		std::cout << "CommandManager::AddCommand failed > CommandID is already used." << std::endl; // TODO: Replace by Logger
		SafeDelete(command);
	 	
		return false;
	 }

	 m_Commands[commandID] = command;
	 return true;
}

Command* Proto::CommandManager::GetCommand(const std::string& commandID) const
{
	if (m_Commands.find(commandID) != m_Commands.end())
	{
		return m_Commands.at(commandID);
	}

	std::cout << "CommandManager::GetCommand failed > Command not found. (NullPointerException)" << std::endl; // TODO: Replace by Logger
	return nullptr;
}

void Proto::CommandManager::ForceExit()
{
	m_Commands.at(COMMAND_EXIT)->Execute();
}

void Proto::CommandManager::ResetInputData()
{
	for (std::pair<std::string, Command*> pair : m_Commands)
	{
		pair.second->SetInputData(nullptr);
	}
}
