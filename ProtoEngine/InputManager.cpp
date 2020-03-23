#include "ProtoEnginePCH.h"
#include "InputManager.h"
#include <SDL.h>

#include "Buttons.h"

Proto::InputManager::InputManager()
{
	for (int i{ XINPUT_CONTROLLER_MIN_INDEX }; i <= XINPUT_CONTROLLER_MAX_INDEX; i++)
	{
		m_pLeftJoysticks[i] = new ControllerJoystick(StickState::Left);
		m_pRightJoysticks[i] = new ControllerJoystick(StickState::Right);

		m_pLeftTriggers[i] = new ControllerTrigger(TriggerPosState::Left);
		m_pRightTriggers[i] = new ControllerTrigger(TriggerPosState::Right);
	}

	m_pMouseButtons[0] = new MouseButton();
	m_pMouseButtons[1] = new MouseButton();
	m_pMouse = new Mouse();
}

Proto::InputManager::~InputManager()
{
	for(auto& upperPair : m_ControllerButtons)
		for(auto& innerPair : upperPair.second)
			SafeDelete(innerPair.second);

	for (auto& pair : m_Keys)
		SafeDelete(pair.second);

	for(auto& pJoystick : m_pLeftJoysticks)
		SafeDelete(pJoystick);

	for (auto& pJoystick : m_pRightJoysticks)
		SafeDelete(pJoystick);

	for (auto& pTrigger : m_pLeftTriggers)
		SafeDelete(pTrigger);

	for (auto& pTrigger : m_pRightTriggers)
		SafeDelete(pTrigger);

	for (auto& pMouseButton : m_pMouseButtons)
		SafeDelete(pMouseButton);

	SafeDelete(m_pMouse);
}

