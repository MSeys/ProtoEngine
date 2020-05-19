#include "ProtoEnginePCH.h"
#include "BoxCollider2D.h"

BoxCollider2D::BoxCollider2D(ComponentID ID, bool isActive, const glm::vec2& collisionSize, float density, float friction, float restitution, bool isTrigger)
	: Collider2D(ID, isActive, density, friction, restitution, isTrigger)
	, m_CollisionSize(collisionSize)
{
}

void BoxCollider2D::DrawInspectorTitle()
{
	DrawActiveCheckbox();
	ImGui::Text("Box Collider 2D");
}

void BoxCollider2D::DrawInspector()
{
	ProtoGui::Presets::Size(m_CollisionSize.x, m_CollisionSize.y);
	Collider2D::DrawInspector();
}

void BoxCollider2D::DrawEditorDebug()
{
	SDL_Rect rect;
	rect.x = int(GetTransform()->GetPosition().x - m_CollisionSize.x);
	rect.y = int(GetTransform()->GetPosition().y - m_CollisionSize.y);
	rect.w = int(m_CollisionSize.x * 2);
	rect.h = int(m_CollisionSize.y * 2);
	ProtoRenderer.RenderLineRect(rect, { 0, 255, 0, 255 });
}

void BoxCollider2D::Start()
{
	const auto pRigidBody{ Gameobject.GetComponent<RigidBody2D>() };
	if (!pRigidBody)
	{
		ProtoLogger.AddLog(LogLevel::Warning, "No RigidBody2D found on GameObject \'"
			+ Gameobject.GetName() +
			"\', BoxCollider2D (ID: " + std::to_string(GetID()) + ") was not created.");
		return;
	}

	if (!pRigidBody->IsReady())
		pRigidBody->ReadyFromCollision();

	b2PolygonShape boxShape;
	boxShape.SetAsBox(float(m_CollisionSize.x) / float(ProtoPhysics.GetPPM()), float(m_CollisionSize.y) / float(ProtoPhysics.GetPPM()));

	b2FixtureDef def;
	def.shape = &boxShape;
	def.density = m_Density;
	def.friction = m_Friction;
	def.restitution = m_Restitution;
	def.isSensor = m_IsTrigger;
	def.userData = this;
	m_pCollision = pRigidBody->GetBody()->CreateFixture(&def);
}

void BoxCollider2D::Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent)
{
	using namespace rapidxml;
	xml_node<>* pComp = doc.allocate_node(node_element, "BoxCollider2D");

	SaveID(doc, pComp);
	SaveActive(doc, pComp);
	
	ProtoSaver::XML::Save<float>("CollisionSizeX", m_CollisionSize.x, doc, pComp);
	ProtoSaver::XML::Save<float>("CollisionSizeY", m_CollisionSize.y, doc, pComp);

	Collider2D::Save(doc, pComp);

	pParent->append_node(pComp);
}
