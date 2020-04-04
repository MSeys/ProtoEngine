#pragma once

#include <iostream>
#include <vector>
#include "Observer.h"

class AchievementObserver : public Observer
{
public:
	AchievementObserver(const std::vector<int>& listeningEvents);
	
	void Notify(void* pData, int event) override;
};