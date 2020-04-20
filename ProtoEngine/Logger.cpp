#include "Logger.h"

#include <string>

#include "ImGui/imgui.h"

bool Log::operator==(const Log& other) const
{
	return Level == other.Level && Text == other.Text;
}

std::string Log::ToText() const
{
	std::string logLevelString{};
	switch (Level)
	{
	case LogLevel::Info:
		logLevelString = "INFO";
		break;
	case LogLevel::Trace:
		logLevelString = "TRACE";
		break;
	case LogLevel::Debug:
		logLevelString = "DEBUG";
		break;
	case LogLevel::Warning:
		logLevelString = "WARNING";
		break;
	case LogLevel::Error:
		logLevelString = "ERROR";
		break;
	case LogLevel::Achievement:
		logLevelString = "ACHIEVEMENT";
		break;
	default:
		break;
	}

	return "[" + logLevelString + "] >> " + Text;
}

void Proto::Logger::AddLog(const LogLevel& logLevel, const std::string& logText)
{
	m_Logs[0].push_back(Log{ logLevel, logText }); // ALL Logger
	m_Logs[int(logLevel)].push_back(Log{ logLevel, logText });
}

void Proto::Logger::Draw()
{
	const std::string title{ "ProtoLogger V" + std::to_string(LOGGER_VERSION).substr(0, 3) };

	ImGui::Begin(title.c_str());

	ImGui::Checkbox("Auto-scroll", &m_AutoScroll);
	ImGui::SameLine();

	if (ImGui::Button("Clear"))
	{
		for (std::vector<Log>& logContainer : m_Logs)
			logContainer.clear();
	}
	
	ImGui::SameLine();
	static int currentID = 0;
	ImGui::Combo("Logger Type Filter", &currentID, "All\0Info\0Trace\0Debug\0Warning\0Error\0Achievement\0");

	ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

	for (const Log& log : m_Logs[currentID])
		ImGui::Text(log.ToText().c_str());

	ImGui::PopStyleVar();

	if (m_AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
		ImGui::SetScrollHereY(1.0f);

	ImGui::EndChild();

	ImGui::End();
}
