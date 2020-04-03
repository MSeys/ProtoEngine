#pragma once
#include "ProtoEnginePCH.h"

#include <array>
#include <SDL.h>

#include "windows.h"
#include <Xinput.h>

#include "CommandManager.h"

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
	virtual void SetInputData(Command& command) = 0;
	
protected:
	ButtonState m_CurrState{ ButtonState::None };

private:
	std::array<std::string, 3> m_Commands{ COMMAND_DEFAULT, COMMAND_DEFAULT, COMMAND_DEFAULT };
};

class ControllerButton final : public Button
{

public:
	ControllerButton(int XInput, std::string stringifiedXInput);
	void Update(const XINPUT_STATE& oldState, const XINPUT_STATE& currState);
	void SetInputData(Command& command) override;
	
private:
	int m_XInput;
	std::string m_StringifiedXInput;
};

class Key : public Button
{
public:
	Key(std::string stringifiedSDLKey);

	virtual void UpdateInPoll(const SDL_Event& currState);
	void UpdateOutPoll();
	void SetInputData(Command& command) override;

private:
	std::string m_StringifiedSDLKey;
};

class MouseButton final : public Key
{
public:
	MouseButton();

	void UpdateInPoll(const SDL_Event& currState) override;
	void SetInputData(Command& command) override;
};