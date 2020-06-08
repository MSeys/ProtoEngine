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

	ImGui::SetNextWindowDockID(m_BottomDockSpace, ImGuiCond_FirstUseEver);
	ProtoLogger.Draw();

	DrawInfo();
	
	DrawHierarchy();
	DrawInspector();

	DrawViewWindow();
	DrawGameWindow();

	DrawMenu();
	DrawEditorModeMenu();

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
		if (ImGui::BeginMenu("Editor", ProtoSettings.GetEditorMode() == EditorMode::EDIT))
		{
			if (ImGui::MenuItem("Reset Selected"))
				m_pCurrentSelected = nullptr;

			if (ImGui::MenuItem("Exit"))
				ProtoCommands.ForceExit();

			ImGui::EndMenu();
		}
	
		if (ImGui::BeginMenu("File", ProtoSettings.GetEditorMode() == EditorMode::EDIT))
		{
			if (ImGui::MenuItem("New"))
				m_OpenNewPopup = true;

			if (ImGui::MenuItem("Save"))
			{
				ProtoScenes.GetCurrentScene()->Save();
				ProtoLogger.AddLog(LogLevel::Info, "Scene \'" + ProtoConvert::ToString(ProtoScenes.GetCurrentScene()->GetSceneName()) + "\' was saved.");
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
						const std::string filePath{ selection[0].substr(fullPath.size()) };

						auto pNewScene = new Scene(filePath);
						pNewScene->Load();
						
						ProtoScenes.Add(pNewScene);
						ProtoScenes.SetCurrentScene(pNewScene->GetSceneName());

						m_pCurrentSelected = nullptr; // Selected from previous Scene doesn't count here anymore

						// Load new editor values
						m_SaveFileFolderStructure = pNewScene->GetFileFolderStructure();
						m_SaveFileName = pNewScene->GetFileName();
						m_SceneName = ProtoConvert::ToString(pNewScene->GetSceneName());
					}
				}
				
				if(ImGui::BeginMenu("Load Existing..."))
				{
					for(Scene* pScene : ProtoScenes.GetScenes())
					{
						if (pScene->GetSceneName() == L"Temp")
							continue;
						
						if (ImGui::MenuItem(ProtoConvert::ToString(pScene->GetSceneName()).c_str()))
						{
							ProtoScenes.SetCurrentScene(pScene->GetSceneName());
							ProtoScenes.GetCurrentScene()->Reset();
							ProtoScenes.GetCurrentScene()->Load();
							
							m_SceneName = ProtoConvert::ToString(pScene->GetSceneName());
							m_SaveFileFolderStructure = pScene->GetFileFolderStructure();
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

		if (ImGui::BeginMenu("Add", ProtoSettings.GetEditorMode() == EditorMode::EDIT))
		{
			if (ImGui::MenuItem("GameObject"))
				ProtoScenes.GetCurrentScene()->AddChild(new GameObject(ProtoScenes.GetCurrentScene()->RequestNewID()));
			
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	if (m_OpenNewPopup)
		ImGui::OpenPopup("New...");

	if (m_OpenSaveAsPopup)
		ImGui::OpenPopup("Save As...");

	if (ImGui::BeginPopupModal("New...", NULL, ImGuiWindowFlags_AlwaysAutoResize))
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

		m_SaveFileFolderStructure.resize(300);
		ImGui::Text("File Location: ");
		ImGui::SameLine();
		ImGui::InputText("##SAVE_FILE_PATH", &m_SaveFileFolderStructure[0], 300, ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();

		if (ImGui::Button("...##SAVE_FILE_BUTTON"))
		{
			const std::string fullPath = fs::canonical(ProtoContent.GetDataPath()).string();
			const auto selection = pfd::select_folder("Select a Folder", fullPath).result();
			if (!selection.empty())
			{
				m_SaveFileFolderStructure = selection.substr(fullPath.size());
			}
		}

		if (m_SceneName[0] == '\0' || m_SaveFileFolderStructure[0] == '\0' || m_SaveFileName[0] == '\0')
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}

		if (ImGui::Button("OK", ImVec2(150, 0)))
		{
			m_OpenNewPopup = false;

			auto pNewScene = new Scene(ProtoConvert::ToWString(m_SceneName), m_SaveFileFolderStructure, m_SaveFileName);
			pNewScene->Save();
			
			ProtoScenes.Add(pNewScene);
			ProtoScenes.SetCurrentScene(pNewScene->GetSceneName());

			m_pCurrentSelected = nullptr;
			
			ImGui::CloseCurrentPopup();
		}

		if (m_SceneName[0] == '\0' || m_SaveFileFolderStructure[0] == '\0' || m_SaveFileName[0] == '\0')
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

		m_SaveFileFolderStructure.resize(300);
		ImGui::Text("File Location: ");
		ImGui::SameLine();
		ImGui::InputText("##SAVE_FILE_PATH", &m_SaveFileFolderStructure[0], 300, ImGuiInputTextFlags_ReadOnly);
		ImGui::SameLine();

		if (ImGui::Button("...##SAVE_FILE_BUTTON"))
		{
			const std::string fullPath = fs::canonical(ProtoContent.GetDataPath()).string();
			const auto selection = pfd::select_folder("Select a Folder", fullPath).result();
			if (!selection.empty())
			{
				m_SaveFileFolderStructure = selection.substr(fullPath.size());
			}
		}

		if (m_SceneName[0] == '\0' || m_SaveFileFolderStructure[0] == '\0' || m_SaveFileName[0] == '\0')
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}

		if (ImGui::Button("OK", ImVec2(150, 0)))
		{
			m_OpenSaveAsPopup = false;

			auto pNewScene = new Scene(ProtoConvert::ToWString(m_SceneName), m_SaveFileFolderStructure, m_SaveFileName);
			pNewScene->LoadAs(ProtoScenes.GetCurrentScene()->GetFilePath());
			pNewScene->Save(); // Actually also immediately save as file so it isn't just memory.

			ProtoScenes.Add(pNewScene);
			ProtoScenes.SetCurrentScene(pNewScene->GetSceneName());

			m_pCurrentSelected = nullptr;
			
			ImGui::CloseCurrentPopup();
		}

		if (m_SceneName[0] == '\0' || m_SaveFileFolderStructure[0] == '\0' || m_SaveFileName[0] == '\0')
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

	if (ImGui::Begin("Editor"))
	{
		ImGui::Indent(ImGui::GetWindowWidth() / 2 - 150);

		EditorMode newMode{ ProtoSettings.GetEditorMode() };
		if (ProtoSettings.GetEditorMode() == EditorMode::PLAY)
		{
			ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
			ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
		}

		if (ImGui::Button("Play", { 150, 30 }))
		{
			newMode = EditorMode::PLAY;

			m_CurrentSelectedID = 0;
			if (m_pCurrentSelected)
				m_CurrentSelectedID = m_pCurrentSelected->GetID();

			ProtoScenes.GetCurrentScene()->Save();
			ProtoScenes.GetCurrentScene()->Reset();
			ProtoScenes.GetCurrentScene()->Load();

			if (m_CurrentSelectedID != 0)
				m_pCurrentSelected = ProtoScenes.GetCurrentScene()->FindGameObjectWithID(m_CurrentSelectedID);

			ProtoTime.TimeScale = 1;

			ProtoScenes.GetCurrentScene()->Start();
			ProtoScenes.GetCurrentScene()->Awake();
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

			ProtoScenes.GetCurrentScene()->Reset();
			ProtoScenes.GetCurrentScene()->Load();

			if (m_CurrentSelectedID != 0)
				m_pCurrentSelected = ProtoScenes.GetCurrentScene()->FindGameObjectWithID(m_CurrentSelectedID);

			ProtoTime.TimeScale = 0;
		}

		if (ProtoSettings.GetEditorMode() == EditorMode::EDIT)
		{
			ImGui::PopItemFlag();
			ImGui::PopStyleVar();
		}

		ProtoSettings.SetEditorMode(newMode);
	}
	
	ImGui::End();
}

void Proto::Editor::DrawInfo() const
{
	ImGui::SetNextWindowDockID(m_BottomDockSpace, ImGuiCond_FirstUseEver);
	if(ImGui::Begin("Info"))
	{
		ProtoGui::Presets::BeginGroupPanel("Engine");

		ImGui::Text(("TimeScale: " + std::to_string(ProtoTime.TimeScale)).c_str());
		ImGui::Text(("DeltaTime: " + std::to_string(ProtoTime.DeltaTime)).c_str());
		ImGui::Text(("FPS: " + std::to_string(ProtoTime.FPS)).c_str());

		ImGui::Text(("DeltaTime (U): " + std::to_string(ProtoTime.DeltaTime_Unscaled)).c_str());
		ImGui::Text(("FPS (U): " + std::to_string(ProtoTime.FPS_Unscaled)).c_str());

		ProtoGui::Presets::EndGroupPanel();

		ProtoGui::Presets::BeginGroupPanel("Game");

		ImGui::Text(("Scene: " + ProtoConvert::ToString(ProtoScenes.GetCurrentScene()->GetSceneName())).c_str());
		ImGui::Text("Camera: ");
		ImGui::SameLine();
		
		if(ProtoScenes.GetCurrentScene()->HasActiveCamera())
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 0, 1, 0, 1 });
			ImGui::Text("Found (Camera is Dynamic)");
			ImGui::PopStyleColor();
		}

		else
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ 1, 0, 0, 1 });
			ImGui::Text("None Found (Camera is Static)");
			ImGui::PopStyleColor();
		}

		ProtoGui::Presets::EndGroupPanel();
	}

	ImGui::End();
}

