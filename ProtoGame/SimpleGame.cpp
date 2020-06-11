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
	ProtoInput.AddInput(0, XINPUT_GAMEPAD_A);
	ProtoInput.GetInput(0, XINPUT_GAMEPAD_A).SetCommand(ButtonState::Pressed, "Jump");

	ProtoInput.AddInput(0, XINPUT_GAMEPAD_B);
	ProtoInput.GetInput(0, XINPUT_GAMEPAD_B).SetCommand(ButtonState::Pressed, "Fire");

	ProtoInput.AddInput(0, XINPUT_GAMEPAD_Y);
	ProtoInput.GetInput(0, XINPUT_GAMEPAD_Y).SetCommand(ButtonState::Pressed, "Duck");

	ProtoInput.AddInput(0, XINPUT_GAMEPAD_X);
	ProtoInput.GetInput(0, XINPUT_GAMEPAD_X).SetCommand(ButtonState::Pressed, "Fart");

	ProtoInput.AddInput(SDLK_t);
	ProtoInput.GetInput(SDLK_t).SetCommand(ButtonState::Pressed, "SwitchAudioService");

	/* Extra Inputs */
	//ProtoInput.GetControllerJoystick(0, StickState::Left).SetCommand(JoystickState::Moving, "Joystick"); // Using old separate command
	//ProtoInput.GetMouse().SetCommand(JoystickState::Moving, "Mouse"); // Using old separate command
	ProtoInput.GetInput(0, StickState::Left).SetCommand(JoystickState::Moving, "Motion");
	ProtoInput.GetMouse().SetCommand(JoystickState::Moving, "Motion");

	ProtoInput.GetInput(0, TriggerPosState::Left).SetCommand(TriggerState::Held, "Trigger");

	/* Key Held Test */
	ProtoInput.AddInput(SDLK_a);
	ProtoInput.GetInput(SDLK_a).SetCommand(ButtonState::Pressed, "Jump");
	ProtoInput.GetInput(SDLK_a).SetCommand(ButtonState::Released, "Fart");

	ProtoInput.GetMouseButton(MouseState::Left).SetCommand(ButtonState::Pressed, "Jump");
	ProtoInput.GetMouseButton(MouseState::Left).SetCommand(ButtonState::Released, "Duck");
}

void SimpleGame::InitializeScene()
{
	ProtoMessenger.AddObserver(new AchievementObserver({}), "Achievement");
	ProtoAchievements(bool).AddAchievement(new ConditionAchievement("Ah, you're finally awake!", "You started up the game for the first time.", true));
	ProtoMessenger.Notify(nullptr, E_GAME_STARTED);
	ProtoLogger.AddLog(LogLevel::Info, "Testing Logging.");

	ProtoPhysics.SetPixelsPerMeter(100);
	ProtoScenes.Add(new Scene("\\Scenes\\test.protoscene"));
	ProtoScenes.SetCurrentScene(L"Test");
}

void SimpleGame::DrawAddComponent(GameObject* pCurrentSelected)
{
	UNREFERENCED_PARAMETER(pCurrentSelected);
	
	ImGui::Text("SimpleGame Comp 1");
	ImGui::Text("SimpleGame Comp 2");
}

void SimpleGame::LoadComponents(rapidxml::xml_node<>* pComponents, GameObject* pCurr)
{
	UNREFERENCED_PARAMETER(pComponents);
	UNREFERENCED_PARAMETER(pCurr);
}