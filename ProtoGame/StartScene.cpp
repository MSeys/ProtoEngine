#include "ProtoEnginePCH.h"
#include "StartScene.h"

#include "GameObject.h"


StartScene::StartScene(const std::wstring& sceneName)
	: BaseScene(sceneName)
{
}

void StartScene::Initialize()
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
