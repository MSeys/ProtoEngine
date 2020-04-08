#include "ProtoEnginePCH.h"
#include "TransformComponent.h"

void TransformComponent::SetPosition(const float x, const float y, const float z)
{
	m_Position.x = x;
	m_Position.y = y;
	m_Position.z = z;
}

void TransformComponent::Initialize()
{
	m_Position = { 0, 0, 0 };
}

void TransformComponent::Update()
{
}

void TransformComponent::FixedUpdate()
{
}

void TransformComponent::Draw()
{
}

void TransformComponent::DrawInspectorTitle()
{
	ImGui::Text("Transform");
}

void TransformComponent::DrawInspector()
{
	std::stringstream thisAddress;
	thisAddress << this;

	std::string labelText{};
	
	ImGui::Text("Position");
	
	ImGui::SameLine(100);
	ImGui::Text("X");
	ImGui::SameLine(115);
	
	ImGui::PushItemWidth(70);
	labelText = "##TRANSFORM_POS_X" + thisAddress.str();
	ImGui::InputFloat(labelText.c_str(), &m_Position.x, 0, 0, "%.1f");
	ImGui::PopItemWidth();

	ImGui::SameLine(200);
	ImGui::Text("Y");
	ImGui::SameLine(215);
	
	ImGui::PushItemWidth(70);
	labelText = "##TRANSFORM_POS_Y" + thisAddress.str();
	ImGui::InputFloat(labelText.c_str(), &m_Position.y, 0, 0, "%.1f");
	ImGui::PopItemWidth();

	ImGui::SameLine(300);
	ImGui::Text("Z");
	ImGui::SameLine(315);
	
	ImGui::PushItemWidth(70);
	labelText = "##TRANSFORM_POS_Z" + thisAddress.str();
	ImGui::InputFloat(labelText.c_str(), &m_Position.z, 0, 0, "%.1f");
	ImGui::PopItemWidth();
}
