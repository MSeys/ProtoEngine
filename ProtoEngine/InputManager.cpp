#include "ProtoEnginePCH.h"
#include "InputManager.h"
#include <SDL.h>

#include "Buttons.h"

void Proto::InputManager::Init()
{
	m_ControllerHandler.Init();
	m_KBMHandler.Init();
}

void Proto::InputManager::Destroy()
{
	m_ControllerHandler.Destroy();
	m_KBMHandler.Destroy();
}

void Proto::InputManager::Update()
{
	m_ControllerHandler.Update();

	m_KBMHandler.UpdateOutPoll();

	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT)
		{
			ProtoCommands.ForceExit();
			break;
		}

		m_KBMHandler.UpdateInPoll(e);
	}
}

void Proto::InputManager::Process()
{
	m_ControllerHandler.Process();
	m_KBMHandler.Process();
}

bool Proto::InputManager::AddControllerButton(int index, int XInput, const std::string& stringifiedXInput)
{
	return m_ControllerHandler.AddInput(index, XInput, stringifiedXInput);
}

bool Proto::InputManager::AddKey(int SDLKey, const std::string& stringifiedSDLKey)
{
	return m_KBMHandler.AddInput(SDLKey, stringifiedSDLKey);
}

ControllerButton& Proto::InputManager::GetControllerButton(int index, int XInput)
{
	return m_ControllerHandler.GetInput(index, XInput);
}

ControllerJoystick& Proto::InputManager::GetControllerJoystick(int index, const StickState& stickState) const noexcept
{
	return m_ControllerHandler.GetInput(index, stickState);
}

ControllerTrigger& Proto::InputManager::GetControllerTrigger(int index, const TriggerPosState& triggerPosState) const noexcept
{
	return m_ControllerHandler.GetInput(index, triggerPosState);
}

Key& Proto::InputManager::GetKey(int SDLKey, bool doesCreate)
{
	return m_KBMHandler.GetInput(SDLKey, doesCreate);
}

MouseButton& Proto::InputManager::GetMouseButton(const StickState& mouseState) const
{
	return m_KBMHandler.GetInput(mouseState);
}

Mouse& Proto::InputManager::GetMouse() const
{
	return m_KBMHandler.GetInput();
}