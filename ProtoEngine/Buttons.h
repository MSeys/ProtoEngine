#pragma once
#include "ProtoEnginePCH.h"

#include <array>
#include <SDL.h>

#include "windows.h"
#include <Xinput.h>

#pragma region Button and Keys
enum class ButtonState
{
	Pressed, Held, Released, None
};

class Button
{
public:
	virtual ~Button() = default;
	bool SetCommand(ButtonState state, const std::string& commandID, bool forceSet = false);
	bool UnlinkCommand(ButtonState state);
	
	void Process();
	
protected:
	ButtonState m_CurrState{ ButtonState::None };

private:
	std::array<std::string, 3> m_Commands{ COMMAND_DEFAULT, COMMAND_DEFAULT, COMMAND_DEFAULT };
};

class ControllerButton final : public Button
{

public:
	ControllerButton(int XInput);
	void Update(const XINPUT_STATE& oldState, const XINPUT_STATE& currState);
private:
	int m_XInput;
};

class Key : public Button
{
public:
	virtual void UpdateInPoll(const SDL_Event& currState);
	void UpdateOutPoll();
};

class MouseButton final : public Key
{
public:
	void UpdateInPoll(const SDL_Event& currState) override;
};
#pragma endregion Button and Keys

#pragma region Joystick and Mouse

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

	virtual void Process() = 0;

protected:
	JoystickState m_CurrState{ JoystickState::None };
	std::array<std::string, 2> m_Commands{ COMMAND_DEFAULT, COMMAND_DEFAULT };
};

class ControllerJoystick : public Joystick
{
public:
	ControllerJoystick(const StickState& stickState);
	void Update(const XINPUT_STATE& oldState, const XINPUT_STATE& currState);
	void Process() override;

private:
	StickState m_StickState;
	PositionData m_Data;
};

class Mouse : public Joystick
{
public:
	void UpdateInPoll(const SDL_Event& currState);
	void UpdateOutPoll();
	void Process() override;
	
private:
	MouseData m_Data;
};
#pragma endregion Joystick and Mouse

#pragma region Trigger
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

private:
	TriggerPosState m_TriggerPosState{};
	TriggerState m_CurrState{ TriggerState::None };
	TriggerData m_Data;

	std::array<std::string, 3> m_Commands{ COMMAND_DEFAULT, COMMAND_DEFAULT, COMMAND_DEFAULT };
};
#pragma endregion Trigger