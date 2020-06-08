#include "ProtoEnginePCH.h"
#include "Camera.h"

Camera::Camera(ComponentID ID, const glm::vec2& position, bool active)
	: BaseBehaviour(ID, true), m_Position(position), m_IsCamActive(active)
{
}

Camera::~Camera()
{
	if (GetGameObject()->GetScene()->m_pActiveCamera == this)
		GetGameObject()->GetScene()->m_pActiveCamera = nullptr;
}

void Camera::DrawInspectorTitle()
{
	ImGui::Text("Camera");
}

void Camera::DrawInspector()
{
	ImGui::Text("Active: ");
	ImGui::SameLine();
	
	if (m_IsCamActive)
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
	if (m_IsCamActive)
	{
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		ImGui::Button("Set Active", { 250, 25 });
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	else
	{
		if (ImGui::Button("Set Active", { 250, 25 }))
			Activate();
	}

	ProtoGui::Presets::Position(m_Position.x, m_Position.y);
}

void Camera::Move(const glm::vec2& destination, glm::vec2 speed)
{
	Reset();
	
	m_Destination = destination;
	m_Speed = speed;
	m_IsMovingBySpeed = true;
}

void Camera::Move(const glm::vec2& destination, float time)
{
	Reset();
	
	m_Destination = destination;
	m_Time = time;
	m_IsMovingByTime = true;
}

void Camera::Move(const glm::vec2& destination)
{
	m_Position = destination;
}

void Camera::Reset()
{
	// Does not reset other variables as they will get overwritten and are only used with specific booleans
	m_CurrTime = 0;
	m_LerpAlphaX = m_LerpAlphaY = 0;
	m_IsMovingBySpeed = false;
	m_IsMovingBySpeed = false;
}

void Camera::Activate()
{
	m_IsCamActive = true;
	ProtoScenes.GetCurrentScene()->SetActiveCamera(this);
}

void Camera::Deactivate()
{
	m_IsCamActive = false;
}

void Camera::Start()
{
	if (m_IsCamActive)
		GetGameObject()->GetScene()->SetActiveCamera(this);
}

void Camera::Update()
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

void Camera::Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent)
{
	using namespace rapidxml;

	xml_node<>* pComp = doc.allocate_node(node_element, doc.allocate_string(ProtoConvert::ToString<Camera>().c_str()));

	SaveID(doc, pComp);
	
	ProtoSaver::XML::Save<float>("PositionX", m_Position.x, doc, pComp);
	ProtoSaver::XML::Save<float>("PositionY", m_Position.y, doc, pComp);
	ProtoSaver::XML::Save<bool>("CamActive", m_IsCamActive, doc, pComp);

	pParent->append_node(pComp);
}

void Camera::DrawEditorDebug()
{
	ProtoRenderer.RenderLineRect({ int(m_Position.x), int(m_Position.y), int(ProtoSettings.GetWindowSettings().GameWindowSize.x), int(ProtoSettings.GetWindowSettings().GameWindowSize.y) }, 
								{ 255, 255, 255, 255 });
}

void Camera::Load(rapidxml::xml_node<>* pComp, GameObject* pCurr)
{
	const auto id{ ProtoParser::XML::Parse<unsigned int>(pComp, "ID") };

	glm::vec2 position;
	position.x = ProtoParser::XML::Parse<float>(pComp, "PositionX");
	position.y = ProtoParser::XML::Parse<float>(pComp, "PositionY");

	const auto active{ ProtoParser::XML::Parse<bool>(pComp, "CamActive") };

	pCurr->AddComponent(new Camera(id, position, active));
}