#include "ProtoEnginePCH.h"
#include "RigidBody2D.h"

RigidBody2D::RigidBody2D(ComponentID ID, RigidBodyType bodyType, float gravityScale, bool fixedRotation, bool isBullet)
	: BaseBehaviour(ID, true), m_BodyType(bodyType), m_IsBullet(isBullet), m_FixedRotation(fixedRotation), m_GravityScale(gravityScale)
{
}

RigidBody2D::~RigidBody2D()
{
	if (ProtoPhysics.GetWorld().GetBodyCount() > 0 && m_pBody)
		ProtoPhysics.GetWorld().DestroyBody(m_pBody);
}

void RigidBody2D::DrawInspectorTitle()
{
	ImGui::Text("Rigid Body 2D");
}

void RigidBody2D::DrawInspector()
{
	auto bodyType{ static_cast<int>(m_BodyType) };

	ImGui::Text("Body Type: ");
	ImGui::SameLine(120);
	
	ImGui::PushItemWidth(100);
	ImGui::Combo("", &bodyType, "Static\0Dynamic\0Kinematic\0");
	ImGui::PopItemWidth();
	m_BodyType = static_cast<RigidBodyType>(bodyType);

	const ProtoGui::ProtoGuiData pgData{ true, 120, 0, true, 70 };
	const ProtoGui::DragData dragData{ 0.01f, 0, 0, "%0.2f", 1 };
	ProtoGui::Drag<float>("Grav. Scale:", pgData, "##GravityScale", m_GravityScale, dragData);
	
	ImGui::Text("Fixed Rotation: ");
	ImGui::SameLine(120);
	ImGui::Checkbox("##FixedRotation", &m_FixedRotation);

	ImGui::Text("Is Bullet: ");
	ImGui::SameLine(120);
	ImGui::Checkbox("##Bullet", &m_IsBullet);
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
	def.angle = ProtoConvert::ToRadians(GetTransform()->GetRotation());
	def.fixedRotation = m_FixedRotation;
	def.gravityScale = m_GravityScale;
	def.bullet = m_IsBullet;

	m_pBody = ProtoPhysics.GetWorld().CreateBody(&def);
	m_pBody->SetUserData(GetGameObject());

	m_IsReady = true;
}

void RigidBody2D::FixedUpdate()
{
	const glm::vec2 position{ ProtoConvert::Box2DToPixels(ProtoConvert::ToGLMVec(m_pBody->GetPosition())) };
	GetTransform()->SetPosition(position.x, position.y);
	GetTransform()->SetRotation(ProtoConvert::ToDegrees(m_pBody->GetAngle()));
}

void RigidBody2D::Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent)
{
	using namespace rapidxml;

	xml_node<>* pComp = doc.allocate_node(node_element, doc.allocate_string(ProtoConvert::ToString<RigidBody2D>().c_str()));
	
	SaveID(doc, pComp);
	
	auto bodyTypeInt{ static_cast<int>(m_BodyType) };
	ProtoSaver::XML::Save<int>("BodyType", bodyTypeInt, doc, pComp);
	ProtoSaver::XML::Save<float>("GravityScale", m_GravityScale, doc, pComp);
	ProtoSaver::XML::Save<bool>("FixedRotation", m_FixedRotation, doc, pComp);
	ProtoSaver::XML::Save<bool>("IsBullet", m_IsBullet, doc, pComp);
	
	pParent->append_node(pComp);
}

void RigidBody2D::Load(rapidxml::xml_node<>* pComp, GameObject* pCurr)
{
	if (!pComp)
		return;

	const auto id{ ProtoParser::XML::Parse<unsigned int>(pComp, "ID") };
	const auto bodyType{ ProtoParser::XML::Parse<int>(pComp, "BodyType") };
	const auto gravityScale{ ProtoParser::XML::Parse<float>(pComp, "GravityScale") };
	const auto fixedRotation{ ProtoParser::XML::Parse<bool>(pComp, "FixedRotation") };
	const auto isBullet{ ProtoParser::XML::Parse<bool>(pComp, "IsBullet") };
	
	pCurr->AddComponent(new RigidBody2D(id, static_cast<RigidBodyType>(bodyType), gravityScale, fixedRotation, isBullet));
}