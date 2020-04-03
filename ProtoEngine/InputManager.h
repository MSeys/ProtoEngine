#pragma once
#include <array>
#include <iostream>
#include <unordered_map>

#include "Buttons.h"
#include "ControllerHandler.h"
#include "KBMHandler.h"
#include "windows.h"
#include "Xinput.h"

#include "Singleton.h"

class Command;

namespace Proto
{
	class InputManager final : public Singleton<InputManager>
	{

	public:
		void Init();
		void Destroy();

		void Update();
		void Process();

		bool AddControllerButton(int index, int XInput);
		bool AddKey(int sdlKey);

		ControllerButton& GetControllerButton(int index, int XInput);
		ControllerJoystick& GetControllerJoystick(int index, const StickState& stickState) const noexcept;
		ControllerTrigger& GetControllerTrigger(int index, const TriggerPosState& triggerPosState) const noexcept;

		Key& GetKey(int sdlKey, bool doesCreate = true);
		MouseButton& GetMouseButton(const StickState& mouseState) const;
		Mouse& GetMouse() const;

	private:
		ControllerHandler m_ControllerHandler;
		KBMHandler m_KBMHandler;
	};
}