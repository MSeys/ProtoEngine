#pragma once

#pragma warning(push)
#pragma warning(disable : 4201)
#include "glm/glm.hpp"
#pragma warning(pop)

#include "Singleton.h"
#include <string>

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

struct RenderSettings
{
	RenderMode RenderMode{ RenderMode::EDITOR };
	glm::vec2 WindowSizeOffset{ 0, 0 };
	glm::vec2 GameRenderOffset{ 0, 0 };
};

struct WindowSettings
{
	std::string Title{ "ProtoEngine" };
	glm::vec2 WindowSize{ 640, 480 };
	FPSState FPSState{ FPSState::PROTO_UNCAPPED };
	FPSRate FPSRate{ FPSRate::PROTO_FPS_NORMAL };
};

namespace Proto
{
	class SettingsManager : public Singleton<SettingsManager>
	{
	public:
		void Init(const RenderSettings& renderSettings, const WindowSettings& windowSettings)
		{
			m_RenderSettings = renderSettings;
			m_WindowSettings = windowSettings;
		}

		RenderSettings GetRenderSettings() const { return m_RenderSettings; }
		WindowSettings GetWindowSettings() const { return m_WindowSettings; }

		glm::vec2 GetWindowSize() const
		{
			glm::vec2 finalWindowSize{ m_WindowSettings.WindowSize };

			if(m_RenderSettings.RenderMode == RenderMode::EDITOR)
			{
				finalWindowSize.x += GetRenderSettings().WindowSizeOffset.x;
				finalWindowSize.y += GetRenderSettings().WindowSizeOffset.y;
			}

			return finalWindowSize;
		}
		
	private:
		RenderSettings m_RenderSettings{};
		WindowSettings m_WindowSettings{};
	};
}