#include "ProtoEnginePCH.h"
#include "AchievementObserver.h"

#include <iostream>

#include "AchievementManager.h"
#include "Events.h"


AchievementObserver::AchievementObserver(const std::vector<int>& listeningEvents)
    : Observer(listeningEvents)
{
}

void AchievementObserver::Notify(void* pData, int event)
{
	UNREFERENCED_PARAMETER(pData);
	
	if(event == E_GAME_STARTED)
	{
		ProtoAchievements(bool).GetAchievement("Ah, you're finally awake!")->Unlock(true);
	}
}