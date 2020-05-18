#pragma once
#include "Scene.h"
#include "GameObject.h"
#include "Singleton.h"

#include "EngineBehaviours.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_internal.h"
#include "portable-file-dialogs.h"

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

namespace Proto
{
	class Editor : public Singleton<Editor>
	{
	public:
		void Init();
		void Draw();
		void Destroy();
		
		void SetCurrentSelected(GameObject* pGameObject) { m_pCurrentSelected = pGameObject; }
		GameObject* GetCurrentSelected() const { return m_pCurrentSelected; }

		void SetSceneName(const std::string& sceneName) { m_SceneName = sceneName; }
		void SetSaveFileFolderStructure(const std::string& saveFileFolderStructure) { m_SaveFileFolderStructure = saveFileFolderStructure; }
		void SetSaveFileName(const std::string& saveFileName) { m_SaveFileName = saveFileName; }

	private:		
		void DrawDocks();
		void DrawMenu();
		void DrawEditorModeMenu();
		void DrawInfo() const;
		
		void DrawHierarchy() const;
		void DrawInspector() const;
		void DrawAddComponent() const;
		void DrawAddComponentsList() const;

		void DrawViewWindow();
		void DrawGameWindow() const;
		
		GameObject* m_pCurrentSelected{};

		bool m_OpenNewPopup{ false };
		bool m_OpenSaveAsPopup{ false };
		
		std::string m_SceneName{};
		std::string m_SaveFileFolderStructure{};
		std::string m_SaveFileName{};

		ImGuiID m_TopDockSpace{};
		ImGuiID m_BottomDockSpace{};
		ImGuiID m_LeftDockSpace{};
		ImGuiID m_RightDockSpace{};
		ImGuiID m_MiddleDockSpace{};

		Texture2D* m_pViewWindow{};
		Texture2D* m_pGameWindow{};
	};
}
