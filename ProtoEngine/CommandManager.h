#pragma once
#include <unordered_map>

#include "BaseCommand.h"
#include "Singleton.h"

namespace Proto
{
	class CommandManager final : public Proto::Singleton<CommandManager>
	{
	public:
		CommandManager();
		~CommandManager();

		bool AddCommand(Command* command, const std::string& commandID);
		Command* GetCommand(const std::string& commandID) const;
		void ResetInputData();

	private:
		std::unordered_map<std::string, Command*> m_Commands;
	};
}