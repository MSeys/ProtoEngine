#include "ProtoEnginePCH.h"
#include "ControllerHandler.h"

void Proto::ControllerHandler::Init()
{
	for (int i{ XINPUT_CONTROLLER_MIN_INDEX }; i <= XINPUT_CONTROLLER_MAX_INDEX; i++)
	{
		m_pLeftJoysticks[i] = new ControllerJoystick(StickState::Left);
		m_pRightJoysticks[i] = new ControllerJoystick(StickState::Right);

		m_pLeftTriggers[i] = new ControllerTrigger(TriggerPosState::Left);
		m_pRightTriggers[i] = new ControllerTrigger(TriggerPosState::Right);
	}
}

void Proto::ControllerHandler::Destroy()
{
	for (auto& upperPair : m_ControllerButtons)
		for (auto& innerPair : upperPair.second)
			SafeDelete(innerPair.second);

	for (auto& pJoystick : m_pLeftJoysticks)
		SafeDelete(pJoystick);

	for (auto& pJoystick : m_pRightJoysticks)
		SafeDelete(pJoystick);

	for (auto& pTrigger : m_pLeftTriggers)
		SafeDelete(pTrigger);

	for (auto& pTrigger : m_pRightTriggers)
		SafeDelete(pTrigger);
}

void Proto::ControllerHandler::Update()
{
	for (int i{ XINPUT_CONTROLLER_MIN_INDEX }; i <= XINPUT_CONTROLLER_MAX_INDEX; i++)
	{
		m_OldStates[i] = m_CurrentStates[i];
		ZeroMemory(&m_CurrentStates[i], sizeof(XINPUT_STATE));
		XInputGetState(0, &m_CurrentStates[i]);
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

void Proto::ControllerHandler::Process()
{
	for (auto& upperPair : m_ControllerButtons)
	{
		for (auto& innerPair : upperPair.second)
		{
			ControllerButton* pControllerButton{ innerPair.second };
			pControllerButton->Process();
		}
	}

	for (auto& pJoystick : m_pLeftJoysticks)
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
}

bool Proto::ControllerHandler::AddInput(int index, int XInput)
{
	if (index > XINPUT_CONTROLLER_MAX_INDEX || index < XINPUT_CONTROLLER_MIN_INDEX)
	{
		std::cout << "ControllerHandler::AddInput(index, XInput) failed > XInput only allows 4 controllers maximum. (index 0 - 3)." << std::endl; // TODO: Replace by Logger
		return false;
	}

	if (m_ControllerButtons[index].find(XInput) != m_ControllerButtons[index].end()) // if map does not exist yet with index, it'll make inner map
	{
		std::cout << "ControllerHandler::AddInput(index, XInput) failed > Controller Button was already added." << std::endl; // TODO: Replace by Logger
		return false;
	}

	m_ControllerButtons[index][XInput] = new ControllerButton{ XInput };
	return true;
}

ControllerButton& Proto::ControllerHandler::GetInput(int index, int XInput)
{
	try
	{
		return *m_ControllerButtons[index].at(XInput);
	}

	catch (const std::out_of_range & exception)
	{
		UNREFERENCED_PARAMETER(exception);

		AddInput(index, XInput);
		std::cout << "ControllerHandler::GetInput(index, XInput) failed > Controller Button not found, but AddInput was called for safety. " << std::endl; // TODO: Replace by Logger

		return GetInput(index, XInput);
	}
}

ControllerJoystick& Proto::ControllerHandler::GetInput(int index, const StickState& stickState) const
{
	if (stickState == StickState::Left)
		return *m_pLeftJoysticks[index];

	return *m_pRightJoysticks[index];
}

ControllerTrigger& Proto::ControllerHandler::GetInput(int index, const TriggerPosState& triggerPosState) const
{
	if (triggerPosState == TriggerPosState::Left)
		return *m_pLeftTriggers[index];

	return *m_pRightTriggers[index];
}
