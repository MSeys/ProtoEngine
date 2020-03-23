#include "ProtoEnginePCH.h"
#include "Buttons.h"
#include "CommandManager.h"

#pragma region Base | Button
bool Button::SetCommand(ButtonState state, const std::string& commandID, bool forceSet)
{
	if (m_Commands[int(state)] == COMMAND_DEFAULT) // Is default command
	{
		m_Commands[int(state)] = commandID;
		return true;
	}

	if (forceSet)
	{
		m_Commands[int(state)] = commandID;
		std::cout << "ControllerButton::SetCommand forced > A previous command has been replaced by force." << std::endl; // TODO: Replace by Logger
		return true;
	}

	std::cout << "ControllerButton::SetCommand failed > There is already a command for this state." << std::endl; // TODO: Replace by Logger
	return true;
}

bool Button::UnlinkCommand(ButtonState state)
{
	if (m_Commands[int(state)] != COMMAND_DEFAULT) // There is a command
	{
		m_Commands[int(state)] = COMMAND_DEFAULT; // Revert to Default Command
		return true;
	}

	std::cout << "ControllerButton::UnlinkCommand failed > There was no command available for this state." << std::endl; // TODO: Replace by Logger
	return false;
}

void Button::Process()
{
	if(m_CurrState != ButtonState::None)
		ProtoCommands.GetCommand(m_Commands[int(m_CurrState)])->Execute();
}
#pragma endregion Base | Button

#pragma region Variant (Button) | ControllerButton
ControllerButton::ControllerButton(int XInput)
	: m_XInput(XInput)
{
}

void ControllerButton::Update(const XINPUT_STATE& oldState, const XINPUT_STATE& currState)
{
	const bool oldDown{ bool(oldState.Gamepad.wButtons & m_XInput) };
	const bool currDown{ bool(currState.Gamepad.wButtons & m_XInput) };

	if(!oldDown && currDown)
	{
		m_CurrState = ButtonState::Pressed;
	}

	else if (oldDown && currDown)
	{
		m_CurrState = ButtonState::Held;
	}

	else if(oldDown && !currDown)
	{
		m_CurrState = ButtonState::Released;
	}

	else // completely released
	{
		m_CurrState = ButtonState::None;
	}
}
#pragma endregion Variant (Button) | ControllerButton

#pragma region Variant (Button) | Key
void Key::UpdateInPoll(const SDL_Event& currState)
{
	const bool currDown{ bool(currState.type == SDL_KEYDOWN) }, currUp{ bool(currState.type == SDL_KEYUP) };

	if(currDown)
	{
		m_CurrState = ButtonState::Pressed;
	}

	else if(currUp)
	{
		m_CurrState = ButtonState::Released;
	}
}

void Key::UpdateOutPoll()
{
	m_CurrState = ButtonState::None;
}

void MouseButton::UpdateInPoll(const SDL_Event& currState)
{
	const bool currDown{ bool(currState.type == SDL_MOUSEBUTTONDOWN) }, currUp{ bool(currState.type == SDL_MOUSEBUTTONUP) };

	if (currDown)
	{
		m_CurrState = ButtonState::Pressed;
	}

	else if (currUp)
	{
		m_CurrState = ButtonState::Released;
	}
}
#pragma endregion Variant (Button) | Key



#pragma region Base | Joystick
bool Joystick::SetCommand(JoystickState state, const std::string& commandID, bool forceSet)
{
	if (m_Commands[int(state)] == COMMAND_DEFAULT) // Is default command
	{
		m_Commands[int(state)] = commandID;
		return true;
	}

	if (forceSet)
	{
		m_Commands[int(state)] = commandID;
		std::cout << "Joystick::SetCommand forced > A previous command has been replaced by force." << std::endl; // TODO: Replace by Logger
		return true;
	}

	std::cout << "Joystick::SetCommand failed > There is already a command for this state." << std::endl; // TODO: Replace by Logger
	return true;
}

bool Joystick::UnlinkCommand(JoystickState state)
{
	if (m_Commands[int(state)] != COMMAND_DEFAULT) // There is a command
	{
		m_Commands[int(state)] = COMMAND_DEFAULT; // Revert to Default Command
		return true;
	}

	std::cout << "Joystick::UnlinkCommand failed > There was no command available for this state." << std::endl; // TODO: Replace by Logger
	return false;
}
#pragma endregion Base | Joystick

#pragma region Variant (Joystick) | ControllerJoystick
ControllerJoystick::ControllerJoystick(const StickState& stickState)
	: m_StickState(stickState)
{
}

