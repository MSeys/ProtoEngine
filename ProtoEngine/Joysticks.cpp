#include "ProtoEnginePCH.h"
#include "Joysticks.h"

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
		ProtoLogger.AddLog(LogLevel::Warning, "Joystick::SetCommand forced > A previous command has been replaced by force.");
		return true;
	}

	ProtoLogger.AddLog(LogLevel::Warning, "Joystick::SetCommand failed > There is already a command for this state.");
	return false;
}

bool Joystick::UnlinkCommand(JoystickState state)
{
	if (m_Commands[int(state)] != COMMAND_DEFAULT) // There is a command
	{
		m_Commands[int(state)] = COMMAND_DEFAULT; // Revert to Default Command
		return true;
	}

	ProtoLogger.AddLog(LogLevel::Warning, "Joystick::UnlinkCommand failed > There was no command available for this state.");
	return false;
}

void Joystick::Process()
{
	if (m_CurrState != JoystickState::None)
	{
		Command& command{ ProtoCommands.GetCommand(m_Commands[int(m_CurrState)]) };
		SetInputData(command);
		command.Execute();
	}
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

	if (m_StickState == StickState::Left)
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

	if (old.IsZero() && !m_Data.IsZero())
	{
		m_CurrState = JoystickState::Moving;
	}

	else if (!old.IsZero() && !m_Data.IsZero()) // still moving
	{
		m_CurrState = JoystickState::Moving;
	}

	else if (!old.IsZero() && m_Data.IsZero())
	{
		m_CurrState = JoystickState::Released;
	}

	else
	{
		m_CurrState = JoystickState::None;
	}
}

void ControllerJoystick::SetInputData(Command& command)
{
	command.SetInputData(CommandOrigin::C_JOYSTICK, -1, XINPUT_Keycode(-1), &m_Data);
}
#pragma endregion Variant (Joystick) | ControllerJoystick

#pragma region Variant (Joystick) | Mouse
void Mouse::UpdateInPoll(const SDL_Event& currState)
{
	const bool currMoving{ bool(currState.type == SDL_MOUSEMOTION) };

	if (currMoving)
	{
		m_CurrState = JoystickState::Moving;

		if(ProtoSettings.GetRenderMode() == RenderMode::EDITOR)
			m_Data.Position = { float((-ProtoSettings.GetGameMouseOffset().x + currState.motion.x) / ProtoSettings.GetGameAspectRatio()),
								float((-ProtoSettings.GetGameMouseOffset().y + currState.motion.y) / ProtoSettings.GetGameAspectRatio()) };
		else
			m_Data.Position = { float(currState.motion.x), float(currState.motion.y) };
		
		m_Data.LeftMouseDown = bool(currState.motion.state & SDL_BUTTON_LMASK);
		m_Data.RightMouseDown = bool(currState.motion.state & SDL_BUTTON_RMASK);
	}
}

void Mouse::UpdateOutPoll()
{
	m_CurrState = JoystickState::None;
}

void Mouse::SetInputData(Command& command)
{
	command.SetInputData(CommandOrigin::KBM_MOUSE, -1, XINPUT_Keycode(-1), &m_Data);
}
#pragma endregion Variant (Joystick) | Mouse