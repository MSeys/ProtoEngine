#pragma once
#include <BaseCommand.h>
#include "glm/glm.hpp"

class JumpCommand : public Command
{
public:
	void Execute() override
	{
		auto& dataStr{ VoidCast<std::string>(EData) };
		std::cout << "JumpCommand::Execute > " << std::boolalpha << CompStringify(IData.StringCode, XINPUT_GAMEPAD_A) << IData.StringCode << " > " << dataStr << std::endl; // TODO: Replace by Logger
		ServiceLocator::GetAudio().PlayAudio(L"jump.wav");
		dataStr = "Boing V2.0";
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
		const auto pData = static_cast<PositionData*>(IData.Data);
		std::cout << "JoystickCommand::Execute > [" << pData->x << ", " << pData->y << "]" << std::endl; // TODO: Replace by Logger
	}
};

class MouseCommand : public Command
{
	void Execute() override
	{
		const auto pData = static_cast<MouseData*>(IData.Data);

		if (pData->LeftMouseDown)
			std::cout << "MouseCommand::Execute > Left Down | [" << pData->Position.x << ", " << pData->Position.y << "]" << std::endl; // TODO: Replace by Logger

		else if (pData->RightMouseDown)
			std::cout << "MouseCommand::Execute > Right Down | [" << pData->Position.x << ", " << pData->Position.y << "]" << std::endl; // TODO: Replace by Logger

		else
			std::cout << "MouseCommand::Execute > [" << pData->Position.x << ", " << pData->Position.y << "]" << std::endl; // TODO: Replace by Logger
	}
};

class MotionCommand : public Command
{
	void Execute() override
	{
		glm::vec2 position{};
		std::string extra{};
		
		if (IData.Origin == CommandOrigin::C_JOYSTICK)
		{
			const auto& data{ VoidCast<PositionData>(IData.Data) };
			position = { data.x, data.y };
		}

		else if (IData.Origin == CommandOrigin::KBM_MOUSE)
		{
			const auto& data{ VoidCast<MouseData>(IData.Data) };
			position = { data.Position.x, data.Position.y };

			if (data.LeftMouseDown)
				extra = "Left Down |";

			if (data.RightMouseDown)
				extra = "Right Down |";
		}

		std::cout << "MotionCommand::Execute > " << extra << " [" << position.x << ", " << position.y << "]" << std::endl; // TODO: Replace by Logger	
	}
};

class TriggerCommand : public Command
{
	void Execute() override
	{
		const auto pData = static_cast<TriggerData*>(IData.Data);
		std::cout << "TriggerCommand::Execute > [" << pData->value << "]" << std::endl; // TODO: Replace by Logger
	}
};

class SwitchAudioServiceCommand : public Command
{
	void Execute() override
	{
		AudioServices& audioServices{ *static_cast<AudioServices*>(EData) };
		if (&ServiceLocator::GetAudio() == &audioServices.PlayerAudio)
			ServiceLocator::RegisterAudioService(&audioServices.LogAudio);
		else
			ServiceLocator::RegisterAudioService(&audioServices.PlayerAudio);
	}
};