#pragma once
#include <chrono>
#include <thread>

#include "Singleton.h"
#include "ProtoUtils.h"

#define TimePoint std::chrono::steady_clock::time_point  // NOLINT(cppcoreguidelines-macro-usage)

namespace Proto
{
	class TimeManager final : public Singleton<TimeManager>
	{

	public:
		void SetStartTime(const TimePoint& startTime) { m_StartTime = startTime; }
		void SetCurrTime(const TimePoint& currTime) { m_CurrTime = currTime; }
		TimePoint GetCurrTime() const { return m_CurrTime; }
		
		void UpdateTime()
		{
			DeltaTime == 0.f ? FPS = 0 : FPS = int(1.f / DeltaTime);
			FPS_Unscaled = int(1.f / DeltaTime_Unscaled);
			
			DeltaTime = std::chrono::duration<float>(m_CurrTime - m_StartTime).count();
						
			if (ProtoSettings.GetWindowSettings().FPSState == FPSState::PROTO_CAPPED)
			{
				const int targetFPS_microSeconds{ int(SecondsToMicroSeconds(1.f / float(ProtoSettings.GetWindowSettings().FPSRate))) };
				const int sleepTime_microSeconds{ targetFPS_microSeconds - int(SecondsToMicroSeconds(DeltaTime)) };

				std::this_thread::sleep_for(std::chrono::microseconds(sleepTime_microSeconds));

				DeltaTime += MicroSecondsToSeconds(float(sleepTime_microSeconds));
			}
			
			DeltaTime_Unscaled = DeltaTime;
			DeltaTime *= TimeScale;
		}

		const float FixedDeltaTime{ 0.02f };
		float DeltaTime{};
		unsigned int FPS{};
		float TimeScale{ 1 };

		float DeltaTime_Unscaled;
		unsigned int FPS_Unscaled;

	private:
		TimePoint m_StartTime;
		TimePoint m_CurrTime;
	};
}