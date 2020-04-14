#include "ProtoEnginePCH.h"
#include "TransformComponent.h"

void TransformComponent::SetPosition(const float x, const float y)
{
	m_Position = { x, y };
}

void TransformComponent::SetScale(float x, float y)
{
	m_Scale = { x, y };
}

void TransformComponent::Initialize()
{
	m_Position = { 0, 0 };
	m_Scale = { 1.f, 1.f };
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

	std::string labelText;

	/* Position */ {
		ImGui::Text("Position");

		ImGui::SameLine(100);
		ImGui::Text("X");
		ImGui::SameLine(115);

		ImGui::PushItemWidth(70);
		labelText = "##TRANSFORM_POS_X" + thisAddress.str();
		ImGui::DragFloat(labelText.c_str(), &m_Position.x, 1, 0, 0, "%.1f");
		ImGui::PopItemWidth();

		ImGui::SameLine(200);
		ImGui::Text("Y");
		ImGui::SameLine(215);

		ImGui::PushItemWidth(70);
		labelText = "##TRANSFORM_POS_Y" + thisAddress.str();
		ImGui::DragFloat(labelText.c_str(), &m_Position.y, 1, 0, 0, "%.1f");
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