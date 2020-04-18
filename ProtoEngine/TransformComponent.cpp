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
	std::stringstream thisAddress;
	thisAddress << this;

	std::string labelText;

	/* Position */ {
		ImGui::Text("Position");

		ImGui::SameLine(100);
		ImGui::Text("X");
		ImGui::SameLine(115);

		ImGui::PushItemWidth(70);
		labelText = "##TRANSFORM_POS_X" + thisAddress.str();
		ImGui::DragFloat(labelText.c_str(), &m_Position.x, 0.1f, 0, 0, "%.1f");
		ImGui::PopItemWidth();

		ImGui::SameLine(200);
		ImGui::Text("Y");
		ImGui::SameLine(215);

		ImGui::PushItemWidth(70);
		labelText = "##TRANSFORM_POS_Y" + thisAddress.str();
		ImGui::DragFloat(labelText.c_str(), &m_Position.y, 0.1f, 0, 0, "%.1f");
		ImGui::PopItemWidth();
	}

	/* Rotation */ {
		ImGui::Text("Rot. Center");

		ImGui::SameLine(100);
		ImGui::Text("X");
		ImGui::SameLine(115);

		ImGui::PushItemWidth(70);
		labelText = "##TRANSFORM_ROT_POS_X" + thisAddress.str();
		ImGui::DragFloat(labelText.c_str(), &m_RotCenter.x, 0.1f, 0, 0, "%.1f");
		ImGui::PopItemWidth();

		ImGui::SameLine(200);
		ImGui::Text("Y");
		ImGui::SameLine(215);

		ImGui::PushItemWidth(70);
		labelText = "##TRANSFORM_ROT_POS_Y" + thisAddress.str();
		ImGui::DragFloat(labelText.c_str(), &m_RotCenter.y, 0.1f, 0, 0, "%.1f");
		ImGui::PopItemWidth();

		ImGui::Text("Rot. Angle");
		
		ImGui::SameLine(115);
		
		ImGui::PushItemWidth(70);
		labelText = "##TRANSFORM_ROT_ANGLE" + thisAddress.str();
		ImGui::DragFloat(labelText.c_str(), &m_RotAngle, 0.1f, 0, 0, "%.1f");
		ImGui::PopItemWidth();
	}

	/* Scale */ {
		ImGui::Text("Scale");

		ImGui::SameLine(100);
		ImGui::Text("X");
		ImGui::SameLine(115);

		ImGui::PushItemWidth(70);
		labelText = "##TRANSFORM_SCALE_X" + thisAddress.str();
		ImGui::DragFloat(labelText.c_str(), &m_Scale.x, 0.01f, 0, 0, "%.2f");
		ImGui::PopItemWidth();

		ImGui::SameLine(200);
		ImGui::Text("Y");
		ImGui::SameLine(215);

		ImGui::PushItemWidth(70);
		labelText = "##TRANSFORM_SCALE_Y" + thisAddress.str();
		ImGui::DragFloat(labelText.c_str(), &m_Scale.y, 0.01f, 0, 0, "%.2f");
		ImGui::PopItemWidth();
	}
}

void TransformComponent::Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent)
{
	using namespace rapidxml;
	
	xml_node<>* component = doc.allocate_node(node_element, "TransformComponent");
	component->append_attribute(doc.allocate_attribute("PositionX", doc.allocate_string(ToCString(m_Position.x))));
	component->append_attribute(doc.allocate_attribute("PositionY", doc.allocate_string(ToCString(m_Position.y))));
	
	component->append_attribute(doc.allocate_attribute("RotCenterX", doc.allocate_string(ToCString(m_RotCenter.x))));
	component->append_attribute(doc.allocate_attribute("RotCenterY", doc.allocate_string(ToCString(m_RotCenter.y))));
	component->append_attribute(doc.allocate_attribute("RotAngle", doc.allocate_string(ToCString(m_RotAngle))));

	component->append_attribute(doc.allocate_attribute("ScaleX", doc.allocate_string(ToCString(m_Scale.x))));
	component->append_attribute(doc.allocate_attribute("ScaleY", doc.allocate_string(ToCString(m_Scale.y))));
	
	pParent->append_node(component);
}