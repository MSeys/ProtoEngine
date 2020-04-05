#pragma once
#include <array>
#include <iostream>
#include <unordered_map>

#include "windows.h"
#include "Xinput.h"

#include "Buttons.h"

#include "Joysticks.h"

#include "ControllerHandler.h"
#include "KBMHandler.h"


#include "Singleton.h"
#include "Triggers.h"

struct ImGuiIO;
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

		bool AddControllerButton(int index, int XInput, const std::string& stringifiedXInput = "UNDEFINED");
		bool AddKey(int SDLKey, const std::string& stringifiedSDLKey = "UNDEFINED");

		ControllerButton& GetControllerButton(int index, int XInput);
		ControllerJoystick& GetControllerJoystick(int index, const StickState& stickState) const noexcept;
		ControllerTrigger& GetControllerTrigger(int index, const TriggerPosState& triggerPosState) const noexcept;

		Key& GetKey(int SDLKey, bool doesCreate = true);
		MouseButton& GetMouseButton(const StickState& mouseState) const;
		Mouse& GetMouse() const;

	private:
		ControllerHandler m_ControllerHandler;
		KBMHandler m_KBMHandler;

		void InitImGuiIO();
	};
}