#pragma once
#include <unordered_map>

#include "BaseCommand.h"
#include "Singleton.h"

constexpr auto COMMAND_DEFAULT = "DEFAULT_C";
constexpr auto COMMAND_EXIT = "EXIT_C";

namespace Proto
{
	class CommandManager final : public Singleton<CommandManager>
	{
	public:
		void Init();
		void Destroy();
		
		bool AddCommand(Command* command, const std::string& commandID);
		Command* GetCommand(const std::string& commandID) const;

		void ForceExit();
		void ResetInputData();
		

	private:
		std::unordered_map<std::string, Command*> m_Commands;
	};
}