#include "ProtoEnginePCH.h"
#include "CameraComponent.h"

CameraComponent::CameraComponent(const glm::vec2& position, bool active)
	: m_Position(position), m_IsActive(active)
{
}

CameraComponent::~CameraComponent()
{
	if (GetGameObject()->GetScene()->m_pActiveCamera == this)
		GetGameObject()->GetScene()->m_pActiveCamera = nullptr;
}

void CameraComponent::DrawInspectorTitle()
{
	ImGui::Text("Camera");
}

void CameraComponent::DrawInspector()
{
	ImGui::Text("Active: ");
	ImGui::SameLine();
	
	if (m_IsActive)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 0, 1, 0, 1 });
		ImGui::Text("Active");
		ImGui::PopStyleColor();
	}

	else
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1, 0, 0, 1 });
		ImGui::Text("Not Active");
		ImGui::PopStyleColor();
	}
	
	ImGui::Spacing();
	ImGui::SameLine(75);
	if (ImGui::Button("Set Active", { 250, 25 }))
		GetGameObject()->GetScene()->SetActiveCamera(this);

	ProtoGui::Presets::Position(m_Position.x, m_Position.y);
}

void CameraComponent::Move(const glm::vec2& destination, glm::vec2 speed)
{
	Reset();
	
	m_Destination = destination;
	m_Speed = speed;
	m_IsMovingBySpeed = true;
}

void CameraComponent::Move(const glm::vec2& destination, float time)
{
	Reset();
	
	m_Destination = destination;
	m_Time = time;
	m_IsMovingByTime = true;
}

void CameraComponent::Move(const glm::vec2& destination)
{
	m_Position = destination;
}

void CameraComponent::Reset()
{
	// Does not reset other variables as they will get overwritten and are only used with specific booleans
	m_CurrTime = 0;
	m_LerpAlphaX = m_LerpAlphaY = 0;
	m_IsMovingBySpeed = false;
	m_IsMovingBySpeed = false;
}

void CameraComponent::Start()
{
	if (m_IsActive)
		GetGameObject()->GetScene()->SetActiveCamera(this);
}

void CameraComponent::Update()
{
	if(m_IsMovingByTime)
	{
		m_CurrTime += ProtoTime.DeltaTime;
		m_Position = Lerp<glm::vec2>(m_Position, m_Destination, m_CurrTime / m_Time);

		if(m_CurrTime > m_Time)
			Reset();
	}

	else if(m_IsMovingBySpeed)
	{
		if (m_LerpAlphaX < 1)
			m_LerpAlphaX += m_Speed.x * ProtoTime.DeltaTime;

		if (m_LerpAlphaY < 1)
			m_LerpAlphaY += m_Speed.y * ProtoTime.DeltaTime;
		
		m_Position.x = Lerp<float>(m_Position.x, m_Destination.x, m_LerpAlphaX);
		m_Position.y = Lerp<float>(m_Position.y, m_Destination.y, m_LerpAlphaY);

		if (equal(m_Position, m_Destination).x)
			Reset();
	}
}

void CameraComponent::Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent)
{
	using namespace rapidxml;
	xml_node<>* pComp = doc.allocate_node(node_element, "CameraComponent");

	ProtoSaver::XML::Save<float>("PositionX", m_Position.x, doc, pComp);
	ProtoSaver::XML::Save<float>("PositionY", m_Position.y, doc, pComp);
	ProtoSaver::XML::Save<bool>("Active", m_IsActive, doc, pComp);

	pParent->append_node(pComp);
}
