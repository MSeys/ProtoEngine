#pragma once
#include "ProtoEnginePCH.h"

#include <array>
#include <SDL.h>

#include "windows.h"
#include <Xinput.h>

#include "CommandManager.h"

struct TriggerData
{
	float value = 0.f;
	bool IsZero() const
	{
		return value == 0;
	}
};

enum class TriggerState
{
	Pressed, Held, Released, None
};

enum class TriggerPosState
{
	Left, Right
};

class ControllerTrigger
{
public:
	ControllerTrigger(TriggerPosState triggerPosState);

	bool SetCommand(TriggerState state, const std::string& commandID, bool forceSet = false);
	bool UnlinkCommand(TriggerState state);

	void Update(const XINPUT_STATE& oldState, const XINPUT_STATE& currState);

	void Process();
	void SetInputData(Command& command);

private:
	TriggerPosState m_TriggerPosState{};
	TriggerState m_CurrState{ TriggerState::None };
	TriggerData m_Data;

	std::array<std::string, 3> m_Commands{ COMMAND_DEFAULT, COMMAND_DEFAULT, COMMAND_DEFAULT };
};