#pragma once
#include "Triggers.h"
#include "Joysticks.h"
#include "Buttons.h"

constexpr int XINPUT_CONTROLLER_MIN_INDEX = 0;
constexpr int XINPUT_CONTROLLER_MAX_INDEX = 3;

namespace Proto
{
	class ControllerHandler
	{
	public:
		void Init();
		void Destroy();
		
		void Update();
		void Process();

		void Reset();

		bool AddInput(int index, XINPUT_Keycode keyCode);
		ControllerButton& GetInput(int index, XINPUT_Keycode keyCode);
		ControllerJoystick& GetInput(int index, const StickState& stickState) const;
		ControllerTrigger& GetInput(int index, const TriggerPosState& triggerPosState) const;

	private:
		std::array<XINPUT_STATE, 4> m_CurrentStates{};
		std::array<XINPUT_STATE, 4> m_OldStates{};

		std::unordered_map<int, std::unordered_map<XINPUT_Keycode, ControllerButton*>> m_ControllerButtons;
		std::array<ControllerJoystick*, 4> m_pLeftJoysticks{};
		std::array<ControllerJoystick*, 4> m_pRightJoysticks{};

		std::array<ControllerTrigger*, 4> m_pLeftTriggers{};
		std::array<ControllerTrigger*, 4> m_pRightTriggers{};
	};
}