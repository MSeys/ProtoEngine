#include "ProtoEnginePCH.h"
#include "LineCollider2D.h"

LineCollider2D::LineCollider2D(ComponentID ID, bool isActive, const glm::vec2& posA, const glm::vec2& posB, float density, float friction, float restitution, bool isTrigger)
	: Collider2D(ID, isActive, density, friction, restitution, isTrigger)
	, m_PosA(posA), m_PosB(posB)
{
}

void LineCollider2D::DrawInspectorTitle()
{
	DrawActiveCheckbox();
	ImGui::Text("Line Collider 2D");
}

void LineCollider2D::DrawInspector()
{
	ProtoGui::Presets::Position(m_PosA.x, m_PosA.y, 0, " A");
	ProtoGui::Presets::Position(m_PosB.x, m_PosB.y, 1, " B");
	Collider2D::DrawInspector();
}

void LineCollider2D::DrawEditorDebug()
{
	ProtoRenderer.RenderLine(GetTransform()->GetPosition() + m_PosA, GetTransform()->GetPosition() + m_PosB, { 0, 255, 0, 255 });
}

void LineCollider2D::Start()
{
	const auto pRigidBody{ Gameobject.GetComponent<RigidBody2D>() };
	if (!pRigidBody)
	{
		ProtoLogger.AddLog(LogLevel::Warning, "No RigidBody2D found on GameObject \'"
			+ Gameobject.GetName() +
			"\', LineCollider2D (ID: " + std::to_string(GetID()) + ") was not created.");
		return;
	}

	if (!pRigidBody->IsReady())
		pRigidBody->ReadyFromCollision();

	b2EdgeShape edgeShape;
	const auto pointA{ m_PosA }, pointB{ m_PosB };
	edgeShape.Set(ProtoConvert::ToBox2DVec(pointA / float(ProtoPhysics.GetPPM())), ProtoConvert::ToBox2DVec(pointB / float(ProtoPhysics.GetPPM())));

	b2FixtureDef def;
	def.shape = &edgeShape;
	def.density = m_Density;
	def.friction = m_Friction;
	def.restitution = m_Restitution;
	def.isSensor = m_IsTrigger;
	def.userData = this;
	m_pCollision = pRigidBody->GetBody()->CreateFixture(&def);
}

void LineCollider2D::Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent)
{
	using namespace rapidxml;
	xml_node<>* pComp = doc.allocate_node(node_element, "LineCollider2D");

	SaveID(doc, pComp);
	SaveActive(doc, pComp);

	ProtoSaver::XML::Save<float>("PointAX", m_PosA.x, doc, pComp);
	ProtoSaver::XML::Save<float>("PointAY", m_PosA.y, doc, pComp);
	ProtoSaver::XML::Save<float>("PointBX", m_PosB.x, doc, pComp);
	ProtoSaver::XML::Save<float>("PointBY", m_PosB.y, doc, pComp);
	
	Collider2D::Save(doc, pComp);

	pParent->append_node(pComp);
}