void Proto::Editor::DrawHierarchy() const
{
	ImGui::SetNextWindowDockID(m_LeftDockSpace, ImGuiCond_FirstUseEver);
	if(ImGui::Begin("Hierarchy"))
		ProtoScenes.DrawHierarchy();
	
	ImGui::End();
}

void Proto::Editor::DrawInspector() const
{
	if (!m_pCurrentSelected)
		return;

	ImGui::SetNextWindowDockID(m_RightDockSpace, ImGuiCond_FirstUseEver);
	if (ImGui::Begin("Inspector"))
	{
		m_pCurrentSelected->DrawInspector();
		ImGui::Separator();
		DrawAddComponent();
	}

	ImGui::End();
}

void Proto::Editor::DrawAddComponent() const
{
	ImGui::Spacing();
	ImGui::SameLine(50);
	if (ImGui::Button("Add Component", { 300, 25 }))
		ImGui::OpenPopup("AddComponentPopup");

	ImGui::SetNextWindowContentSize(ImVec2(250, 0.0f));
	if (ImGui::BeginPopup("AddComponentPopup", ImGuiWindowFlags_AlwaysAutoResize))
	{
		DrawAddComponentsList();
		
		ImGui::EndPopup();
	}
}

void Proto::Editor::DrawAddComponentsList() const
{
	if (ImGui::BeginTabBar("##AddComponentsTabs", ImGuiTabBarFlags_None))
	{
		if (ImGui::BeginTabItem("General"))
		{
			if (ImGui::Selectable("Camera"))
				m_pCurrentSelected->AddComponent(new Camera(m_pCurrentSelected->RequestNewID(), {}, false));
			
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Graphics"))
		{
			if (ImGui::Selectable("Sprite"))
				m_pCurrentSelected->AddComponent(new Sprite(m_pCurrentSelected->RequestNewID(), true, nullptr, {}));

			if (ImGui::Selectable("Text"))
				m_pCurrentSelected->AddComponent(new Text(m_pCurrentSelected->RequestNewID(), true, "", nullptr, {}));

			if (ImGui::Selectable("FPS Text"))
				m_pCurrentSelected->AddComponent(new FPSText(m_pCurrentSelected->RequestNewID(), true, nullptr, {}));


			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Physics"))
		{
			if (ImGui::Selectable("RigidBody2D"))
			{
				if (!m_pCurrentSelected->HasComponent<RigidBody2D>())
					m_pCurrentSelected->AddComponent(new RigidBody2D(m_pCurrentSelected->RequestNewID(), RigidBodyType::STATIC));
				else
					ProtoLogger.AddLog(LogLevel::Warning, "RigidBody2D found on GameObject \'" + m_pCurrentSelected->GetName() + "\'. Only ONE allowed.");
			}

			if (ImGui::Selectable("BoxCollider2D"))
				m_pCurrentSelected->AddComponent(new BoxCollider2D(m_pCurrentSelected->RequestNewID(), true));

			if (ImGui::Selectable("SphereCollider2D"))
				m_pCurrentSelected->AddComponent(new SphereCollider2D(m_pCurrentSelected->RequestNewID(), true));

			if (ImGui::Selectable("LineCollider2D"))
				m_pCurrentSelected->AddComponent(new LineCollider2D(m_pCurrentSelected->RequestNewID(), true));

			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("Game"))
		{
			ProtoSettings.GetRefGame()->DrawAddComponent(m_pCurrentSelected);

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}
}

void Proto::Editor::DrawViewWindow()
{
	ImGui::SetNextWindowDockID(m_MiddleDockSpace, ImGuiCond_Always);
	if (ImGui::Begin("Scene View"))
	{
		ProtoSettings.SetEditorRenderMode(RenderMode::EDITOR);
		
		const glm::vec2 windowSize{ ProtoConvert::ToGLMVec(ImGui::GetWindowSize()) }, viewTextureSize{ ProtoConvert::ToGLMVec(ImGui::GetWindowSize()) - 35.f };
		const float titleBarHeight{ ImGui::GetCurrentWindow()->TitleBarHeight() };
		
		SafeDelete(m_pViewWindow);
		SDL_Texture* pViewTexture{ SDL_CreateTexture(ProtoRenderer.GetSDLRenderer(), SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 
									int(viewTextureSize.x), int(viewTextureSize.y)) };
		
		m_pViewWindow = new Texture2D(pViewTexture, "", "");
		
		SDL_SetRenderTarget(ProtoRenderer.GetSDLRenderer(), m_pViewWindow->GetSDLTexture());
		SDL_RenderClear(ProtoRenderer.GetSDLRenderer());

		ProtoScenes.Draw();

		if (m_pCurrentSelected)
			m_pCurrentSelected->DrawEditorDebug();

		SDL_SetRenderTarget(ProtoRenderer.GetSDLRenderer(), nullptr);

		ImVec2 cursorPos;
		cursorPos.x = (windowSize.x - viewTextureSize.x) * 0.5f;
		cursorPos.y = (windowSize.y - titleBarHeight - viewTextureSize.y) * 0.5f + titleBarHeight;
		ImGui::SetCursorPos(cursorPos);
		ImGui::Image(m_pViewWindow->GetSDLTexture(), ProtoConvert::ToImGuiVec(viewTextureSize));
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

		const glm::vec2 windowSize{ ProtoConvert::ToGLMVec(ImGui::GetWindowSize()) };
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

		ProtoSettings.SetGameMouseOffset(ProtoConvert::ToGLMVec(ImGui::GetCursorScreenPos()));
		ImGui::Image(m_pGameWindow->GetSDLTexture(), ProtoConvert::ToImGuiVec(scaleYGameWindowSize));
	}
	ImGui::End();
}