#include "ProtoEnginePCH.h"
#include "BB_PickupController.h"

#include "BB_PlayerController.h"

void BB_PickupController::DrawInspectorTitle()
{
	ImGui::Text("BB - Pickup Controller");
}

void BB_PickupController::DrawInspector()
{
	ImGui::Text("Internal Runtime Usage");
}

void BB_PickupController::Start()
{
}

void BB_PickupController::Awake()
{
	GameObject* pGameModeObject{ Gameobject.GetScene()->FindGameObjectInRootWithName("GameMode") };
	if (pGameModeObject)
		m_pGameMode = pGameModeObject->GetComponent<BB_GameMode>();
}

void BB_PickupController::OnTriggerEnter(const Collision& collision)
{
	if(collision.other == m_pGameMode->GetPlayerOne()->GetGameObject())
	{
		Gameobject.Destroy();
		return;
	}

	if(m_pGameMode->HasPlayerTwo())
	{
		if (collision.other == m_pGameMode->GetPlayerTwo()->GetGameObject())
		{
			Gameobject.Destroy();
			return;
		}
	}
}

void BB_PickupController::PreSolveCollision(const Collision& collision)
{
	collision.contact->SetEnabled(collision.other->GetName() == "Floors");
}
