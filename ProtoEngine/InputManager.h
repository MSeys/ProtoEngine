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

		bool AddInput(int index, XINPUT_Keycode keyCode);
		bool AddInput(SDL_Keycode keyCode);

		ControllerButton& GetInput(int index, XINPUT_Keycode keyCode);
		ControllerJoystick& GetInput(int index, const StickState& stickState) const noexcept;
		ControllerTrigger& GetInput(int index, const TriggerPosState& triggerPosState) const noexcept;

		Key& GetInput(SDL_Keycode keyCode);
		MouseButton& GetMouseButton(const MouseState& mouseState) const;
		Mouse& GetMouse() const;

	private:
		ControllerHandler m_ControllerHandler;
		KBMHandler m_KBMHandler;

		void InitImGuiIO();
	};
}