#pragma once
#include <chrono>
#include "Singleton.h"

#define Time std::chrono::steady_clock::time_point  // NOLINT(cppcoreguidelines-macro-usage)

namespace Proto
{
	class TimeManager final : public Singleton<TimeManager>
	{

	public:
		void SetStartTime(const Time& startTime) { m_StartTime = startTime; }
		void SetCurrTime(const Time& currTime) { m_CurrTime = currTime; }
		Time GetCurrTime() const { return m_CurrTime; }
		void SetMaxFPS(unsigned int maxFPS) { m_MaxFPS = maxFPS; }
		void UpdateTime() { DeltaTime = std::chrono::duration<float>(m_CurrTime - m_StartTime).count(); }
		
		float DeltaTime{};

	private:
		Time m_StartTime;
		Time m_CurrTime;
		unsigned int m_MaxFPS{};
	};
}

