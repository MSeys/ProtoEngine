#pragma once

#pragma warning(push)
#pragma warning(disable : 4201)
#include "glm/glm.hpp"
#pragma warning(pop)

#include "Singleton.h"
#include <string>

#include "BaseGame.h"
#include "EditorGame.h"
#include "ProtoUtils.h"

enum class FPSRate
{
	PROTO_FPS_VERY_HIGH = 144,
	PROTO_FPS_HIGH = 120,
	PROTO_FPS_NORMAL = 60,
	PROTO_FPS_LOW = 30,
	PROTO_FPS_POTATO = 10
};

enum class FPSState
{
	PROTO_UNCAPPED, PROTO_CAPPED, PROTO_VSYNC
};

enum class RenderMode
{
	GAME, EDITOR
};

enum class EditorMode
{
	EDIT, PLAY
};

struct EditorSettings
{
	bool FullWindowMode = false;
	glm::vec2 EditorWindowSize{ 0, 0 };
};

struct WindowSettings
{
	std::string Title{ "ProtoEngine" };
	glm::vec2 GameWindowSize{ 640, 480 };
	FPSState FPSState{ FPSState::PROTO_UNCAPPED };
	FPSRate FPSRate{ FPSRate::PROTO_FPS_NORMAL };
};

namespace Proto
{
	class SettingsManager : public Singleton<SettingsManager>
	{
	public:
		void Init(BaseGame* pGame, BaseGame* pRefGame, const EditorSettings& editorSettings, const WindowSettings& windowSettings, const RenderMode& renderMode)
		{
			m_pGame = pGame;
			m_pRefGame = pRefGame;
			
			m_EditorSettings = editorSettings;
			m_WindowSettings = windowSettings;
			m_RenderMode = renderMode;
		}

		void Destroy()
		{
			if(m_pGame != m_pRefGame)
				SafeDelete(m_pRefGame);
			
			SafeDelete(m_pGame);
		}
		
		EditorSettings GetEditorSettings() const { return m_EditorSettings; }
		WindowSettings GetWindowSettings() const { return m_WindowSettings; }

		glm::vec2 GetWindowSize() const
		{
			if (m_RenderMode == RenderMode::EDITOR && m_EditorMode == EditorMode::EDIT)
				return m_EditorSettings.EditorWindowSize;

			return m_WindowSettings.GameWindowSize;
		}

		RenderMode GetRenderMode() const { return m_RenderMode; }
		
		void SetEditorMode(const EditorMode& editorMode) { m_EditorMode = editorMode; }
		EditorMode GetEditorMode() const { return m_EditorMode; }

		void SetEditorRenderMode(const RenderMode& editorRenderMode) { m_EditorRenderMode = editorRenderMode; }
		RenderMode GetEditorRenderMode() const { return m_EditorRenderMode; }

		void SetGameMouseOffset(const glm::vec2& offset) { m_GameMousePosOffset = offset; }
		glm::vec2 GetGameMouseOffset() const { return m_GameMousePosOffset; }

		void SetGameAspectRatio(float aspectRatio) { m_GameAspectRatio = aspectRatio; }
		float GetGameAspectRatio() const { return m_GameAspectRatio; }
		
		void TranslateEditorCamera(float x, float y)
		{
			m_EditorCamOffset.x += x;
			m_EditorCamOffset.y += y;
		}

		glm::vec2 GetEditorCamera() const { return m_EditorCamOffset; }
		BaseGame* GetGame() const { return m_pGame; }
		BaseGame* GetRefGame() const { return m_pRefGame; }
		
	private:
		EditorSettings m_EditorSettings{};
		WindowSettings m_WindowSettings{};

		RenderMode m_RenderMode{ RenderMode::GAME };
		EditorMode m_EditorMode{ EditorMode::EDIT };
		RenderMode m_EditorRenderMode{ RenderMode::EDITOR };
		
		BaseGame* m_pGame{}, *m_pRefGame{};
		
		glm::vec2 m_EditorCamOffset{ 0, 0 };
		glm::vec2 m_GameMousePosOffset{ 0, 0 };
		float m_GameAspectRatio{ 1 };
	};
}