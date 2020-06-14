#pragma once
#include <unordered_map>

#include "BaseCommand.h"
#include "Singleton.h"

constexpr auto COMMAND_DEFAULT = "C_DEFAULT";

namespace Proto
{
	class CommandManager final : public Singleton<CommandManager>
	{
	public:
		void Init();
		void Destroy();

		void Reset();
		
		bool AddCommand(Command* command, const std::string& commandID);
		Command& GetCommand(const std::string& commandID) const;
		
		void ForceExit() const;
		void ResetInputData();
		
	private:
		friend class Engine;
		Command& GetExitCommand() const { return *m_pExitCommand; }
		
		std::unordered_map<std::string, Command*> m_Commands;
		Command* m_pExitCommand{};
	};
}