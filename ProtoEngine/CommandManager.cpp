#include "ProtoEnginePCH.h"
#include "CommandManager.h"

#include <iostream>


Proto::CommandManager::CommandManager()
{
	m_Commands[COMMAND_DEFAULT] = new DefaultCommand(); // Add Default Command | Useful for Debugging Purposes
	m_Commands[COMMAND_EXIT] = new ExitCommand();
}

Proto::CommandManager::~CommandManager()
{
	for(std::pair<std::string, Command*> pair : m_Commands)
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

void Proto::CommandManager::ResetInputData()
{
	for (std::pair<std::string, Command*> pair : m_Commands)
	{
		pair.second->SetInputData(nullptr);
	}
}
