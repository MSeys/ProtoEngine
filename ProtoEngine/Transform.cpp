#include "ProtoEnginePCH.h"
#include "Transform.h"

glm::vec2 Transform::GetPosition() const
{
	if (GetGameObject()->GetParent())
	{
		return GetGameObject()->GetParent()->GetTransform()->GetPosition() + m_Position;
	}

	return m_Position;
}

float Transform::GetRotation() const
{
	if (GetGameObject()->GetParent())
	{
		return GetGameObject()->GetParent()->GetTransform()->GetRotation() + m_Rotation;
	}

	return m_Rotation;
}

glm::vec2 Transform::GetScale() const
{
	if (GetGameObject()->GetParent())
	{
		return GetGameObject()->GetParent()->GetTransform()->GetScale() * m_Scale;
	}

	return m_Scale;
}

void Transform::SetPosition(const float x, const float y)
{
	m_Position = { x, y };
}

void Transform::SetRotation(float angle)
{
	m_Rotation = angle;
}

void Transform::SetScale(float x, float y)
{
	m_Scale = { x, y };
}

void Transform::DrawInspectorTitle()
{
	ImGui::Text("Transform");
}

void Transform::DrawInspector()
{
	ProtoGui::ProtoGuiData pgData{ true, 0, -1, true, 70 };
	ProtoGui::DragData dragData{ 0.1f, 0, 0, "%.1f" };
	
	/* Position */ {
		ImGui::Text("Position");
		ImGui::SameLine(100);

		pgData.sameLineOffset = 115;
		ProtoGui::Drag<float>("X", pgData, "##TRANSFORM_POS_X", m_Position.x, dragData);
		ImGui::SameLine(200);

		pgData.sameLineOffset = 215;
		ProtoGui::Drag<float>("Y", pgData, "##TRANSFORM_POS_Y", m_Position.y, dragData);
	}

	/* Rotation */ {
		ImGui::Text("Rotation");
		
		ImGui::SameLine(115);

		pgData.sameLineOffset = 115;
		ProtoGui::Drag<float>("", pgData, "##TRANSFORM_ROT_ANGLE", m_Rotation, dragData);
	}

	/* Scale */ {
		ImGui::Text("Scale");
		ImGui::SameLine(100);

		dragData.v_speed = 0.01f;
		dragData.format = "%.2f";
		
		pgData.sameLineOffset = 115;
		ProtoGui::Drag<float>("X", pgData, "##TRANSFORM_SCALE_X", m_Scale.x, dragData);
		ImGui::SameLine(200);

		pgData.sameLineOffset = 215;
		ProtoGui::Drag<float>("Y", pgData, "##TRANSFORM_SCALE_Y", m_Scale.y, dragData);
	}
}

void Transform::Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent)
{
	using namespace rapidxml;
	
	xml_node<>* pComp = doc.allocate_node(node_element, doc.allocate_string(ProtoConvert::ToString<Transform>().c_str()));

	SaveID(doc, pComp);
	
	ProtoSaver::XML::Save<float>("PositionX", m_Position.x, doc, pComp);
	ProtoSaver::XML::Save<float>("PositionY", m_Position.y, doc, pComp);
	
	ProtoSaver::XML::Save<float>("Rotation", m_Rotation, doc, pComp);
	
	ProtoSaver::XML::Save<float>("ScaleX", m_Scale.x, doc, pComp);
	ProtoSaver::XML::Save<float>("ScaleY", m_Scale.y, doc, pComp);
	
	pParent->append_node(pComp);
}

void Transform::Load(rapidxml::xml_node<>* pComp, GameObject* pCurr)
{
	using namespace rapidxml;
	
	auto* pTransformComp{ pCurr->GetTransform() };

	glm::vec2 position;
	position.x = ProtoParser::XML::Parse<float>(pComp, "PositionX");
	position.y = ProtoParser::XML::Parse<float>(pComp, "PositionY");

	const float rotation{ProtoParser::XML::Parse<float>(pComp, "Rotation") };

	glm::vec2 scale;
	scale.x = ProtoParser::XML::Parse<float>(pComp, "ScaleX");
	scale.y = ProtoParser::XML::Parse<float>(pComp, "ScaleY");

	pTransformComp->SetPosition(position.x, position.y);
	pTransformComp->SetRotation(rotation);
	pTransformComp->SetScale(scale.x, scale.y);
}
