#pragma once
#include <array>

#include "ImGui/imgui.h"
#include "Singleton.h"
#include <iomanip>

constexpr double LOGGER_VERSION = 1.5;

enum class LogLevel
{
	Info = 1, Trace = 2, Debug = 3, Warning = 4, Error = 5, Achievement = 6
};

struct Log
{
	LogLevel Level;
	std::string Text;
	
	bool operator==(const Log& other) const
	{
		return Level == other.Level && Text == other.Text;
	}

	std::string ToText() const
	{
		std::string logLevelString{};
		switch(Level)
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
};

namespace Proto
{
	class Logger final : public Singleton<Logger>
	{
	public:
		void AddLog(const LogLevel& logLevel, const std::string& logText)
		{
			m_Logs[0].push_back(Log{ logLevel, logText }); // ALL Logger

			m_Logs[int(logLevel)].push_back(Log{ logLevel, logText });
		}

		void Draw()
		{
			const std::string title{ "ProtoLogger V" + std::to_string(LOGGER_VERSION).substr(0, 3) };

			ImGuiWindowFlags flags = 0;
			flags |= ImGuiWindowFlags_NoMove;
			flags |= ImGuiWindowFlags_NoResize;
			flags |= ImGuiWindowFlags_NoCollapse;

			ImGui::SetNextWindowPos(ImVec2(0, 480), ImGuiCond_Always);
			ImGui::SetNextWindowSize(ImVec2(640, 200), ImGuiCond_Always);
			
			ImGui::Begin(title.c_str(), nullptr, flags);
			
			if(ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
			{
				if(ImGui::BeginTabItem("All"))
				{
					ImGui::BeginChild("scrolling##ALL", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
					
					for (const Log& log : m_Logs[0])
						ImGui::TextColored(m_LogColors[int(log.Level) - 1], log.ToText().c_str());
			
					ImGui::PopStyleVar();
			
					if (m_AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
						ImGui::SetScrollHereY(1.0f);
			
					ImGui::EndChild();
					ImGui::EndTabItem();
				}
			
				if (ImGui::BeginTabItem("Info"))
				{
					ImGui::BeginChild("scrolling##INFO", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				
					for (const Log& log : m_Logs[int(LogLevel::Info)])
						ImGui::TextColored(m_LogColors[int(log.Level) - 1], log.ToText().c_str());
				
					ImGui::PopStyleVar();
				
					if (m_AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
						ImGui::SetScrollHereY(1.0f);
				
					ImGui::EndChild();
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Trace"))
				{
					ImGui::BeginChild("scrolling##TRACE", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

					for (const Log& log : m_Logs[int(LogLevel::Trace)])
						ImGui::TextColored(m_LogColors[int(log.Level) - 1], log.ToText().c_str());

					ImGui::PopStyleVar();

					if (m_AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
						ImGui::SetScrollHereY(1.0f);

					ImGui::EndChild();
					ImGui::EndTabItem();
				}
				
				if (ImGui::BeginTabItem("Debug"))
				{
					ImGui::BeginChild("scrolling##DEBUG", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				
					for (const Log& log : m_Logs[int(LogLevel::Debug)])
						ImGui::TextColored(m_LogColors[int(log.Level) - 1], log.ToText().c_str());
				
					ImGui::PopStyleVar();
				
					if (m_AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
						ImGui::SetScrollHereY(1.0f);
				
					ImGui::EndChild();
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Warning"))
				{
					ImGui::BeginChild("scrolling##Warning", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

					for (const Log& log : m_Logs[int(LogLevel::Warning)])
						ImGui::TextColored(m_LogColors[int(log.Level) - 1], log.ToText().c_str());

					ImGui::PopStyleVar();

					if (m_AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
						ImGui::SetScrollHereY(1.0f);

					ImGui::EndChild();
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Error"))
				{
					ImGui::BeginChild("scrolling##ERROR", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

					for (const Log& log : m_Logs[int(LogLevel::Error)])
						ImGui::TextColored(m_LogColors[int(log.Level) - 1], log.ToText().c_str());

					ImGui::PopStyleVar();

					if (m_AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
						ImGui::SetScrollHereY(1.0f);

					ImGui::EndChild();
					ImGui::EndTabItem();
				}

				if (ImGui::BeginTabItem("Achievement"))
				{
					ImGui::BeginChild("scrolling##ACHIEVEMENT", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
					ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

					for (const Log& log : m_Logs[int(LogLevel::Achievement)])
						ImGui::TextColored(m_LogColors[int(log.Level) - 1], log.ToText().c_str());

					ImGui::PopStyleVar();

					if (m_AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
						ImGui::SetScrollHereY(1.0f);

					ImGui::EndChild();
					ImGui::EndTabItem();
				}

				if(ImGui::BeginTabItem("Settings"))
				{
					ImGui::Checkbox("Auto-scroll", &m_AutoScroll);
					if(ImGui::Button("Clear"))
					{
						for (std::vector<Log>& logContainer : m_Logs)
							logContainer.clear();
					}
					ImGui::EndTabItem();
				}
				
				ImGui::EndTabBar();
			}
			
			ImGui::End();
		}

	private:
		std::array<std::vector<Log>, 7> m_Logs;
		
		bool m_AutoScroll{ true };
		ImVec4 m_LogColors[6]
		{
			{ 1, 1, 1, 1 }, // INFO
			{ 0.5, 0.5, 0.5, 1 }, // TRACE
			{ 106.f / 255.f, 13.f / 255.f, 173.f / 255.f, 1 }, // DEBUG
			{ 1, 0.5f, 0, 1 }, // WARNING
			{ 1, 0, 0, 1 }, // ERROR
			{ 0, 1, 0, 1 } // ACHIEVEMENT
		};
	};
}
