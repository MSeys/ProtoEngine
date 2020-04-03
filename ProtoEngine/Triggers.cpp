#include "ProtoEnginePCH.h"
#include "Triggers.h"

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

	if (m_TriggerPosState == TriggerPosState::Left)
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
		Command& command{ ProtoCommands.GetCommand(m_Commands[int(m_CurrState)]) };
		SetInputData(command);
		command.Execute();
	}
}

void ControllerTrigger::SetInputData(Command& command)
{
	command.SetInputData(CommandOrigin::C_TRIGGER, "NONE", &m_Data);
}
#pragma endregion Trigger
