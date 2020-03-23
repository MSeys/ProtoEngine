#pragma once
#include <iostream>

#include "Audio.h"
#include "Buttons.h"

#pragma region Base Command
class Command
{
public:
	virtual ~Command() = default;
	virtual void Execute() = 0;
	virtual void SetExecuteData(void* data)
	{
		Data = data;
	}

	virtual void SetInputData(void* inputData)
	{
		InputData = inputData;
	}

protected:
	void* Data = nullptr;
	void* InputData = nullptr;
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
		*static_cast<bool*>(Data) = true;
	}
};