#pragma once
#include <iostream>

#include "Audio.h"
#include "Buttons.h"

enum class CommandOrigin
{
	C_BUTTON, C_JOYSTICK, C_TRIGGER, KBM_KEY, KBM_MOUSE, KBM_MOUSE_KEY, NONE
};

struct InputData
{
	CommandOrigin Origin{ CommandOrigin::NONE };
	std::string StringifiedCode = "NONE";
	void* Data = nullptr;
};

#pragma region Base Command
class Command
{
public:
	virtual ~Command() = default;
	virtual void Execute() = 0;
	virtual void SetExecuteData(void* data)
	{
		ExecuteData = data;
	}

	virtual void SetInputData(const CommandOrigin& origin, const std::string& code, void* data)
	{
		InputData = { origin, code, data };
	}

protected:
	void* ExecuteData = nullptr;
	InputData InputData;
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
		*static_cast<bool*>(ExecuteData) = true;
	}
};