#include "ProtoEnginePCH.h"
#include "BB_BubbleController.h"

void BB_BubbleController::DrawInspectorTitle()
{
	ImGui::Text("BB - Bubble Controller");
}

void BB_BubbleController::DrawInspector()
{
	ImGui::Text("Internal Runtime Usage");
}

void BB_BubbleController::Awake()
{
	RigidBody2D* pRigidBody = Gameobject.GetComponent<RigidBody2D>();
	if (!pRigidBody)
		return;
	
	pRigidBody->GetBody()->ApplyForce({ (m_IsLookingLeft ? -1.f : 1.f) * m_BubbleInitialForce, 0 }, pRigidBody->GetBody()->GetWorldCenter(), true);
}

void BB_BubbleController::Update()
{
	m_BubbleHealth -= ProtoTime.DeltaTime;

	if (m_BubbleHealth <= 0.0f)
		Gameobject.Destroy();
}

void BB_BubbleController::OnCollisionEnter(const Collision& collision)
{
	if(collision.other->GetName().find("Enemy") != std::string::npos)
		Gameobject.Destroy();
}
