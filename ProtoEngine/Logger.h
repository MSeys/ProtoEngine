#pragma once
#include <array>

#include "Singleton.h"
#include <iomanip>
#include <vector>

constexpr double LOGGER_VERSION = 1.6;

enum class LogLevel
{
	Info = 1, Trace = 2, Debug = 3, Warning = 4, Error = 5, Achievement = 6
};

struct Log
{
	LogLevel Level;
	std::string Text;
	
	bool operator==(const Log& other) const;
	std::string ToText() const;
};

namespace Proto
{
	class Logger final : public Singleton<Logger>
	{
	public:
		void AddLog(const LogLevel& logLevel, const std::string& logText);
		void Draw();

	private:
		std::array<std::vector<Log>, 7> m_Logs;
		
		bool m_AutoScroll{ true };
	};
}
