#include "ProtoEnginePCH.h"
#include "KBMHandler.h"

void KBMHandler::Init()
{
	m_pMouseButtons[0] = new MouseButton();
	m_pMouseButtons[1] = new MouseButton();
	m_pMouse = new Mouse();

	m_pDefaultKey = new Key("UNDEFINED");
}

void KBMHandler::Destroy()
{
	for (auto& pair : m_Keys)
		SafeDelete(pair.second);

	for (auto& pMouseButton : m_pMouseButtons)
		SafeDelete(pMouseButton);

	SafeDelete(m_pMouse);

	SafeDelete(m_pDefaultKey);
}

void KBMHandler::UpdateOutPoll()
{
	for (auto& pair : m_Keys)
	{
		Key* pKey{ pair.second };
		pKey->UpdateOutPoll();
	}

	for (auto& pMouseButton : m_pMouseButtons)
	{
		pMouseButton->UpdateOutPoll();
	}

	GetInput().UpdateOutPoll();
}

void KBMHandler::UpdateInPoll(const SDL_Event& e)
{
	if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
	{
		GetInput(e.key.keysym.sym, false).UpdateInPoll(e);
	}

	if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP)
	{
		if (e.button.button == SDL_BUTTON_LEFT)
			GetInput(StickState::Left).UpdateInPoll(e);

		else if (e.button.button == SDL_BUTTON_RIGHT)
			GetInput(StickState::Right).UpdateInPoll(e);
	}

	if (e.type == SDL_MOUSEMOTION)
	{
		GetInput().UpdateInPoll(e);
	}
}

void KBMHandler::Process()
{
	for (auto& pair : m_Keys)
	{
		Key* pKey{ pair.second };
		pKey->Process();
	}

	for (auto& pMouseButton : m_pMouseButtons)
	{
		pMouseButton->Process();
	}

	GetInput().Process();
}

bool KBMHandler::AddInput(int SDLKey, const std::string& stringifiedSDLKey)
{
	if (m_Keys.find(SDLKey) != m_Keys.end())
	{
		std::cout << "KBMHandler::AddInput(SDLKey) failed > Key was already added." << std::endl; // TODO: Replace by Logger
		return false;
	}

	m_Keys[SDLKey] = new Key(stringifiedSDLKey);
	return true;
}

Key& KBMHandler::GetInput(int SDLKey, bool doesCreate)
{
	try
	{
		return *m_Keys.at(SDLKey);
	}

	catch (const std::out_of_range & exception)
	{
		UNREFERENCED_PARAMETER(exception);
		if (doesCreate)
		{
			AddInput(SDLKey, "UNDEFINED");
			std::cout << "KBMHandler::GetInput(SDLKey, ...) failed > Key was not found, but has been created. Use AddKey first on the desired key." << std::endl; // TODO: Replace by Logger

			return GetInput(SDLKey);
		}

		std::cout << "KBMHandler::GetInput(SDLKey, ...) failed > Key was not found, default key has been returned." << std::endl; // TODO: Replace by Logger
		return *m_pDefaultKey;
	}
}

MouseButton& KBMHandler::GetInput(const StickState& mouseState) const noexcept
{
	if (mouseState == StickState::Left)
		return *m_pMouseButtons[0];

	return *m_pMouseButtons[1];
}

Mouse& KBMHandler::GetInput() const noexcept
{
	return *m_pMouse;
}
