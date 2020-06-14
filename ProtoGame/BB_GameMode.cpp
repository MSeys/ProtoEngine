#include "ProtoEnginePCH.h"
#include "BB_GameMode.h"

#include "BB_PlayerController.h"

void BB_GameMode::DrawInspectorTitle()
{
	ImGui::Text("BB - GameMode");
}

void BB_GameMode::DrawInspector()
{
	const ProtoGui::ProtoGuiData pgData{ true, 120, -1, true, 70 };
	const ProtoGui::DragData dragData{ 1.f, -1, 0, "%.0f" };
	ProtoGui::Drag<int>("Player One GOID", pgData, "##BB_GameMode_P1", m_PlayerOneID, dragData);
	ProtoGui::Drag<int>("Player Two GOID", pgData, "##BB_GameMode_P2", m_PlayerTwoID, dragData);
}

void BB_GameMode::SetBackgroundFade(float fade) const
{
	if(m_pFadeBackground)
		m_pFadeBackground->SetColor({ 0, 0, 0, Uint8(fade * 255) });
}

void BB_GameMode::Start()
{
}

void BB_GameMode::Awake()
{
	m_pLevels[0] = Gameobject.GetScene()->FindGameObjectInRootWithName("Level1");
	m_pLevels[1] = Gameobject.GetScene()->FindGameObjectInRootWithName("Level2");
	m_pLevels[2] = Gameobject.GetScene()->FindGameObjectInRootWithName("Level3");
	
	m_pFadeBackground = Gameobject.GetComponent<Image>(true);
	if (m_PlayerOneID != -1)
	{
		GameObject* pPlayerOne{ Gameobject.GetScene()->FindGameObjectWithID(unsigned(m_PlayerOneID)) };
		m_pPlayerOneController = pPlayerOne->GetComponent<BB_PlayerController>();
		m_pPlayerOneUI = Gameobject.FindGameObjectWithName("P1_UI");
		m_pPlayerOneHealth = dynamic_cast<Sprite*>(m_pPlayerOneUI->FindComponentWithID(6));
	}

	if (m_PlayerTwoID != -1)
	{
		GameObject* pPlayerTwo{ Gameobject.GetScene()->FindGameObjectWithID(unsigned(m_PlayerTwoID)) };
		m_pPlayerTwoController = pPlayerTwo->GetComponent<BB_PlayerController>();
		m_pPlayerTwoUI = Gameobject.FindGameObjectWithName("P2_UI");
		m_pPlayerTwoUI->SetActive(true);

		m_pPlayerTwoHealth = dynamic_cast<Sprite*>(m_pPlayerTwoUI->FindComponentWithID(6));
	}
}

void BB_GameMode::Update()
{
	m_pPlayerOneHealth->ResizeFrameList(m_pPlayerOneController->GetHealth());
	if (m_pPlayerTwoHealth)
		m_pPlayerTwoHealth->ResizeFrameList(m_pPlayerTwoController->GetHealth());
}

void BB_GameMode::FixedUpdate()
{
	if (!m_IsTransitioning)
	{
		if (m_pLevels[m_CurrentLevel]->GetChildren().size() == 2)
			m_IsTransitioning = true;
	}

	if (m_IsTransitioning)
	{
		SetBackgroundFade((m_TransitionCurrTimer <= m_TransitionTimer / 2.f) ? m_TransitionCurrTimer : m_TransitionTimer - m_TransitionCurrTimer);
		m_TransitionCurrTimer += ProtoTime.DeltaTime;
		if(m_TransitionCurrTimer >= m_TransitionTimer / 2.f && !m_IsTransitioningHalfway)
		{
			if (m_CurrentLevel + 1 == int(m_pLevels.size()))
			{
				ProtoScenes.Load(L"BubbleBobble_Menu");
				return;
			}
			
			m_pLevels[m_CurrentLevel]->SetActive(false);
			m_CurrentLevel += 1;
			m_pLevels[m_CurrentLevel]->SetActive(true);

			m_pPlayerOneController->ResetPosition();

			if (HasPlayerTwo())
				m_pPlayerTwoController->ResetPosition();
			
			m_IsTransitioningHalfway = true;
		}
		if (m_TransitionCurrTimer > m_TransitionTimer)
		{
			m_TransitionCurrTimer = 0.f;
			m_IsTransitioning = false;
			m_IsTransitioningHalfway = false;
		}
	}
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