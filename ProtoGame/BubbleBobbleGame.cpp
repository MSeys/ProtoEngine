#include "ProtoEnginePCH.h"
#include "BubbleBobbleGame.h"

#include "BB_MenuController.h"
#include "BB_PlayerController.h"
#include "BB_ZenChanController.h"

void BubbleBobbleGame::InitializeScene()
{
	ProtoPhysics.SetPixelsPerMeter(100);
	ProtoScenes.Add(new Scene("\\Scenes\\BubbleBobble_Menu.protoscene"));
	ProtoScenes.Add(new Scene("\\Scenes\\BubbleBobble_SP.protoscene"));
	ProtoScenes.Add(new Scene("\\Scenes\\BubbleBobble_MP.protoscene"));
	ProtoScenes.SetCurrentScene(L"BubbleBobble_Menu");
}

void BubbleBobbleGame::DrawAddComponent(GameObject* pCurrentSelected)
{
	if (ImGui::Selectable("GameMode Controller"))
		pCurrentSelected->AddComponent(new BB_GameMode(pCurrentSelected->RequestNewID(), true, -1, -1));
	
	if (ImGui::Selectable("Main Menu Controller"))
		pCurrentSelected->AddComponent(new BB_MenuController(pCurrentSelected->RequestNewID(), true));

	if (ImGui::Selectable("Player Controller"))
		pCurrentSelected->AddComponent(new BB_PlayerController(pCurrentSelected->RequestNewID(), true, 0));

	if (ImGui::Selectable("Zen-Chan Controller"))
		pCurrentSelected->AddComponent(new BB_ZenChanController(pCurrentSelected->RequestNewID(), true));
}

void BubbleBobbleGame::LoadComponents(rapidxml::xml_node<>* pComponents, GameObject* pCurr)
{
	ProtoParser::XML::Load<BB_MenuController>(pComponents, pCurr, false);
	ProtoParser::XML::Load<BB_PlayerController>(pComponents, pCurr, false);
	ProtoParser::XML::Load<BB_ZenChanController>(pComponents, pCurr, false);
	ProtoParser::XML::Load<BB_GameMode>(pComponents, pCurr, false);
}