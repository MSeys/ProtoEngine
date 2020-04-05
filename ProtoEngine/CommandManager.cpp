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
		ProtoLogger.AddLog(LogLevel::Warning, "CommandManager::AddCommand failed > CommandID is already used.");
		SafeDelete(command);
	 	
		return false;
	 }

	 m_Commands[commandID] = command;
	 return true;
}

Command& Proto::CommandManager::GetCommand(const std::string& commandID) const
{
	try
	{
		return *m_Commands.at(commandID);
	}

	catch(const std::out_of_range& exception)
	{
		UNREFERENCED_PARAMETER(exception);

		ProtoLogger.AddLog(LogLevel::Warning, "CommandManager::GetCommand failed > Command not found. Default Command ran.");
		return *m_Commands.at(COMMAND_DEFAULT);
	}
}

void Proto::CommandManager::ForceExit()
{
	m_Commands.at(COMMAND_EXIT)->Execute();
}

void Proto::CommandManager::ResetInputData()
{
	for (std::pair<std::string, Command*> pair : m_Commands)
	{
		pair.second->SetInputData(CommandOrigin::NONE, "NONE", nullptr);
	}
}
