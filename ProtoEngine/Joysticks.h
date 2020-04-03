#pragma once
#include "ProtoEnginePCH.h"

#include <array>
#include <SDL.h>

#include "windows.h"
#include <Xinput.h>

#include "CommandManager.h"

enum class StickState
{
	Left, Right
};

enum class JoystickState
{
	Moving, Released, None
};

struct PositionData
{
	float x;
	float y;

	bool IsZero() const
	{
		return x == 0 && y == 0;
	}
};

struct MouseData
{
	PositionData Position{ 0, 0 };
	bool LeftMouseDown{ false };
	bool RightMouseDown{ false };
};

class Joystick
{
public:
	virtual ~Joystick() = default;
	bool SetCommand(JoystickState state, const std::string& commandID, bool forceSet = false);
	bool UnlinkCommand(JoystickState state);

	void Process();
	virtual void SetInputData(Command& command) = 0;

protected:
	JoystickState m_CurrState{ JoystickState::None };
	std::array<std::string, 2> m_Commands{ COMMAND_DEFAULT, COMMAND_DEFAULT };
};

class ControllerJoystick : public Joystick
{
public:
	ControllerJoystick(const StickState& stickState);
	void Update(const XINPUT_STATE& oldState, const XINPUT_STATE& currState);

	void SetInputData(Command& command) override;
	
private:
	StickState m_StickState;
	PositionData m_Data{};
};

class Mouse : public Joystick
{
public:
	void UpdateInPoll(const SDL_Event& currState);
	void UpdateOutPoll();

	void SetInputData(Command& command) override;
private:
	MouseData m_Data;
};