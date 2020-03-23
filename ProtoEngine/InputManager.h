#pragma once
#include <array>
#include <iostream>
#include <unordered_map>

#include "Buttons.h"
#include "CommandManager.h"
#include "windows.h"
#include "Xinput.h"

#include "Singleton.h"

class Command;

#define XINPUT_CONTROLLER_MIN_INDEX 0  // NOLINT(cppcoreguidelines-macro-usage)
#define XINPUT_CONTROLLER_MAX_INDEX 3  // NOLINT(cppcoreguidelines-macro-usage)

namespace Proto
{
	class InputManager final : public Singleton<InputManager>
	{

	public:
		InputManager();
		~InputManager();

		void UpdateStates();
		void ProcessStates();

		bool AddControllerButton(int index, int XInput);
		bool AddKey(int sdlKey);

		ControllerButton* GetControllerButton(int index, int XInput);
		ControllerJoystick* GetControllerJoystick(int index, StickState stickState) const;
		ControllerTrigger* GetControllerTrigger(int index, TriggerPosState triggerPosState) const;

		Key* GetKey(int sdlKey, bool logNPE = true);
		MouseButton* GetMouseButton(int sdlKey);
		Mouse* GetMouse() const;

	private:
		friend ControllerButton;
		std::array<XINPUT_STATE, 4> m_CurrentStates{};
		std::array<XINPUT_STATE, 4> m_OldStates{};

		std::unordered_map<int, std::unordered_map<int, ControllerButton*>> m_ControllerButtons; // Map with Controller Index | Map with Buttons
		std::unordered_map<int, Key*> m_Keys;
		std::array<ControllerJoystick*, 4> m_pLeftJoysticks{};
		std::array<ControllerJoystick*, 4> m_pRightJoysticks{};

		std::array<ControllerTrigger*, 4> m_pLeftTriggers{};
		std::array<ControllerTrigger*, 4> m_pRightTriggers{};

		std::array<MouseButton*, 2> m_pMouseButtons{};
		Mouse* m_pMouse{};
	};
}