#pragma once
#include <BaseCommand.h>

class JumpCommand : public Command
{
public:
	void Execute() override
	{
		const std::string dataStr{ *static_cast<std::string*>(ExecuteData) };
		std::cout << "JumpCommand::Execute > " << InputData.StringifiedCode << " > " << dataStr << std::endl; // TODO: Replace by Logger
		ServiceLocator::GetAudio().PlayAudio(L"jump.wav");
	}
};

class FireCommand : public Command
{
public:
	void Execute() override
	{
		std::cout << "FireCommand::Execute > I set FIIIIIIIRE to the raaaain!" << std::endl; // TODO: Replace by Logger
		ServiceLocator::GetAudio().PlayAudio(L"fire.wav");
	}
};

class DuckCommand : public Command
{
public:
	void Execute() override
	{
		std::cout << "DuckCommand::Execute > HOOOOOONK! Quack! Quack!" << std::endl; // TODO: Replace by Logger
		ServiceLocator::GetAudio().PlayAudio(L"quack.wav");
	}
};

class FartCommand : public Command
{
public:
	void Execute() override
	{
		std::cout << "FartCommand::Execute > Prrrrrttt! WHO FARTED?" << std::endl; // TODO: Replace by Logger
		ServiceLocator::GetAudio().PlayAudio(L"fart.wav");
	}
};

class JoystickCommand : public Command
{
	void Execute() override
	{
		const auto pData = static_cast<PositionData*>(InputData.Data);
		std::cout << "JoystickCommand::Execute > [" << pData->x << ", " << pData->y << "]" << std::endl; // TODO: Replace by Logger
	}
};

class MouseCommand : public Command
{
	void Execute() override
	{
		const auto pData = static_cast<MouseData*>(InputData.Data);

		if (pData->LeftMouseDown)
			std::cout << "MouseCommand::Execute > Left Down | [" << pData->Position.x << ", " << pData->Position.y << "]" << std::endl; // TODO: Replace by Logger

		else if (pData->RightMouseDown)
			std::cout << "MouseCommand::Execute > Right Down | [" << pData->Position.x << ", " << pData->Position.y << "]" << std::endl; // TODO: Replace by Logger

		else
			std::cout << "MouseCommand::Execute > [" << pData->Position.x << ", " << pData->Position.y << "]" << std::endl; // TODO: Replace by Logger
	}
};

class TriggerCommand : public Command
{
	void Execute() override
	{
		const auto pData = static_cast<TriggerData*>(InputData.Data);
		std::cout << "TriggerCommand::Execute > [" << pData->value << "]" << std::endl; // TODO: Replace by Logger
	}
};

class SwitchAudioServiceCommand : public Command
{
	void Execute() override
	{
		AudioServices& audioServices{ *static_cast<AudioServices*>(ExecuteData) };
		if (&ServiceLocator::GetAudio() == &audioServices.PlayerAudio)
			ServiceLocator::RegisterAudioService(&audioServices.LogAudio);
		else
			ServiceLocator::RegisterAudioService(&audioServices.PlayerAudio);
	}
};