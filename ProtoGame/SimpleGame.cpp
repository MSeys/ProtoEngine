#include "ProtoEnginePCH.h"

#include "SimpleGame.h"

#include "AchievementObserver.h"
#include "Commands.h"
#include "Events.h"

void SimpleGame::InitializeCommands()
{
	ProtoCommands.AddCommand(new JumpCommand(), "Jump");
	ProtoCommands.GetCommand("Jump").SetExecuteData(&m_JumpDataExample); // Sets data that can be used in Execute
	ProtoCommands.AddCommand(new FireCommand(), "Fire");
	ProtoCommands.AddCommand(new DuckCommand(), "Duck");
	ProtoCommands.AddCommand(new FartCommand(), "Fart");
	ProtoCommands.AddCommand(new SwitchAudioServiceCommand(), "SwitchAudioService");

	ProtoAudio::RegisterAudioService(&m_AudioServices.LogAudio);
	ProtoCommands.GetCommand("SwitchAudioService").SetExecuteData(&m_AudioServices);

	/* Extra Commands */
	ProtoCommands.AddCommand(new JoystickCommand(), "Joystick");
	ProtoCommands.AddCommand(new MouseCommand(), "Mouse");
	ProtoCommands.AddCommand(new TriggerCommand(), "Trigger");

	ProtoCommands.AddCommand(new MotionCommand(), "Motion");
}

void SimpleGame::InitializeControls()
{
	ProtoInput.AddControllerButton(0, XINPUT_GAMEPAD_A, STRINGIFY(XINPUT_GAMEPAD_A));
	ProtoInput.GetControllerButton(0, XINPUT_GAMEPAD_A).SetCommand(ButtonState::Pressed, "Jump");

	ProtoInput.AddControllerButton(0, XINPUT_GAMEPAD_B, STRINGIFY(XINPUT_GAMEPAD_B));
	ProtoInput.GetControllerButton(0, XINPUT_GAMEPAD_B).SetCommand(ButtonState::Pressed, "Fire");

	ProtoInput.AddControllerButton(0, XINPUT_GAMEPAD_Y, STRINGIFY(XINPUT_GAMEPAD_Y));
	ProtoInput.GetControllerButton(0, XINPUT_GAMEPAD_Y).SetCommand(ButtonState::Pressed, "Duck");

	ProtoInput.AddControllerButton(0, XINPUT_GAMEPAD_X, STRINGIFY(XINPUT_GAMEPAD_X));
	ProtoInput.GetControllerButton(0, XINPUT_GAMEPAD_X).SetCommand(ButtonState::Pressed, "Fart");

	ProtoInput.AddKey(SDLK_t, STRINGIFY(SDLK_t));
	ProtoInput.GetKey(SDLK_t).SetCommand(ButtonState::Pressed, "SwitchAudioService");

	/* Extra Inputs */
	//ProtoInput.GetControllerJoystick(0, StickState::Left).SetCommand(JoystickState::Moving, "Joystick"); // Using old separate command
	//ProtoInput.GetMouse().SetCommand(JoystickState::Moving, "Mouse"); // Using old separate command
	ProtoInput.GetControllerJoystick(0, StickState::Left).SetCommand(JoystickState::Moving, "Motion");
	ProtoInput.GetMouse().SetCommand(JoystickState::Moving, "Motion");

	ProtoInput.GetControllerTrigger(0, TriggerPosState::Left).SetCommand(TriggerState::Held, "Trigger");


	/* Key Held Test */
	ProtoInput.AddKey(SDLK_a, STRINGIFY(SDLK_a));
	ProtoInput.GetKey(SDLK_a).SetCommand(ButtonState::Pressed, "Jump");
	ProtoInput.GetKey(SDLK_a).SetCommand(ButtonState::Released, "Fart");

	ProtoInput.GetMouseButton(StickState::Left).SetCommand(ButtonState::Pressed, "Jump");
	ProtoInput.GetMouseButton(StickState::Left).SetCommand(ButtonState::Released, "Duck");
}

void SimpleGame::InitializeScene()
{
	ProtoMessenger.AddObserver(new AchievementObserver({}), "Achievement");
	ProtoAchievements(bool).AddAchievement(new ConditionAchievement("Ah, you're finally awake!", "You started up the game for the first time.", true));
	ProtoMessenger.Notify(nullptr, E_GAME_STARTED);
	ProtoLogger.AddLog(LogLevel::Info, "Testing Logging.");

	ProtoScenes.AddGameScene(new Scene("\\Scenes\\test.protoscene"));
}

void SimpleGame::DrawAddComponent()
{
	ImGui::Text("SimpleGame Comp 1");
	ImGui::Text("SimpleGame Comp 2");
}

void SimpleGame::LoadComponents(rapidxml::xml_node<>* pComponents, GameObject* pCurr)
{
	UNREFERENCED_PARAMETER(pComponents);
	UNREFERENCED_PARAMETER(pCurr);
}