void ControllerJoystick::Update(const XINPUT_STATE& oldState, const XINPUT_STATE& currState)
{
	const float deadzoneX = 0.5f;
	const float deadzoneY = 0.5f;
	PositionData old{};
	
	if(m_StickState == StickState::Left)
	{
		{
			const float normLX = fmaxf(-1, static_cast<float>(oldState.Gamepad.sThumbLX) / 32767);
			const float normLY = fmaxf(-1, static_cast<float>(oldState.Gamepad.sThumbLY) / 32767);

			old.x = (abs(normLX) < deadzoneX ? 0 : normLX);
			old.y = (abs(normLY) < deadzoneY ? 0 : normLY);
		}

		{
			const float normLX = fmaxf(-1, static_cast<float>(currState.Gamepad.sThumbLX) / 32767);
			const float normLY = fmaxf(-1, static_cast<float>(currState.Gamepad.sThumbLY) / 32767);

			m_Data.x = (abs(normLX) < deadzoneX ? 0 : normLX);
			m_Data.y = (abs(normLY) < deadzoneY ? 0 : normLY);
		}
	}

	else
	{
		{
			const float normLX = fmaxf(-1, static_cast<float>(oldState.Gamepad.sThumbRX) / 32767);
			const float normLY = fmaxf(-1, static_cast<float>(oldState.Gamepad.sThumbRY) / 32767);

			old.x = (abs(normLX) < deadzoneX ? 0 : normLX);
			old.y = (abs(normLY) < deadzoneY ? 0 : normLY);
		}

		{
			const float normLX = fmaxf(-1, static_cast<float>(currState.Gamepad.sThumbRX) / 32767);
			const float normLY = fmaxf(-1, static_cast<float>(currState.Gamepad.sThumbRY) / 32767);

			m_Data.x = (abs(normLX) < deadzoneX ? 0 : normLX);
			m_Data.y = (abs(normLY) < deadzoneY ? 0 : normLY);
		}
	}

	if(old.IsZero() && !m_Data.IsZero())
	{
		m_CurrState = JoystickState::Moving;
	}

	else if(!old.IsZero() && !m_Data.IsZero()) // still moving
	{
		m_CurrState = JoystickState::Moving;
	}

	else if(!old.IsZero() && m_Data.IsZero())
	{
		m_CurrState = JoystickState::Released;
	}

	else
	{
		m_CurrState = JoystickState::None;
	}
}

void ControllerJoystick::Process()
{
	if (m_CurrState != JoystickState::None)
	{
		Command* currCommand{ ProtoCommands.GetCommand(m_Commands[int(m_CurrState)]) };
		currCommand->SetInputData(&m_Data);
		currCommand->Execute();
	}
}
#pragma endregion Variant (Joystick) | ControllerJoystick

#pragma region Variant (Joystick) | Mouse
void Mouse::UpdateInPoll(const SDL_Event& currState)
{
	const bool currMoving{ bool(currState.type == SDL_MOUSEMOTION) };

	if (currMoving)
	{
		m_CurrState = JoystickState::Moving;

		m_Data.Position = { float(currState.motion.x), float(currState.motion.y) };
		m_Data.LeftMouseDown = bool(currState.motion.state & SDL_BUTTON_LMASK);
		m_Data.RightMouseDown = bool(currState.motion.state & SDL_BUTTON_RMASK);
	}
}

void Mouse::UpdateOutPoll()
{
	m_CurrState = JoystickState::None;
}

void Mouse::Process()
{
	if (m_CurrState != JoystickState::None)
	{
		Command* currCommand{ ProtoCommands.GetCommand(m_Commands[int(m_CurrState)]) };
		currCommand->SetInputData(&m_Data);
		currCommand->Execute();
	}
}
#pragma endregion Variant (Joystick) | Mouse

#pragma region Trigger
ControllerTrigger::ControllerTrigger(TriggerPosState triggerPosState)
	: m_TriggerPosState(triggerPosState)
{
}

bool ControllerTrigger::SetCommand(TriggerState state, const std::string& commandID, bool forceSet)
{
	if (m_Commands[int(state)] == COMMAND_DEFAULT) // Is default command
	{
		m_Commands[int(state)] = commandID;
		return true;
	}

	if (forceSet)
	{
		m_Commands[int(state)] = commandID;
		std::cout << "Trigger::SetCommand forced > A previous command has been replaced by force." << std::endl; // TODO: Replace by Logger
		return true;
	}

	std::cout << "Trigger::SetCommand failed > There is already a command for this state." << std::endl; // TODO: Replace by Logger
	return true;
}

bool ControllerTrigger::UnlinkCommand(TriggerState state)
{
	if (m_Commands[int(state)] != COMMAND_DEFAULT) // There is a command
	{
		m_Commands[int(state)] = COMMAND_DEFAULT; // Revert to Default Command
		return true;
	}

	std::cout << "Trigger::UnlinkCommand failed > There was no command available for this state." << std::endl; // TODO: Replace by Logger
	return false;
}

void ControllerTrigger::Update(const XINPUT_STATE& oldState, const XINPUT_STATE& currState)
{
	TriggerData old{};
	
	if(m_TriggerPosState == TriggerPosState::Left)
	{
		old.value = static_cast<float>(oldState.Gamepad.bLeftTrigger) / 255;
		m_Data.value = static_cast<float>(currState.Gamepad.bLeftTrigger) / 255;
	}
	
	else
	{
		old.value = static_cast<float>(oldState.Gamepad.bRightTrigger) / 255;
		m_Data.value = static_cast<float>(currState.Gamepad.bRightTrigger) / 255;
	}

	if (old.IsZero() && !m_Data.IsZero())
	{
		m_CurrState = TriggerState::Pressed;
	}

	else if (!old.IsZero() && !m_Data.IsZero())
	{
		m_CurrState = TriggerState::Held;
	}

	else if (!old.IsZero() && m_Data.IsZero())
	{
		m_CurrState = TriggerState::Released;
	}

	else // completely released
	{
		m_CurrState = TriggerState::None;
	}
}

void ControllerTrigger::Process()
{
	if (m_CurrState != TriggerState::None)
	{
		Command* currCommand{ ProtoCommands.GetCommand(m_Commands[int(m_CurrState)]) };
		currCommand->SetInputData(&m_Data);
		currCommand->Execute();
	}
}
#pragma endregion Trigger