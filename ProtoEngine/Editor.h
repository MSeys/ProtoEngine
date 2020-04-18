#pragma once
#include "Scene.h"
#include "GameObject.h"
#include "Singleton.h"

#include "Components.h"
#include "ImGui/imgui_internal.h"
#include "portable-file-dialogs.h"

constexpr double HIERARCHY_VERSION = 0.5;
constexpr double INSPECTOR_VERSION = 1.0;

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

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



			ImGui::SetNextWindowPos({ 0, 20 }, ImGuiCond_Always);
			ImGui::SetNextWindowSize({ 350, ProtoSettings.GetWindowSize().y - 20 }, ImGuiCond_Always);

			ImGui::Begin("##DOCKLEFT", nullptr, windowFlags);

			const ImGuiID leftDockSpace = ImGui::GetID("LeftDockSpace");
			ImGui::DockSpace(leftDockSpace, {}, dockSpaceFlags);

			ImGui::End();



			ImGui::SetNextWindowPos({ ProtoSettings.GetWindowSize().x - 400, 20 }, ImGuiCond_Always);
			ImGui::SetNextWindowSize({ 400, ProtoSettings.GetWindowSize().y - 20 }, ImGuiCond_Always);

			ImGui::Begin("##DOCKRIGHT", nullptr, windowFlags);

			const ImGuiID rightDockSpace = ImGui::GetID("RightDockSpace");
			ImGui::DockSpace(rightDockSpace, {}, dockSpaceFlags);

			ImGui::End();



			ImGui::SetNextWindowPos({ 350, ProtoSettings.GetWindowSize().y - 350 }, ImGuiCond_Always);
			ImGui::SetNextWindowSize({ ProtoSettings.GetWindowSize().x - 750, 350 }, ImGuiCond_Always);

			ImGui::Begin("##DOCKBOTTOM", nullptr, windowFlags);

			const ImGuiID bottomDockSpace = ImGui::GetID("BottomDockSpace");
			ImGui::DockSpace(bottomDockSpace, {}, dockSpaceFlags);

			ImGui::End();

		}

		void Draw()
		{
			SDL_Rect fullWindow{
					0,
					0,
					int(ProtoSettings.GetWindowSize().x),
					int(ProtoSettings.GetWindowSize().y) };

			SDL_Rect gameWindow{
					int(ProtoSettings.GetRenderSettings().GameRenderOffset.x),
					int(ProtoSettings.GetRenderSettings().GameRenderOffset.y),
					int(ProtoSettings.GetWindowSettings().WindowSize.x),
					int(ProtoSettings.GetWindowSettings().WindowSize.y) };

			SDL_SetRenderDrawColor(ProtoRenderer.GetSDLRenderer(), 25, 25, 25, 80);
			SDL_RenderFillRect(ProtoRenderer.GetSDLRenderer(), &fullWindow);
			SDL_SetRenderDrawColor(ProtoRenderer.GetSDLRenderer(), 0, 0, 0, 255);
			SDL_RenderFillRect(ProtoRenderer.GetSDLRenderer(), &gameWindow);

			DrawMenu();
			ProtoLogger.Draw();
			DrawHierarchy();
			DrawInspector();

			ImGui::ShowDemoWindow();

		}

		void SetCurrentSelected(GameObject* pGameObject) { m_pCurrentSelected = pGameObject; }
		GameObject* GetCurrentSelected() const { return m_pCurrentSelected; }

	private:
		void DrawAddComponent() const;

		void DrawMenu()
		{
			if (ImGui::BeginMainMenuBar())
			{
				if (ImGui::BeginMenu("File"))
				{
					if (ImGui::MenuItem("New"))
					{
						Scene* oldScene = ProtoScenes.GetActiveScene();
						ProtoScenes.RemoveGameScene(oldScene);
						ProtoScenes.AddGameScene(new Scene(L"Editor"));
						ProtoScenes.SetActiveGameScene(L"Editor");

						SafeDelete(oldScene);
					}

					if (ImGui::MenuItem("Save"))
						m_OpenSavePopup = true;

					if (ImGui::MenuItem("Open..."))
					{
						const std::string fullPath = fs::canonical(ProtoContent.GetDataPath()).string();
						const auto selection = pfd::open_file("Select a Scene", fullPath, { "ProtoScenes", "*.protoscene" }).result();
						if (!selection.empty())
						{							
							Scene* oldScene = ProtoScenes.GetActiveScene();
							ProtoScenes.RemoveGameScene(oldScene);

							auto pNewScene = new Scene(L"Editor");
							ProtoScenes.AddGameScene(pNewScene);
							ProtoScenes.SetActiveGameScene(L"Editor");
							
							pNewScene->Load(selection[0].substr(fullPath.size()), &m_SaveFilePath, &m_SaveFileName);

							SafeDelete(oldScene);
							m_pCurrentSelected = nullptr;
						}
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

			if (m_OpenSavePopup)
				ImGui::OpenPopup("SavePopup");

			if (ImGui::BeginPopupModal("SavePopup", NULL, ImGuiWindowFlags_AlwaysAutoResize))
			{
				ImGui::Text("Where would you like to save your Scene?\nNote: Stick to Data folder!");
				ImGui::Separator();

				m_SaveFileName.resize(50);
				ImGui::Text("File Name: ");
				ImGui::SameLine();
				ImGui::InputText("##SAVE_FILE_NAME", &m_SaveFileName[0], 50);

				ImGui::SameLine();
				ImGui::Text(".protoscene");

				m_SaveFilePath.resize(300);
				ImGui::Text("File Location: ");
				ImGui::SameLine();
				ImGui::InputText("##SAVE_FILE_PATH", &m_SaveFilePath[0], 300, ImGuiInputTextFlags_ReadOnly);
				ImGui::SameLine();

				if (ImGui::Button("...##SAVE_FILE_BUTTON"))
				{
					const std::string fullPath = fs::canonical(ProtoContent.GetDataPath()).string();
					const auto selection = pfd::select_folder("Select a Folder", fullPath).result();
					if (!selection.empty())
					{
						m_SaveFilePath = selection.substr(fullPath.size());
					}
				}

				if(m_SaveFilePath[0] == '\0' || m_SaveFileName[0] == '\0')
				{
					ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
					ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
				}
				
				if (ImGui::Button("OK", ImVec2(150, 0)))
				{
					m_OpenSavePopup = false;
					ProtoScenes.GetActiveScene()->Save(m_SaveFilePath, m_SaveFileName);
					ImGui::CloseCurrentPopup();
				}

				if (m_SaveFilePath[0] == '\0' || m_SaveFileName[0] == '\0')
				{
					ImGui::PopItemFlag();
					ImGui::PopStyleVar();
				}

				ImGui::SetItemDefaultFocus();
				ImGui::SameLine();

				if (ImGui::Button("Cancel", ImVec2(150, 0)))
				{
					m_OpenSavePopup = false;
					ImGui::CloseCurrentPopup();
				}

				ImGui::EndPopup();
			}
		}

		void DrawHierarchy() const
		{
			const std::string title{ "ProtoHierarchy V" + std::to_string(HIERARCHY_VERSION).substr(0, 3) };

			ImGui::Begin(title.c_str());

			ProtoScenes.DrawHierarchy();

			ImGui::End();
		}

		void DrawInspector()
		{
			if (!m_pCurrentSelected)
				return;

			const std::string title{ "ProtoInspector V" + std::to_string(INSPECTOR_VERSION).substr(0, 3) };

			ImGui::Begin(title.c_str());

			if (!m_pCurrentSelected->DrawInspector())
			{
				m_pCurrentSelected = nullptr;
				ImGui::End();
				return;
			}

			ImGui::Separator();

			DrawAddComponent();

			ImGui::End();
		}

		GameObject* m_pCurrentSelected{};
		bool m_OpenSavePopup{ false };
		std::string m_SaveFilePath{};
		std::string m_SaveFileName{};
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

			if (ImGui::Selectable("Image"))
				m_pCurrentSelected->AddComponent(new ImageComponent(nullptr, {}));

			if (ImGui::Selectable("Text"))
				m_pCurrentSelected->AddComponent(new TextComponent("", nullptr, {}));

			if (ImGui::Selectable("FPS Component"))
				m_pCurrentSelected->AddComponent(new FPSComponent(nullptr, {}));
			
			ImGui::EndPopup();
		}
	}
}
