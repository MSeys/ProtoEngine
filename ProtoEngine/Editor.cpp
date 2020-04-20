#include "ProtoEnginePCH.h"
#include "Editor.h"

void Proto::Editor::Init()
{
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

void Proto::Editor::Draw()
{
	DrawDocks();

	SDL_Rect fullWindow{
			0,
			0,
			int(ProtoSettings.GetEditorSettings().EditorWindowSize.x),
			int(ProtoSettings.GetEditorSettings().EditorWindowSize.y) };

	SDL_Rect gameWindow{
			int(ProtoSettings.GetEditorSettings().GameRenderOffset.x),
			int(ProtoSettings.GetEditorSettings().GameRenderOffset.y),
			int(ProtoSettings.GetWindowSettings().GameWindowSize.x),
			int(ProtoSettings.GetWindowSettings().GameWindowSize.y) };

	if(ProtoSettings.GetEditorMode() == EditorMode::EDIT)
	{
		gameWindow.x += int(ProtoSettings.GetEditorCamera().x);
		gameWindow.y += int(ProtoSettings.GetEditorCamera().y);
	}

	SDL_SetRenderDrawColor(ProtoRenderer.GetSDLRenderer(), 25, 25, 25, 80);
	SDL_RenderFillRect(ProtoRenderer.GetSDLRenderer(), &fullWindow);
	SDL_SetRenderDrawColor(ProtoRenderer.GetSDLRenderer(), 0, 0, 0, 255);
	SDL_RenderFillRect(ProtoRenderer.GetSDLRenderer(), &gameWindow);

	DrawMenu();
	DrawEditorModeMenu();

	ImGui::SetNextWindowDockID(m_BottomDockSpace, ImGuiCond_FirstUseEver);
	ProtoLogger.Draw();
	
	DrawHierarchy();
	DrawInspector();

	ImGui::ShowDemoWindow();
}

void Proto::Editor::DrawDocks()
{
	const ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize;
	const ImGuiDockNodeFlags dockSpaceFlags = ImGuiDockNodeFlags_None;



	ImGui::SetNextWindowPos({ 0, 20 }, ImGuiCond_Always);
	ImGui::SetNextWindowSize({ 350, ProtoSettings.GetEditorSettings().EditorWindowSize.y - 20 }, ImGuiCond_Always);

	ImGui::Begin("##DOCKLEFT", nullptr, windowFlags);

	m_LeftDockSpace = ImGui::GetID("LeftDockSpace");
	ImGui::DockSpace(m_LeftDockSpace, {}, dockSpaceFlags);

	ImGui::End();



	ImGui::SetNextWindowPos({ ProtoSettings.GetEditorSettings().EditorWindowSize.x - 400, 20 }, ImGuiCond_Always);
	ImGui::SetNextWindowSize({ 400, ProtoSettings.GetEditorSettings().EditorWindowSize.y - 20 }, ImGuiCond_Always);

	ImGui::Begin("##DOCKRIGHT", nullptr, windowFlags);

	m_RightDockSpace = ImGui::GetID("RightDockSpace");
	ImGui::DockSpace(m_RightDockSpace, {}, dockSpaceFlags);

	ImGui::End();

	ImGui::SetNextWindowPos({ 350, 20 }, ImGuiCond_Always);
	ImGui::SetNextWindowSize({ ProtoSettings.GetEditorSettings().EditorWindowSize.x - 750, 100 }, ImGuiCond_Always);

	ImGui::Begin("##DOCKTOP", nullptr, windowFlags);

	m_TopDockSpace = ImGui::GetID("TopDockSpace");
	ImGui::DockSpace(m_TopDockSpace, {}, dockSpaceFlags);

	ImGui::End();

	ImGui::SetNextWindowPos({ 350, ProtoSettings.GetEditorSettings().EditorWindowSize.y - 350 }, ImGuiCond_Always);
	ImGui::SetNextWindowSize({ ProtoSettings.GetEditorSettings().EditorWindowSize.x - 750, 350 }, ImGuiCond_Always);

	ImGui::Begin("##DOCKBOTTOM", nullptr, windowFlags);

	m_BottomDockSpace = ImGui::GetID("BottomDockSpace");
	ImGui::DockSpace(m_BottomDockSpace, {}, dockSpaceFlags);

	ImGui::End();
}

void Proto::Editor::DrawMenu()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New"))
				m_OpenNewPopup = true;

			if (ImGui::MenuItem("Save"))
			{
				ProtoScenes.GetActiveScene()->Save(m_SaveFilePath, m_SaveFileName);
				ProtoLogger.AddLog(LogLevel::Info, "Scene \'" + WStringToString(ProtoScenes.GetActiveScene()->GetSceneName()) + "\' was saved.");
			}

			if (ImGui::MenuItem("Save As..."))
				m_OpenSaveAsPopup = true;
			
			if(ImGui::BeginMenu("Load"))
			{
				if(ImGui::MenuItem("Load New..."))
				{
					const std::string fullPath = fs::canonical(ProtoContent.GetDataPath()).string();
					const auto selection = pfd::open_file("Select a Scene", fullPath, { "ProtoScenes", "*.protoscene" }).result();
					if (!selection.empty())
					{
						auto pNewScene = new Scene();
						pNewScene->Load(selection[0].substr(fullPath.size()), &m_SaveFilePath, &m_SaveFileName);
						ProtoScenes.AddGameScene(pNewScene);
						ProtoScenes.SetActiveGameScene(pNewScene->GetSceneName());

						m_pCurrentSelected = nullptr; // Selected from previous Scene doesn't count here anymore
						m_SceneName = WStringToString(pNewScene->GetSceneName());
					}
				}
				
				if(ImGui::BeginMenu("Load Existing..."))
				{
					for(Scene* pScene : ProtoScenes.GetScenes())
					{
						if (pScene == ProtoScenes.GetActiveScene() || pScene->GetSceneName() == L"Temp")
							continue;
						
						if (ImGui::MenuItem(WStringToString(pScene->GetSceneName()).c_str()))
						{
							ProtoScenes.SetActiveGameScene(pScene->GetSceneName());
							
							m_SceneName = WStringToString(pScene->GetSceneName());
							m_SaveFilePath = pScene->GetFilePath();
							m_SaveFileName = pScene->GetFileName();
							
							m_pCurrentSelected = nullptr; // Selected from previous Scene doesn't count here anymore
						}
					}
					
					ImGui::EndMenu();
				}

				ImGui::EndMenu();
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

	if (m_OpenNewPopup)
		ImGui::OpenPopup("NewPopup");

	if (m_OpenSaveAsPopup)
		ImGui::OpenPopup("SaveAsPopup");

	if (ImGui::BeginPopupModal("NewPopup", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Please fill in the necessary info to create a new scene.\nNote: Stick to Data folder for saving your scenes.");
		ImGui::Separator();

		m_SceneName.resize(50);
		ImGui::Text("Scene Name: ");
		ImGui::SameLine();
		ImGui::InputText("##SAVE_FILE_SCENE_NAME", &m_SceneName[0], 50);
		
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

		if (m_SceneName[0] == '\0' || m_SaveFilePath[0] == '\0' || m_SaveFileName[0] == '\0')
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}

		if (ImGui::Button("OK", ImVec2(150, 0)))
		{
			m_OpenNewPopup = false;

			auto pNewScene = new Scene();
			pNewScene->SetSceneName(StringToWString(m_SceneName));
			pNewScene->SetFilePath(m_SaveFilePath);
			pNewScene->SetFileName(m_SaveFileName);
			
			ProtoScenes.AddGameScene(pNewScene);
			ProtoScenes.SetActiveGameScene(pNewScene->GetSceneName());

			m_pCurrentSelected = nullptr;
			
			ImGui::CloseCurrentPopup();
		}

		if (m_SceneName[0] == '\0' || m_SaveFilePath[0] == '\0' || m_SaveFileName[0] == '\0')
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}

		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(150, 0)))
		{
			m_OpenNewPopup = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if (ImGui::BeginPopupModal("SaveAsPopup", NULL, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::Text("Please fill in the necessary info to create save your scene.\nNote: Stick to Data folder for saving your scenes.");
		ImGui::Separator();

		m_SceneName.resize(50);
		ImGui::Text("Scene Name: ");
		ImGui::SameLine();
		ImGui::InputText("##SAVE_FILE_SCENE_NAME", &m_SceneName[0], 50);

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

		if (m_SceneName[0] == '\0' || m_SaveFilePath[0] == '\0' || m_SaveFileName[0] == '\0')
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}

		if (ImGui::Button("OK", ImVec2(150, 0)))
		{
			m_OpenSaveAsPopup = false;

			const std::wstring oldSceneName{ ProtoScenes.GetActiveScene()->GetSceneName() }; // Temporarily store old scene name

			ProtoScenes.GetActiveScene()->SetSceneName(StringToWString(m_SceneName)); // Temporarily change Scene name to new Scene name
			ProtoScenes.GetActiveScene()->Save(m_SaveFilePath, m_SaveFileName); // Saves to new file path and name, with the new Scene name. // READY FOR LOAD
			ProtoScenes.GetActiveScene()->SetSceneName(oldSceneName); // Reset Scene name
			
			// Build up path for Loading // c_str() is necessary as strings have reserved space (filled with '\0')
			const std::string path{ ProtoContent.GetDataPath() + m_SaveFilePath.c_str() + "\\" + m_SaveFileName.c_str() + ".protoscene" }; // NOLINT(readability-redundant-string-cstr)
			
			Scene* pNewScene = new Scene(path); // Create the new scene with the newly made file.
			ProtoScenes.AddGameScene(pNewScene);
			ProtoScenes.SetActiveGameScene(pNewScene->GetSceneName());

			m_SceneName = WStringToString(pNewScene->GetSceneName());
			m_pCurrentSelected = nullptr;
			
			ImGui::CloseCurrentPopup();
		}

		if (m_SceneName[0] == '\0' || m_SaveFilePath[0] == '\0' || m_SaveFileName[0] == '\0')
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}

		ImGui::SetItemDefaultFocus();
		ImGui::SameLine();

		if (ImGui::Button("Cancel", ImVec2(150, 0)))
		{
			m_OpenSaveAsPopup = false;
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void Proto::Editor::DrawEditorModeMenu()
{
	const std::string title{ "ProtoEditor V" + std::to_string(EDITOR_MENU_VERSION).substr(0, 3) + " | " + WStringToString(ProtoScenes.GetActiveScene()->GetSceneName()) };

	ImGui::SetNextWindowDockID(m_TopDockSpace, ImGuiCond_Always);
	ImGui::Begin(title.c_str());
	ImGui::Indent(ImGui::GetWindowWidth() / 2 - 150);
	
	EditorMode newMode{ ProtoSettings.GetEditorMode() };
	if(ProtoSettings.GetEditorMode() == EditorMode::PLAY)
	{
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}
	
	if (ImGui::Button("Play", { 150, 30 }))
	{
		newMode = EditorMode::PLAY;
		ProtoScenes.GetActiveScene()->Save(m_SaveFilePath, m_SaveFileName);

		ProtoTime.TimeScale = 1;
		ProtoScenes.Awake();
	}

	if (ProtoSettings.GetEditorMode() == EditorMode::PLAY)
	{
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	ImGui::SameLine();

	if (ProtoSettings.GetEditorMode() == EditorMode::EDIT)
	{
		ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
		ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
	}

	if (ImGui::Button("Edit", { 150, 30 }))
	{
		newMode = EditorMode::EDIT;

		Scene* oldScene = ProtoScenes.GetActiveScene();
		ProtoScenes.RemoveGameScene(oldScene);

		// Build up path for Loading // c_str() is necessary as strings have reserved space (filled with '\0')
		const std::string path{ ProtoContent.GetDataPath() + m_SaveFilePath.c_str() + "\\" + m_SaveFileName.c_str() + ".protoscene" }; // NOLINT(readability-redundant-string-cstr)
		const auto pNewScene{ new Scene(path) };

		ProtoScenes.AddGameScene(pNewScene);
		ProtoScenes.SetActiveGameScene(pNewScene->GetSceneName());

		SafeDelete(oldScene);
		m_pCurrentSelected = nullptr;

		ProtoTime.TimeScale = 0;
	}

	if (ProtoSettings.GetEditorMode() == EditorMode::EDIT)
	{
		ImGui::PopItemFlag();
		ImGui::PopStyleVar();
	}

	ProtoSettings.SetEditorMode(newMode);
	
	ImGui::End();
}

void Proto::Editor::DrawHierarchy() const
{
	const std::string title{ "ProtoHierarchy V" + std::to_string(HIERARCHY_VERSION).substr(0, 3) };

	ImGui::SetNextWindowDockID(m_LeftDockSpace, ImGuiCond_FirstUseEver);
	ImGui::Begin(title.c_str());
	ProtoScenes.DrawHierarchy();
	ImGui::End();
}

void Proto::Editor::DrawInspector()
{
	if (!m_pCurrentSelected)
		return;

	const std::string title{ "ProtoInspector V" + std::to_string(INSPECTOR_VERSION).substr(0, 3) };

	ImGui::SetNextWindowDockID(m_RightDockSpace, ImGuiCond_FirstUseEver);
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

void Proto::Editor::DrawAddComponent() const
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

		ProtoSettings.GetRefGame()->DrawAddComponent();
		
		ImGui::EndPopup();
	}
}
