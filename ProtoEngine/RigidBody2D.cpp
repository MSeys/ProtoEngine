#include "ProtoEnginePCH.h"
#include "RigidBody2D.h"

RigidBody2D::RigidBody2D(ComponentID ID, RigidBodyType bodyType)
	: BaseBehaviour(ID, true), m_BodyType(bodyType)
{
}

void RigidBody2D::DrawInspectorTitle()
{
	ImGui::Text("Rigid Body");
}

void RigidBody2D::DrawInspector()
{
	auto bodyType{ static_cast<int>(m_BodyType) };

	ImGui::Text("Body Type: ");
	ImGui::SameLine();
	
	ImGui::PushItemWidth(100);
	ImGui::Combo("", &bodyType, "Static\0Dynamic\0Kinematic\0");
	ImGui::PopItemWidth();
	m_BodyType = static_cast<RigidBodyType>(bodyType);

	ImGui::Text("Locked Rotation: ");
	ImGui::SameLine();

	auto fixedRotation = m_FixedRotation;
	ImGui::Checkbox("(Unlocked is unsupported)", &fixedRotation);
}

void RigidBody2D::Start()
{
	if (m_IsReady)
		return;

	b2BodyDef def;
	switch (m_BodyType)
	{
	case RigidBodyType::STATIC:
		def.type = b2_staticBody;
		break;
	case RigidBodyType::DYNAMIC:
		def.type = b2_dynamicBody;
		break;
	case RigidBodyType::KINEMATIC:
		def.type = b2_kinematicBody;
		break;
	default:;
	}

	def.position = ProtoConvert::ToBox2DVec(GetTransform()->GetPosition() / float(ProtoPhysics.GetPPM()));
	def.fixedRotation = m_FixedRotation;
	m_pBody = ProtoPhysics.GetWorld().CreateBody(&def);
	m_pBody->SetUserData(GetGameObject());

	m_IsReady = true;
}

void RigidBody2D::FixedUpdate()
{
	GetTransform()->SetPosition(m_pBody->GetPosition().x * ProtoPhysics.GetPPM(), m_pBody->GetPosition().y * ProtoPhysics.GetPPM());
}

void RigidBody2D::Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent)
{
	using namespace rapidxml;
	xml_node<>* pComp = doc.allocate_node(node_element, "RigidBody2D");

	SaveID(doc, pComp);
	
	auto bodyTypeInt{ static_cast<int>(m_BodyType) };
	ProtoSaver::XML::Save<int>("BodyType", bodyTypeInt, doc, pComp);

	pParent->append_node(pComp);
}