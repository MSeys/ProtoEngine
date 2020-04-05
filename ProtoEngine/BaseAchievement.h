#pragma once
#include <iostream>
#include <string>
#include <utility>

enum class AchievementState
{
	NOT_ACHIEVED, ACHIEVED, ALREADY_ACHIEVED
};

template <typename T>
class Achievement
{
public:
	Achievement(std::string title, std::string prefix, T req, std::string suffix)
		: m_Title(std::move(title)), m_Prefix(std::move(prefix)), m_Suffix(std::move(suffix)), m_Req(req)
	{
	}

	virtual ~Achievement() = default;

	Achievement(const Achievement& other) = delete;
	Achievement(Achievement&& other) = delete;
	Achievement& operator=(const Achievement& other) = delete;
	Achievement& operator=(Achievement&& other) = delete;
	
	virtual AchievementState Unlock(T progress) = 0;

	std::string GetTitle() const
	{
		return m_Title;
	}

protected:
	bool m_Unlocked{ false };
	std::string m_Title;
	std::string m_Prefix;
	std::string m_Suffix;
	T m_Req{};
};

class NumberAchievement : public Achievement<int>
{
public:
	NumberAchievement(const std::string& title, const std::string& prefix, const int req, const std::string& suffix)
		: Achievement<int>(title, prefix, req, suffix)
	{
	}

	AchievementState Unlock(int progress) override
	{
		if (m_Unlocked)
			return AchievementState::ALREADY_ACHIEVED;
		
		if(progress >= m_Req)
		{
			ProtoLogger.AddLog(LogLevel::Achievement, m_Title + " - " + m_Prefix + " " + std::to_string(m_Req) + " " + m_Suffix);
			
			m_Unlocked = true;
			return AchievementState::ACHIEVED;
		}

		return AchievementState::NOT_ACHIEVED;
	}
};

class ConditionAchievement : public Achievement<bool>
{
public:
	ConditionAchievement(const std::string& title, const std::string& prefix, const bool req, const std::string& suffix = "")
		: Achievement<bool>(title, prefix, req, suffix)
	{
	}

	AchievementState Unlock(bool progress) override
	{
		if (m_Unlocked)
			return AchievementState::ALREADY_ACHIEVED;

		if (progress == m_Req)
		{
			ProtoLogger.AddLog(LogLevel::Achievement, m_Title + " - " + m_Prefix + " " + m_Suffix);

			m_Unlocked = true;
			return AchievementState::ACHIEVED;
		}

		return AchievementState::NOT_ACHIEVED;
	}
};