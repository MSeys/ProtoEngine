#include "ProtoEnginePCH.h"
#include "Buttons.h"

#include <utility>
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
		ProtoLogger.AddLog(LogLevel::Warning, "ControllerButton::SetCommand forced > A previous command has been replaced by force.");
		return true;
	}

	ProtoLogger.AddLog(LogLevel::Warning, "ControllerButton::SetCommand failed > There is already a command for this state.");
	return false;
}

bool Button::UnlinkCommand(ButtonState state)
{
	if (m_Commands[int(state)] != COMMAND_DEFAULT) // There is a command
	{
		m_Commands[int(state)] = COMMAND_DEFAULT; // Revert to Default Command
		return true;
	}

	ProtoLogger.AddLog(LogLevel::Warning, "ControllerButton::UnlinkCommand failed > There was no command available for this state.");
	return false;
}

void Button::Process()
{
	if (m_CurrState != ButtonState::None)
	{
		Command& command{ ProtoCommands.GetCommand(m_Commands[int(m_CurrState)]) };
		SetInputData(command);
		command.Execute();
	}
}
#pragma endregion Base | Button

#pragma region Variant (Button) | ControllerButton
ControllerButton::ControllerButton(const XINPUT_Keycode& keyCode)
	: m_KeyCode(keyCode)
{
}

void ControllerButton::Update(const XINPUT_STATE& oldState, const XINPUT_STATE& currState)
{
	const bool oldDown{ bool(oldState.Gamepad.wButtons & m_KeyCode) };
	const bool currDown{ bool(currState.Gamepad.wButtons & m_KeyCode) };

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

void ControllerButton::SetInputData(Command& command)
{
	command.SetInputData(CommandOrigin::C_BUTTON, -1, m_KeyCode, nullptr);
}
#pragma endregion Variant (Button) | ControllerButton

#pragma region Variant (Button) | Key
Key::Key(const SDL_Keycode& keyCode)
	: m_KeyCode(keyCode)
{
}

void Key::UpdateInPoll(const SDL_Event& currState)
{
	const bool currDown{ bool(currState.type == SDL_KEYDOWN) }, currUp{ bool(currState.type == SDL_KEYUP) };

	if(currDown)
		m_CurrState = ButtonState::Pressed;

	else if(currUp)
		m_CurrState = ButtonState::Released;
}

void Key::UpdateOutPoll()
{
	switch(m_CurrState)
	{
	case ButtonState::Pressed:
	case ButtonState::Held:
		m_CurrState = ButtonState::Held;
		break;
	case ButtonState::Released:
	case ButtonState::None:
		m_CurrState = ButtonState::None;
		break;
	default:
		break;
	}
}

void Key::SetInputData(Command& command)
{
	command.SetInputData(CommandOrigin::KBM_KEY, m_KeyCode, XINPUT_Keycode(-1), nullptr);
}
#pragma endregion Variant (Button) | Key

#pragma region Variant (Button) | MouseButton
MouseButton::MouseButton()
	: Key(-1)
{
}

void MouseButton::UpdateInPoll(const SDL_Event& currState)
{
	const bool currDown{ bool(currState.type == SDL_MOUSEBUTTONDOWN) }, currUp{ bool(currState.type == SDL_MOUSEBUTTONUP) };

	if (currDown)
		m_CurrState = ButtonState::Pressed;

	else if (currUp)
		m_CurrState = ButtonState::Released;
}

void MouseButton::SetInputData(Command& command)
{
	command.SetInputData(CommandOrigin::KBM_MOUSE_KEY, -1, XINPUT_Keycode(-1), nullptr);
}
#pragma endregion Variant (Button) | MouseButton