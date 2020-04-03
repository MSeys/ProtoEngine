#include "ProtoEnginePCH.h"
#include "StartScene.h"

#include "Commands.h"
#include "GameObject.h"

StartScene::StartScene(const std::wstring& sceneName)
	: BaseScene(sceneName)
{
}

void StartScene::InitializeCommands()
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
}

void StartScene::InitializeControls()
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
	ProtoInput.GetControllerJoystick(0, StickState::Left).SetCommand(JoystickState::Moving, "Joystick");
	ProtoInput.GetControllerTrigger(0, TriggerPosState::Left).SetCommand(TriggerState::Held, "Trigger");
	ProtoInput.GetMouse().SetCommand(JoystickState::Moving, "Mouse");
}

void StartScene::InitializeScene()
{
	GameObject* pGameObject{ new GameObject() };
	Proto::Texture2D* pBackground = ProtoResources.LoadTexture("background.jpg");
	Proto::Texture2D* pLogo = ProtoResources.LoadTexture("logo.png");
	Proto::Font* pTextFont = ProtoResources.LoadFont("Lingua.otf", 36);
	Proto::Font* pFPSFont = ProtoResources.LoadFont("Lingua.otf", 24);

	RenderComponent* pRenderBackground{ new RenderComponent(pBackground, Proto::TextureData{}) };
	RenderComponent* pRenderLogo{ new RenderComponent(pLogo, Proto::TextureData{ 216, 180 }) };

	TextComponent* pTextComponent{ new TextComponent("Programming 4 Assignment", pTextFont, { 80, 20 }) };
	FPSComponent* pFPSComponent{ new FPSComponent(pFPSFont, { 0, 0, -1, -1, { 255, 255, 0, 255 } }) };

	pGameObject->AddComponent(pRenderBackground);
	pGameObject->AddComponent(pRenderLogo);
	pGameObject->AddComponent(pTextComponent);
	pGameObject->AddComponent(pFPSComponent);

	AddChild(pGameObject);
}

void StartScene::Update()
{
}

void StartScene::Draw()
{
}

void StartScene::SceneActivated()
{
}

void StartScene::SceneDeactivated()
{
}