#include "ProtoEnginePCH.h"

#include "BB_MenuController.h"

void BB_MenuController::DrawInspectorTitle()
{
	ImGui::Text("BB - Menu Controller");
}

void BB_MenuController::DrawInspector()
{
	ImGui::Text("Internal Usage");
}

void BB_MenuController::Start()
{
	ProtoCommands.AddCommand(new MenuControlDownCommand(), "BubbleBobble_MenuDown");
	ProtoCommands.AddCommand(new MenuControlUpCommand(), "BubbleBobble_MenuUp");
	ProtoCommands.AddCommand(new MenuControlSubmitCommand(), "BubbleBobble_MenuSubmit");

	ProtoCommands.GetCommand("BubbleBobble_MenuDown").SetExecuteData(&m_MenuState);
	ProtoCommands.GetCommand("BubbleBobble_MenuUp").SetExecuteData(&m_MenuState);
	ProtoCommands.GetCommand("BubbleBobble_MenuSubmit").SetExecuteData(&m_MenuState);
	
	ProtoInput.AddInput(SDLK_UP);
	ProtoInput.GetInput(SDLK_UP).SetCommand(ButtonState::Pressed, "BubbleBobble_MenuUp");

	ProtoInput.AddInput(SDLK_DOWN);
	ProtoInput.GetInput(SDLK_DOWN).SetCommand(ButtonState::Pressed, "BubbleBobble_MenuDown");

	ProtoInput.AddInput(SDLK_w);
	ProtoInput.GetInput(SDLK_w).SetCommand(ButtonState::Pressed, "BubbleBobble_MenuUp");

	ProtoInput.AddInput(SDLK_s);
	ProtoInput.GetInput(SDLK_s).SetCommand(ButtonState::Pressed, "BubbleBobble_MenuDown");
	
	ProtoInput.AddInput(SDLK_RETURN);
	ProtoInput.GetInput(SDLK_RETURN).SetCommand(ButtonState::Pressed, "BubbleBobble_MenuSubmit");

	for(int i{}; i < 2; i++)
	{
		ProtoInput.AddInput(i, XINPUT_GAMEPAD_DPAD_UP);
		ProtoInput.GetInput(i, XINPUT_GAMEPAD_DPAD_UP).SetCommand(ButtonState::Pressed, "BubbleBobble_MenuUp");

		ProtoInput.AddInput(i, XINPUT_GAMEPAD_DPAD_DOWN);
		ProtoInput.GetInput(i, XINPUT_GAMEPAD_DPAD_DOWN).SetCommand(ButtonState::Pressed, "BubbleBobble_MenuDown");

		ProtoInput.AddInput(i, XINPUT_GAMEPAD_A);
		ProtoInput.GetInput(i, XINPUT_GAMEPAD_A).SetCommand(ButtonState::Pressed, "BubbleBobble_MenuSubmit");
	}
}

void BB_MenuController::Awake()
{
}

void BB_MenuController::Update()
{
	switch(m_MenuState)
	{
	case BB_MenuState::Solo:
		GetTransform()->SetPosition(435.f, 360.f);
		break;
	case BB_MenuState::Coop:
		GetTransform()->SetPosition(435.f, 410.f);
		break;
	case BB_MenuState::Versus:
		GetTransform()->SetPosition(435.f, 460.f);
		break;
	default: ;
	}
}

void BB_MenuController::FixedUpdate()
{
}

void BB_MenuController::Draw()
{
}

void BB_MenuController::Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent)
{
	using namespace rapidxml;

	xml_node<>* pComp = doc.allocate_node(node_element, doc.allocate_string(ProtoConvert::ToString<BB_MenuController>().c_str()));
	SaveID(doc, pComp);
	SaveActive(doc, pComp);
	
	pParent->append_node(pComp);
}

void BB_MenuController::Load(rapidxml::xml_node<>* pComp, GameObject* pCurr)
{
	const auto id{ ProtoParser::XML::Parse<unsigned int>(pComp, "ID") };
	const auto isActive{ ProtoParser::XML::Parse<bool>(pComp, "Active") };

	pCurr->AddComponent(new BB_MenuController(id, isActive));
}