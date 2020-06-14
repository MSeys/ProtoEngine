#pragma once
#include <iostream>

#include "Editor.h"

enum class CommandOrigin
{
	C_BUTTON, C_JOYSTICK, C_TRIGGER, KBM_KEY, KBM_MOUSE, KBM_MOUSE_KEY, NONE
};

struct InputData
{
	CommandOrigin Origin{ CommandOrigin::NONE };
	SDL_Keycode SDLKeycode{ -1 };
	XINPUT_Keycode XINPUTKeycode{ XINPUT_Keycode(-1) };
	void* Data = nullptr;
};

#pragma region Base Command
class Command
{
public:
	virtual ~Command() = default;
	virtual void Execute() = 0;
	void SetExecuteData(void* pData)
	{
		EData = pData;
	}

	void SetInputData(const CommandOrigin& origin, SDL_Keycode sdlKeycode, XINPUT_Keycode xinputKeycode, void* pData)
	{
		IData = { origin, sdlKeycode, xinputKeycode, pData };
	}

protected:
	/**
	 * \brief
	 * Execution Data (EData) is used to store anything that you want to change within a derived Command. \n
	 * Execution Data is a void* and requires casting. (VoidCast<T>(...) might help) \n
	 * Execution Data is set by using SetExecuteData(...) on Command. \n
	 * Execution Data can only be directly accessed within the Execute(...) function of a derived Command.
	 */
	void* EData = nullptr;

	/**
	 * \brief
	 * Input Data (IData) is used to store input information given by the Input Manager. \n
	 * Input Data contains:
	 * - Origin (CommandOrigin Enum Class)
	 * - SDL_Keycode (SDLK_xxx)
	 * * XINPUT_Keycode (XINPUT_GAMEPAD_xxx)
	 * - Data (void* containing specific data. E.g. Position of Joystick or more info for Mouse)
	 */
	InputData IData;
};
#pragma endregion Base Command

class DefaultCommand : public Command
{
public:
	void Execute() override
	{
		//std::cout << "DefaultCommand::Execute > Default Command was triggered." << std::endl; // TODO: Replace by Logger
	}
};

class ExitCommand : public Command
{
	void Execute() override
	{
		if(ProtoSettings.GetRenderMode() == RenderMode::GAME)
			*static_cast<bool*>(EData) = true;

		else
		{
			if(ProtoSettings.GetEditorMode() == EditorMode::EDIT)
				*static_cast<bool*>(EData) = true;

			else
			{
				Proto::Editor::GetInstance().ReturnToEditMode();
				ProtoSettings.SetEditorMode(EditorMode::EDIT);
			}
		}
	}
};