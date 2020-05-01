#pragma once
#include "Scene.h"
#include "GameObject.h"
#include "Singleton.h"

#include "Components.h"
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
		void SetSaveFilePath(const std::string& saveFilePath) { m_SaveFilePath = saveFilePath; }
		void SetSaveFileName(const std::string& saveFileName) { m_SaveFileName = saveFileName; }

	private:
		void DrawDocks();
		void DrawMenu();
		void DrawEditorModeMenu();

		void DrawHierarchy() const;
		void DrawInspector();
		void DrawAddComponent() const;

		void DrawViewWindow();
		void DrawGameWindow() const;
		
		GameObject* m_pCurrentSelected{};

		bool m_OpenNewPopup{ false };
		bool m_OpenSaveAsPopup{ false };
		
		std::string m_SceneName{};
		std::string m_SaveFilePath{};
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
