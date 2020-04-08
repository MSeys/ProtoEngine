#pragma once
#include "BaseScene.h"
#include "GameObject.h"
#include "Singleton.h"

#include "Components.h"

constexpr double HIERARCHY_VERSION = 0.5;
constexpr double INSPECTOR_VERSION = 1.0;

namespace Proto
{
	class Editor : public Singleton<Editor>
	{
	public:
		void Init()
		{
			ImGuiIO& io = ImGui::GetIO();
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		}

		void DrawDocks() const
		{
			const ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse;
			const ImGuiDockNodeFlags dockSpaceFlags = ImGuiDockNodeFlags_None;


			
			ImGui::SetNextWindowPos({ 0, 20 }, ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize({ 350, ProtoSettings.GetWindowSize().y - 20 }, ImGuiCond_FirstUseEver);
			
			ImGui::Begin("##DOCKLEFT", nullptr, windowFlags);
			
			const ImGuiID leftDockSpace = ImGui::GetID("LeftDockSpace");
			ImGui::DockSpace(leftDockSpace, {}, dockSpaceFlags);

			ImGui::End();


			
			ImGui::SetNextWindowPos({ ProtoSettings.GetWindowSize().x - 400, 20 }, ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize({ 400, ProtoSettings.GetWindowSize().y - 20 }, ImGuiCond_FirstUseEver);

			ImGui::Begin("##DOCKRIGHT", nullptr, windowFlags);
			
			const ImGuiID rightDockSpace = ImGui::GetID("RightDockSpace");
			ImGui::DockSpace(rightDockSpace, {}, dockSpaceFlags);
			
			ImGui::End();

			
			
			ImGui::SetNextWindowPos({ 350, ProtoSettings.GetWindowSize().y - 350 }, ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSize({ ProtoSettings.GetWindowSize().x - 750, 350 }, ImGuiCond_FirstUseEver);

			ImGui::Begin("##DOCKBOTTOM", nullptr, windowFlags);

			const ImGuiID bottomDockSpace = ImGui::GetID("BottomDockSpace");
			ImGui::DockSpace(bottomDockSpace, {}, dockSpaceFlags);

			ImGui::End();

		}
		
		void Draw() const
		{
			DrawMenu();
			ProtoLogger.Draw();
			DrawHierarchy();
			DrawInspector();
		}

		void SetCurrentSelected(GameObject* pGameObject) { m_pCurrentSelected = pGameObject; }
		GameObject* GetCurrentSelected() const { return m_pCurrentSelected; }
		
	private:
		void DrawAddComponent() const;
		
		void DrawMenu() const
		{
			if (ImGui::BeginMainMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("New"))
					{
						
					}

					if (ImGui::MenuItem("Open..."))
					{

					}
					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Add"))
				{
					if (ImGui::MenuItem("GameObject"))
					{
						ProtoScenes.GetActiveScene()->AddChild(new GameObject());
					}
					ImGui::EndMenu();
				}
				
				ImGui::EndMainMenuBar();
			}
		}
		
		void DrawHierarchy() const
		{
			const std::string title{ "ProtoHierarchy V" + std::to_string(HIERARCHY_VERSION).substr(0, 3) };

			ImGui::Begin(title.c_str());

			ProtoScenes.DrawHierarchy();
			
			ImGui::End();
		}

		GameObject* m_pCurrentSelected{};

		void DrawInspector() const
		{
			if (!m_pCurrentSelected)
				return;

			const std::string title{ "ProtoInspector V" + std::to_string(INSPECTOR_VERSION).substr(0, 3) };

			ImGui::Begin(title.c_str());

			m_pCurrentSelected->DrawInspector();

			ImGui::Separator();

			DrawAddComponent();

			ImGui::End();
		}
	};

	inline void Editor::DrawAddComponent() const
	{
		ImGui::Spacing();
		ImGui::SameLine(50);
		if (ImGui::Button("Add Component", { 300, 25 }))
			ImGui::OpenPopup("AddComponentPopup");

		if (ImGui::BeginPopup("AddComponentPopup"))
		{
			ImGui::Text("Component");
			ImGui::Separator();

			if (ImGui::Selectable("Render Component"))
				m_pCurrentSelected->AddComponent(new RenderComponent(nullptr, {}));

			if (ImGui::Selectable("Text Component"))
				m_pCurrentSelected->AddComponent(new TextComponent("", nullptr, {}));

			if (ImGui::Selectable("FPS Component"))
				m_pCurrentSelected->AddComponent(new FPSComponent(nullptr, {}));
			
			ImGui::EndPopup();
		}
	}
}
