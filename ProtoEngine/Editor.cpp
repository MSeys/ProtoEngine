#include "ProtoEnginePCH.h"
#include "Editor.h"

#include "Texture2D.h"

void Proto::Editor::Init()
{
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	SDL_Texture* pGameTexture{ SDL_CreateTexture(ProtoRenderer.GetSDLRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 
										int(ProtoSettings.GetWindowSettings().GameWindowSize.x), int(ProtoSettings.GetWindowSettings().GameWindowSize.y)) };

	m_pGameWindow = new Texture2D(pGameTexture, "", "");
}

void Proto::Editor::Draw()
{
	DrawDocks();

	DrawMenu();
	DrawEditorModeMenu();

	ImGui::SetNextWindowDockID(m_BottomDockSpace, ImGuiCond_FirstUseEver);
	ProtoLogger.Draw();
	
	DrawHierarchy();
	DrawInspector();

	DrawViewWindow();
	DrawGameWindow();

	ImGui::ShowDemoWindow();
}

void Proto::Editor::Destroy()
{
	SafeDelete(m_pViewWindow);
	SafeDelete(m_pGameWindow);
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


	
	ImGui::SetNextWindowPos({ 350, 120 }, ImGuiCond_Always);
	ImGui::SetNextWindowSize({ ProtoSettings.GetEditorSettings().EditorWindowSize.x - 750, ProtoSettings.GetEditorSettings().EditorWindowSize.y - 420 }, ImGuiCond_Always);

	ImGui::Begin("##DOCKMIDDLE", nullptr, windowFlags);

	m_MiddleDockSpace = ImGui::GetID("MiddleDockSpace");
	ImGui::DockSpace(m_MiddleDockSpace, {}, dockSpaceFlags);

	ImGui::End();

	
	
	ImGui::SetNextWindowPos({ 350, ProtoSettings.GetEditorSettings().EditorWindowSize.y - 300 }, ImGuiCond_Always);
	ImGui::SetNextWindowSize({ ProtoSettings.GetEditorSettings().EditorWindowSize.x - 750, 300 }, ImGuiCond_Always);

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
				ProtoScenes.GetActiveScene()->AddChild(new GameObject(ProtoScenes.GetActiveScene()->RequestNewID()));
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (m_OpenNewPopup)
		ImGui::OpenPopup("Open New...");

	if (m_OpenSaveAsPopup)
		ImGui::OpenPopup("Save As...");

	if (ImGui::BeginPopupModal("Open New...", NULL, ImGuiWindowFlags_AlwaysAutoResize))
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

	if (ImGui::BeginPopupModal("Save As...", NULL, ImGuiWindowFlags_AlwaysAutoResize))
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
	ImGui::SetNextWindowDockID(m_TopDockSpace, ImGuiCond_Always);
	ImGui::Begin("Editor");
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
	ImGui::SetNextWindowDockID(m_LeftDockSpace, ImGuiCond_FirstUseEver);
	ImGui::Begin("Hierarchy");
	ProtoScenes.DrawHierarchy();
	ImGui::End();
}

void Proto::Editor::DrawInspector()
{
	if (!m_pCurrentSelected)
		return;

	ImGui::SetNextWindowDockID(m_RightDockSpace, ImGuiCond_FirstUseEver);
	ImGui::Begin("Inspector");

	m_pCurrentSelected->DrawInspector();

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

void Proto::Editor::DrawViewWindow()
{
	ImGui::SetNextWindowDockID(m_MiddleDockSpace, ImGuiCond_Always);
	if (ImGui::Begin("Scene View"))
	{
		ProtoSettings.SetEditorRenderMode(RenderMode::EDITOR);
		
		const glm::vec2 windowSize{ ImGui::GetWindowSize().x, ImGui::GetWindowSize().y }, viewTextureSize{ ImGui::GetWindowSize().x - 35.f, ImGui::GetWindowSize().y - 35.f };
		const float titleBarHeight{ ImGui::GetCurrentWindow()->TitleBarHeight() };
		
		SafeDelete(m_pViewWindow);
		SDL_Texture* pViewTexture{ SDL_CreateTexture(ProtoRenderer.GetSDLRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 
									int(ImGui::GetWindowSize().x - 35), int(ImGui::GetWindowSize().y - 35)) };
		
		m_pViewWindow = new Texture2D(pViewTexture, "", "");
		
		SDL_SetRenderTarget(ProtoRenderer.GetSDLRenderer(), m_pViewWindow->GetSDLTexture());
		SDL_RenderClear(ProtoRenderer.GetSDLRenderer());

		ProtoScenes.Draw();

		SDL_SetRenderTarget(ProtoRenderer.GetSDLRenderer(), nullptr);

		ImVec2 cursorPos;
		cursorPos.x = (windowSize.x - viewTextureSize.x) * 0.5f;
		cursorPos.y = (windowSize.y - titleBarHeight - viewTextureSize.y) * 0.5f + titleBarHeight;
		ImGui::SetCursorPos(cursorPos);
		ImGui::Image(m_pViewWindow->GetSDLTexture(), { viewTextureSize.x, viewTextureSize.y });
	}
	ImGui::End();
}

void Proto::Editor::DrawGameWindow() const
{
	ImGui::SetNextWindowDockID(m_MiddleDockSpace, ImGuiCond_Always);
	if (ImGui::Begin("Game View"))
	{
		if (ImGui::IsWindowHovered())
		{
			ImGui::CaptureKeyboardFromApp(false);
			ImGui::CaptureMouseFromApp(false);
		}

		const glm::vec2 windowSize{ ImGui::GetWindowSize().x, ImGui::GetWindowSize().y };
		const float titleBarHeight{ ImGui::GetCurrentWindow()->TitleBarHeight() };
		const glm::vec2 gameWindowSize{ ProtoSettings.GetWindowSettings().GameWindowSize };
		
		ProtoSettings.SetEditorRenderMode(RenderMode::GAME);
		SDL_SetRenderTarget(ProtoRenderer.GetSDLRenderer(), m_pGameWindow->GetSDLTexture());
		SDL_RenderClear(ProtoRenderer.GetSDLRenderer());

		ProtoScenes.Draw();

		SDL_SetRenderTarget(ProtoRenderer.GetSDLRenderer(), nullptr);

		glm::vec2 scaleXGameWindowSize{ gameWindowSize };
		if(gameWindowSize.x > windowSize.x - 35)
		{
			ProtoSettings.SetGameAspectRatio((windowSize.x - 35) / gameWindowSize.x);
			scaleXGameWindowSize = gameWindowSize * ProtoSettings.GetGameAspectRatio();
		}

		glm::vec2 scaleYGameWindowSize{ scaleXGameWindowSize };
		if(scaleXGameWindowSize.y > windowSize.y - 35)
		{
			ProtoSettings.SetGameAspectRatio((windowSize.y - 35) / gameWindowSize.y);
			scaleYGameWindowSize = gameWindowSize * ProtoSettings.GetGameAspectRatio();
		}
		
		ImVec2 cursorPos;
		cursorPos.x = (windowSize.x - scaleYGameWindowSize.x) * 0.5f;
		cursorPos.y = (windowSize.y - titleBarHeight - scaleYGameWindowSize.y) * 0.5f + titleBarHeight;
		ImGui::SetCursorPos(cursorPos);

		ProtoSettings.SetGameMouseOffset({ ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y });
		ImGui::Image(m_pGameWindow->GetSDLTexture(), { scaleYGameWindowSize.x, scaleYGameWindowSize.y });
	}
	ImGui::End();
}