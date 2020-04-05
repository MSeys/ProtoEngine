#pragma once
#include <unordered_map>


#include "BaseAchievement.h"
#include "Singleton.h"

namespace Proto
{
	template <typename T>
	class AchievementManager : public Singleton<AchievementManager<T>>
	{
	public:
		AchievementManager()
		{
		}
		
		~AchievementManager()
		{
			for (auto& pair : m_pAchievements)
			{
				SafeDelete(pair.second);
			}
		}

		AchievementManager(const AchievementManager& other) = delete;
		AchievementManager(AchievementManager&& other) = delete;
		AchievementManager& operator=(const AchievementManager& other) = delete;
		AchievementManager& operator=(AchievementManager&& other) = delete;

		bool AddAchievement(Achievement<T>* pAchievement)
		{
			if (m_pAchievements.find(pAchievement->GetTitle()) == m_pAchievements.end())
			{
				m_pAchievements[pAchievement->GetTitle()] = pAchievement;
				return true;
			}

			ProtoLogger.AddLog(LogLevel::Warning, "AchievementManager::AddAchievement > Achievement " + pAchievement->GetTitle() + " already added.");

			return false;
		}

		Achievement<T>* GetAchievement(const std::string& title)
		{
			try
			{
				return m_pAchievements.at(title);
			}
			catch(const std::out_of_range& exception)
			{
				UNREFERENCED_PARAMETER(exception);
				
				// TODO: Avoid NullPointerException
				ProtoLogger.AddLog(LogLevel::Error, "AchievementManager::GetAchievement > Achievement " + title + " not found. NullPointerException");
				return nullptr;
			}
		}

	private:
		std::unordered_map<std::string, Achievement<T>*> m_pAchievements;
	};
}