void Proto::InputManager::UpdateStates()
{
	for (int i{ XINPUT_CONTROLLER_MIN_INDEX }; i <= XINPUT_CONTROLLER_MAX_INDEX; i++)
	{
		m_OldStates[i] = m_CurrentStates[i];
		ZeroMemory(&m_CurrentStates[i], sizeof(XINPUT_STATE));
		XInputGetState(0, &m_CurrentStates[i]);
	}

	for (auto& pair : m_Keys)
	{
		Key* pKey{ pair.second };
		pKey->UpdateOutPoll();
	}

	for(auto& pMouseButton : m_pMouseButtons)
	{
		pMouseButton->UpdateOutPoll();
	}

	GetMouse()->UpdateOutPoll();
	
	SDL_Event e;
	while (SDL_PollEvent(&e)) {
		if(e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
		{
			if(GetKey(e.key.keysym.sym, false))
			{
				GetKey(e.key.keysym.sym)->UpdateInPoll(e);
			}
		}

		if(e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP)
		{
			GetMouseButton(e.button.button)->UpdateInPoll(e);
		}

		if(e.type == SDL_MOUSEMOTION)
		{
			GetMouse()->UpdateInPoll(e);
		}
	}

	for (auto& upperPair : m_ControllerButtons)
	{
		const int index{ upperPair.first };
		for (auto& innerPair : upperPair.second)
		{
			ControllerButton* pControllerButton{ innerPair.second };
			pControllerButton->Update(m_OldStates[index], m_CurrentStates[index]);
		}
	}

	for (int i{ XINPUT_CONTROLLER_MIN_INDEX }; i <= XINPUT_CONTROLLER_MAX_INDEX; i++)
	{
		m_pLeftJoysticks[i]->Update(m_OldStates[i], m_CurrentStates[i]);
		m_pRightJoysticks[i]->Update(m_OldStates[i], m_CurrentStates[i]);

		m_pLeftTriggers[i]->Update(m_OldStates[i], m_CurrentStates[i]);
		m_pRightTriggers[i]->Update(m_OldStates[i], m_CurrentStates[i]);
	}
}

void Proto::InputManager::ProcessStates()
{
	for(auto& upperPair : m_ControllerButtons)
	{
		for(auto& innerPair : upperPair.second)
		{
			ControllerButton* pControllerButton{ innerPair.second };
			pControllerButton->Process();
		}
	}

	for (auto& pair : m_Keys)
	{
		Key* pKey{ pair.second };
		pKey->Process();
	}

	for(auto& pJoystick : m_pLeftJoysticks)
	{
		pJoystick->Process();
	}

	for (auto& pJoystick : m_pRightJoysticks)
	{
		pJoystick->Process();
	}

	for (auto& pTrigger : m_pLeftTriggers)
	{
		pTrigger->Process();
	}

	for (auto& pTrigger : m_pRightTriggers)
	{
		pTrigger->Process();
	}

	for(auto& pMouseButton : m_pMouseButtons)
	{
		pMouseButton->Process();
	}

	GetMouse()->Process();
}

bool Proto::InputManager::AddControllerButton(int index, int XInput)
{
	if(index > XINPUT_CONTROLLER_MAX_INDEX || index < XINPUT_CONTROLLER_MIN_INDEX)
	{
		std::cout << "InputManager::AddControllerButton failed > XInput only allows 4 controllers maximum. (index 0 - 3)." << std::endl; // TODO: Replace by Logger
		return false;
	}

	if(m_ControllerButtons[index].find(XInput) != m_ControllerButtons[index].end()) // if map does not exist yet with index, it'll make inner map
	{
		std::cout << "InputManager::AddControllerButton failed > Controller Button was already added." << std::endl; // TODO: Replace by Logger
		return false;
	}

	m_ControllerButtons[index][XInput] = new ControllerButton{ XInput };
	return true;
}

bool Proto::InputManager::AddKey(int sdlKey)
{
	if (m_Keys.find(sdlKey) != m_Keys.end())
	{
		std::cout << "InputManager::AddKey failed > Key was already added." << std::endl; // TODO: Replace by Logger
		return false;
	}

	m_Keys[sdlKey] = new Key();
	return true;
}

ControllerButton* Proto::InputManager::GetControllerButton(int index, int XInput)
{
	if(m_ControllerButtons[index].find(XInput) != m_ControllerButtons[index].end())
	{
		return m_ControllerButtons[index][XInput];
	}

	std::cout << "InputManager::GetControllerButton failed > Controller Button not found (NullPointerException)." << std::endl; // TODO: Replace by Logger
	return nullptr;
}

ControllerJoystick* Proto::InputManager::GetControllerJoystick(int index, StickState stickState) const
{
	switch(stickState) {
		case StickState::Left:
			return m_pLeftJoysticks[index];
		case StickState::Right:
			return m_pRightJoysticks[index];
	}

	return nullptr;
}

ControllerTrigger* Proto::InputManager::GetControllerTrigger(int index, TriggerPosState triggerPosState) const
{
	switch (triggerPosState) {
	case TriggerPosState::Left:
		return m_pLeftTriggers[index];
	case TriggerPosState::Right:
		return m_pRightTriggers[index];
	}

	return nullptr;
}

Key* Proto::InputManager::GetKey(int sdlKey, bool logNPE)
{
	if (m_Keys.find(sdlKey) != m_Keys.end())
	{
		return m_Keys[sdlKey];
	}

	if(logNPE)
		std::cout << "InputManager::GetKey failed > Key not found (NullPointerException)." << std::endl; // TODO: Replace by Logger

	return nullptr;
}

MouseButton* Proto::InputManager::GetMouseButton(int sdlKey)
{
	if (sdlKey == SDL_BUTTON_LEFT)
		return m_pMouseButtons[0];

	if(sdlKey == SDL_BUTTON_RIGHT)
		return m_pMouseButtons[1];

	return nullptr;
}

Mouse* Proto::InputManager::GetMouse() const
{
	return m_pMouse;
}