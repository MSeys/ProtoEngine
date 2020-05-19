#include "ProtoEnginePCH.h"
#include "SphereCollider2D.h"

SphereCollider2D::SphereCollider2D(ComponentID ID, bool isActive, glm::vec2 position, float radius, float density, float friction, float restitution, bool isTrigger)
	: Collider2D(ID, isActive, density, friction, restitution, isTrigger)
	, m_Position(position), m_Radius(radius)
{
}

void SphereCollider2D::DrawInspectorTitle()
{
	DrawActiveCheckbox();
	ImGui::Text("Sphere Collider 2D");
}

void SphereCollider2D::DrawInspector()
{
	const ProtoGui::ProtoGuiData pgData{ true, 100, -1, true, 70 };
	const ProtoGui::DragData dragData{ 0.1f, 0, 0, "%.1f" };

	ProtoGui::Presets::Position(m_Position.x, m_Position.y);
	ProtoGui::Drag<float>("Radius", pgData, "##RADIUS", m_Radius, dragData);
	Collider2D::DrawInspector();
}

void SphereCollider2D::DrawEditorDebug()
{
	const glm::vec2 circlePos = GetTransform()->GetPosition() + m_Position;
	ProtoRenderer.RenderLineCircle(circlePos, m_Radius, { 0, 255, 0, 255 });
}

void SphereCollider2D::Start()
{
	const auto pRigidBody{ Gameobject.GetComponent<RigidBody2D>() };
	if (!pRigidBody)
	{
		ProtoLogger.AddLog(LogLevel::Warning, "No RigidBody2D found on GameObject \'"
			+ Gameobject.GetName() +
			"\', SphereCollider2D (ID: " + std::to_string(GetID()) + ") was not created.");
		return;
	}

	if (!pRigidBody->IsReady())
		pRigidBody->ReadyFromCollision();

	b2CircleShape circleShape;
	circleShape.m_p.Set(float(m_Position.x) / float(ProtoPhysics.GetPPM()), float(m_Position.y) / float(ProtoPhysics.GetPPM()));
	circleShape.m_radius = m_Radius / float(ProtoPhysics.GetPPM());

	b2FixtureDef def;
	def.shape = &circleShape;
	def.density = m_Density;
	def.friction = m_Friction;
	def.restitution = m_Restitution;
	def.isSensor = m_IsTrigger;
	def.userData = this;
	m_pCollision = pRigidBody->GetBody()->CreateFixture(&def);
}

void SphereCollider2D::Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent)
{
	using namespace rapidxml;
	xml_node<>* pComp = doc.allocate_node(node_element, "SphereCollider2D");

	SaveID(doc, pComp);
	SaveActive(doc, pComp);

	ProtoSaver::XML::Save<float>("PositionX", m_Position.x, doc, pComp);
	ProtoSaver::XML::Save<float>("PositionY", m_Position.y, doc, pComp);
	ProtoSaver::XML::Save<float>("Radius", m_Radius, doc, pComp);

	Collider2D::Save(doc, pComp);

	pParent->append_node(pComp);
}
