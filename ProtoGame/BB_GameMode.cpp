#include "ProtoEnginePCH.h"
#include "BB_GameMode.h"

void BB_GameMode::DrawInspectorTitle()
{
	ImGui::Text("BB - GameMode");
}

void BB_GameMode::DrawInspector()
{
	const ProtoGui::ProtoGuiData pgData{ true, 100, -1, true, 70 };
	const ProtoGui::DragData dragData{ 1.f, -1, 0, "%.0f" };
	ProtoGui::Drag<int>("Player One GOID", pgData, "##BB_GameMode_P1", m_PlayerOneID, dragData);
	ProtoGui::Drag<int>("Player Two GOID", pgData, "##BB_GameMode_P2", m_PlayerTwoID, dragData);
}

void BB_GameMode::Start()
{
}

void BB_GameMode::Awake()
{
	if (m_PlayerOneID != -1)
	{
		GameObject* pPlayerOne{ Gameobject.GetScene()->FindGameObjectWithID(unsigned(m_PlayerOneID)) };
		m_pPlayerOneController = pPlayerOne->GetComponent<BB_PlayerController>();
	}

	if (m_PlayerTwoID != -1)
	{
		GameObject* pPlayerTwo{ Gameobject.GetScene()->FindGameObjectWithID(unsigned(m_PlayerTwoID)) };
		m_pPlayerTwoController = pPlayerTwo->GetComponent<BB_PlayerController>();
	}
}

void BB_GameMode::Update()
{
}

void BB_GameMode::FixedUpdate()
{
}

void BB_GameMode::Draw()
{
}

void BB_GameMode::Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent)
{
	using namespace rapidxml;

	xml_node<>* pComp = doc.allocate_node(node_element, doc.allocate_string(ProtoConvert::ToString<BB_GameMode>().c_str()));
	SaveID(doc, pComp);
	SaveActive(doc, pComp);

	ProtoSaver::XML::Save<int>("PlayerOneID", m_PlayerOneID, doc, pComp);
	ProtoSaver::XML::Save<int>("PlayerTwoID", m_PlayerTwoID, doc, pComp);
	
	pParent->append_node(pComp);
}

void BB_GameMode::Load(rapidxml::xml_node<>* pComp, GameObject* pCurr)
{
	const auto id{ ProtoParser::XML::Parse<unsigned int>(pComp, "ID") };
	const auto isActive{ ProtoParser::XML::Parse<bool>(pComp, "Active") };

	const auto playerOneID{ ProtoParser::XML::Parse<int>(pComp, "PlayerOneID") };
	const auto playerTwoID{ ProtoParser::XML::Parse<int>(pComp, "PlayerTwoID") };

	pCurr->AddComponent(new BB_GameMode(id, isActive, playerOneID, playerTwoID));
}