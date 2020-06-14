#pragma once
#include "BaseBehaviour.h"

enum class BB_MenuState
{
	Solo, Coop, Versus
};

class BB_MenuController final : public BaseBehaviour
{
public:
	BB_MenuController(const ComponentID ID, const bool isActive)
		: BaseBehaviour(ID, isActive)
	{
	}

	void DrawInspectorTitle() override;
	void DrawInspector() override;
	
protected:
	void Start() override;
	void Awake() override;
	void Update() override;
	void FixedUpdate() override;
	void Draw() override;

	void Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent) override;
	
private:
	BB_MenuState m_MenuState{ BB_MenuState::Solo };

public:
	static void Load(rapidxml::xml_node<>* pComp, GameObject* pCurr);
};

class MenuControlUpCommand : public Command
{
public:
	void Execute() override
	{
		auto& menuState = VoidCast<BB_MenuState>(EData);


		if (menuState == BB_MenuState::Solo)
			menuState = BB_MenuState::Versus;

		else
			menuState = BB_MenuState(int(menuState) - 1);
	}
};

class MenuControlDownCommand : public Command
{
public:
	void Execute() override
	{
		auto& menuState = VoidCast<BB_MenuState>(EData);


		if (menuState == BB_MenuState::Versus)
			menuState = BB_MenuState::Solo;

		else
			menuState = BB_MenuState(int(menuState) + 1);
	}
};

class MenuControlSubmitCommand : public Command
{
public:
	void Execute() override
	{
		auto& menuState = VoidCast<BB_MenuState>(EData);

		switch(menuState)
		{
		case BB_MenuState::Solo:
			ProtoLogger.AddLog(LogLevel::Info, "Going to Solo");
			ProtoScenes.Load(L"BubbleBobble_SP");
			break;
		case BB_MenuState::Coop:
			ProtoLogger.AddLog(LogLevel::Info, "Going to Co-op");
			break;
		case BB_MenuState::Versus:
			ProtoLogger.AddLog(LogLevel::Info, "Going to Versus");
			break;
		default: ;
		}
	}
};