#include "ProtoEnginePCH.h"
#include "KBMHandler.h"

void KBMHandler::Init()
{
	m_pMouseButtons[0] = new MouseButton();
	m_pMouseButtons[1] = new MouseButton();
	m_pMouse = new Mouse();

	m_pDefaultKey = new Key(-1);
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
		pMouseButton->UpdateOutPoll();

	GetMouse().UpdateOutPoll();
}

void KBMHandler::UpdateInPoll(const SDL_Event& e)
{
	// Necessary to check for NO Key repeat or it'll be sending the KEYDOWN event over and over after a delay
	// https://stackoverflow.com/questions/22156815/how-to-disable-key-repeat-in-sdl2
	if ((e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) && e.key.repeat == 0)
		GetInput(e.key.keysym.sym).UpdateInPoll(e);

	if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP)
	{
		if (e.button.button == SDL_BUTTON_LEFT)
			GetMouseButton(MouseState::Left).UpdateInPoll(e);

		else if (e.button.button == SDL_BUTTON_RIGHT)
			GetMouseButton(StickState::Right).UpdateInPoll(e);
	}

	if (e.type == SDL_MOUSEMOTION)
	{
		GetMouse().UpdateInPoll(e);
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
		pMouseButton->Process();

	GetMouse().Process();
}

bool KBMHandler::AddInput(SDL_Keycode keyCode)
{
	if (m_Keys.find(keyCode) != m_Keys.end())
	{
		ProtoLogger.AddLog(LogLevel::Warning, "KBMHandler::AddInput failed > Key was already added.");
		return false;
	}

	m_Keys[keyCode] = new Key(keyCode);
	return true;
}

Key& KBMHandler::GetInput(SDL_Keycode keyCode)
{
	try
	{
		return *m_Keys.at(keyCode);
	}

	catch (const std::out_of_range & exception)
	{
		UNREFERENCED_PARAMETER(exception);

		ProtoLogger.AddLog(LogLevel::Warning, "KBMHandler::GetInput failed > Key was not found, default key has been returned.");
		return *m_pDefaultKey;
	}
}

MouseButton& KBMHandler::GetMouseButton(const MouseState& mouseState) const noexcept
{
	if (mouseState == MouseState::Left)
		return *m_pMouseButtons[0];

	return *m_pMouseButtons[1];
}

Mouse& KBMHandler::GetMouse() const noexcept
{
	return *m_pMouse;
}
