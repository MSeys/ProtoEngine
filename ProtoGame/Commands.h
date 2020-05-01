#pragma once
#include <BaseCommand.h>
#include "glm/glm.hpp"

class JumpCommand : public Command
{
public:
	void Execute() override
	{
		auto& dataStr{ VoidCast<std::string>(EData) };
		ProtoLogger.AddLog(LogLevel::Debug, "JumpCommand::Execute > " + IData.StringCode + " > " + dataStr);
		ProtoAudio::GetAudio().PlayAudio(L"jump.wav");
		dataStr = "Boing V2.0";
	}
};

class FireCommand : public Command
{
public:
	void Execute() override
	{
		ProtoLogger.AddLog(LogLevel::Debug, "FireCommand::Execute > I set FIIIIIIIRE to the raaaain!");
		ProtoAudio::GetAudio().PlayAudio(L"fire.wav");
	}
};

class DuckCommand : public Command
{
public:
	void Execute() override
	{
		ProtoLogger.AddLog(LogLevel::Debug, "DuckCommand::Execute > HOOOOOONK! Quack! Quack!");
		ProtoAudio::GetAudio().PlayAudio(L"quack.wav");
	}
};

class FartCommand : public Command
{
public:
	void Execute() override
	{
		ProtoLogger.AddLog(LogLevel::Debug, "FartCommand::Execute > Prrrrrttt! WHO FARTED?");
		ProtoAudio::GetAudio().PlayAudio(L"fart.wav");
	}
};

class JoystickCommand : public Command // Old Separate Command
{
	void Execute() override
	{
		const auto& data{ VoidCast<PositionData>(IData.Data) };
		ProtoLogger.AddLog(LogLevel::Debug, "JoystickCommand::Execute > [" + std::to_string(data.x) + ", " + std::to_string(data.y) + "]");
	}
};

class MouseCommand : public Command // Old Separate Command
{
	void Execute() override
	{
		const auto& data{ VoidCast<MouseData>(IData.Data) };

		if (data.LeftMouseDown)
			ProtoLogger.AddLog(LogLevel::Debug, "MouseCommand::Execute > Left Down | [" + std::to_string(data.Position.x) + ", " + std::to_string(data.Position.y) + "]");

		else if (data.RightMouseDown)
			ProtoLogger.AddLog(LogLevel::Debug, "MouseCommand::Execute > Right Down | [" + std::to_string(data.Position.x) + ", " + std::to_string(data.Position.y) + "]");

		else
			ProtoLogger.AddLog(LogLevel::Debug, "MouseCommand::Execute > [" + std::to_string(data.Position.x) + ", " + std::to_string(data.Position.y) + "]");
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

		ProtoLogger.AddLog(LogLevel::Debug, "MotionCommand::Execute > " + extra + " [" + std::to_string(position.x) + ", " + std::to_string(position.y) + "]");
	}
};

class TriggerCommand : public Command
{
	void Execute() override
	{
		const auto& data{ VoidCast<TriggerData>(IData.Data) };
		ProtoLogger.AddLog(LogLevel::Debug, "TriggerCommand::Execute > [" + std::to_string(data.value) + "]");
	}
};

class SwitchAudioServiceCommand : public Command
{
	void Execute() override
	{
		AudioServices& audioServices{ *static_cast<AudioServices*>(EData) };
		if (&ProtoAudio::GetAudio() == &audioServices.PlayerAudio)
			ProtoAudio::RegisterAudioService(&audioServices.LogAudio);
		else
			ProtoAudio::RegisterAudioService(&audioServices.PlayerAudio);
	}
};