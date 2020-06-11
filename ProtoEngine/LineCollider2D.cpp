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
	ProtoGui::Presets::InputXY({ "Position A", "X", "Y" }, m_PosA.x, m_PosA.y, { 1.f, 0, 0, "%.0f" }, 0);
	ProtoGui::Presets::InputXY({ "Position B", "X", "Y" }, m_PosB.x, m_PosB.y, { 1.f, 0, 0, "%.0f" }, 1);
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

	xml_node<>* pComp = doc.allocate_node(node_element, doc.allocate_string(ProtoConvert::ToString<LineCollider2D>().c_str()));

	SaveID(doc, pComp);
	SaveActive(doc, pComp);

	ProtoSaver::XML::Save<float>("PointAX", m_PosA.x, doc, pComp);
	ProtoSaver::XML::Save<float>("PointAY", m_PosA.y, doc, pComp);
	ProtoSaver::XML::Save<float>("PointBX", m_PosB.x, doc, pComp);
	ProtoSaver::XML::Save<float>("PointBY", m_PosB.y, doc, pComp);
	
	Collider2D::Save(doc, pComp);

	pParent->append_node(pComp);
}

void LineCollider2D::Load(rapidxml::xml_node<>* pComp, GameObject* pCurr)
{
	const auto id{ ProtoParser::XML::Parse<unsigned int>(pComp, "ID") };
	const auto isActive{ ProtoParser::XML::Parse<bool>(pComp, "Active") };

	const glm::vec2 posA{ ProtoParser::XML::Parse<float>(pComp, "PointAX"),  ProtoParser::XML::Parse<float>(pComp, "PointAY") };
	const glm::vec2 posB{ ProtoParser::XML::Parse<float>(pComp, "PointBX"),  ProtoParser::XML::Parse<float>(pComp, "PointBY") };

	const auto density{ ProtoParser::XML::Parse<float>(pComp, "Density") };
	const auto friction{ ProtoParser::XML::Parse<float>(pComp, "Friction") };
	const auto restitution{ ProtoParser::XML::Parse<float>(pComp, "Restitution") };
	const auto isTrigger{ ProtoParser::XML::Parse<bool>(pComp, "IsTrigger") };

	const auto pLineCollider = new LineCollider2D(id, isActive, posA, posB, density, friction, restitution, isTrigger);
	pCurr->AddComponent(pLineCollider);
}
