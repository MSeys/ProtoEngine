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
	if (m_CurrState != ButtonState::None)
	{
		Command& command{ ProtoCommands.GetCommand(m_Commands[int(m_CurrState)]) };
		SetInputData(command);
		command.Execute();
	}
}
#pragma endregion Base | Button

#pragma region Variant (Button) | ControllerButton
ControllerButton::ControllerButton(int XInput, std::string stringifiedXInput)
	: m_XInput(XInput), m_StringifiedXInput(std::move(stringifiedXInput))
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

void ControllerButton::SetInputData(Command& command)
{
	command.SetInputData(CommandOrigin::C_BUTTON, m_StringifiedXInput, nullptr);
}
#pragma endregion Variant (Button) | ControllerButton

#pragma region Variant (Button) | Key
Key::Key(std::string stringifiedSDLKey)
	: m_StringifiedSDLKey(std::move(stringifiedSDLKey))
{
}

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

void Key::SetInputData(Command& command)
{
	command.SetInputData(CommandOrigin::KBM_KEY, m_StringifiedSDLKey, nullptr);
}
#pragma endregion Variant (Button) | Key

#pragma region Variant (Button) | MouseButton
MouseButton::MouseButton()
	: Key("")
{
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

void MouseButton::SetInputData(Command& command)
{
	command.SetInputData(CommandOrigin::KBM_MOUSE_KEY, "NONE", nullptr);
}
#pragma endregion Variant (Button) | MouseButton