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
		AchievementManager() = default;
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

			// TODO: To Logger
			std::cout << "AchievementManager::AddAchievement > Achievement " << pAchievement->GetTitle() << " already added." << std::endl;

			return false;
		}

		Achievement<T>* GetAchievement(const std::string& title)
		{
			if (m_pAchievements.find(title) != m_pAchievements.end())
			{
				return m_pAchievements[title];
			}

			// TODO: To Logger
			std::cout << "AchievementManager::GetAchievement > Achievement " << title << " not found. (NullPointerException)" << std::endl;

			return nullptr;
		}

	private:
		std::unordered_map<std::string, Achievement<T>*> m_pAchievements;
	};
}