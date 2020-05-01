#include "ProtoEnginePCH.h"
#include "TransformComponent.h"

glm::vec2 TransformComponent::GetPosition() const
{
	if (GetGameObject()->GetParent())
	{
		return GetGameObject()->GetParent()->GetTransform()->GetPosition() + m_Position;
	}

	return m_Position;
}

glm::vec2 TransformComponent::GetRotCenter() const
{
	if (GetGameObject()->GetParent())
	{
		return GetGameObject()->GetParent()->GetTransform()->GetRotCenter() + m_RotCenter;
	}

	return m_RotCenter;
}

float TransformComponent::GetRotAngle() const
{
	if (GetGameObject()->GetParent())
	{
		return GetGameObject()->GetParent()->GetTransform()->GetRotAngle() + m_RotAngle;
	}

	return m_RotAngle;
}

glm::vec2 TransformComponent::GetScale() const
{
	if (GetGameObject()->GetParent())
	{
		return GetGameObject()->GetParent()->GetTransform()->GetScale() * m_Scale;
	}

	return m_Scale;
}

void TransformComponent::SetPosition(const float x, const float y)
{
	m_Position = { x, y };
}

void TransformComponent::SetRotCenter(float x, float y)
{
	m_RotCenter = { x, y };
}

void TransformComponent::SetRotAngle(float angle)
{
	m_RotAngle = angle;
}

void TransformComponent::SetScale(float x, float y)
{
	m_Scale = { x, y };
}

void TransformComponent::DrawInspectorTitle()
{
	ImGui::Text("Transform");
}

void TransformComponent::DrawInspector()
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
		ImGui::Text("Rot. Center");
		ImGui::SameLine(100);

		pgData.sameLineOffset = 115;
		ProtoGui::Drag<float>("X", pgData, "##TRANSFORM_ROT_POS_X", m_RotCenter.x, dragData);
		ImGui::SameLine(200);

		pgData.sameLineOffset = 215;
		ProtoGui::Drag<float>("Y", pgData, "##TRANSFORM_ROT_POS_Y", m_RotCenter.y, dragData);

		ImGui::Text("Rot. Angle");
		
		ImGui::SameLine(115);

		pgData.sameLineOffset = 115;
		ProtoGui::Drag<float>("", pgData, "##TRANSFORM_ROT_ANGLE", m_RotAngle, dragData);
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

void TransformComponent::Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent)
{
	using namespace rapidxml;
	xml_node<>* pComp = doc.allocate_node(node_element, "TransformComponent");
	
	ProtoSaver::XML::Save<float>("PositionX", m_Position.x, doc, pComp);
	ProtoSaver::XML::Save<float>("PositionY", m_Position.y, doc, pComp);
	
	ProtoSaver::XML::Save<float>("RotCenterX", m_RotCenter.x, doc, pComp);
	ProtoSaver::XML::Save<float>("RotCenterY", m_RotCenter.y, doc, pComp);
	ProtoSaver::XML::Save<float>("RotAngle", m_RotAngle, doc, pComp);
	
	ProtoSaver::XML::Save<float>("ScaleX", m_Scale.x, doc, pComp);
	ProtoSaver::XML::Save<float>("ScaleY", m_Scale.y, doc, pComp);
	
	pParent->append_node(pComp);
}