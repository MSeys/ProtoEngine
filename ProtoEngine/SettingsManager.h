#pragma once
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

struct WindowSettings
{
	std::string Title{ "ProtoEngine" };
	int WindowWidth{ 640 };
	int WindowHeight{ 480 };
	FPSState FPSState{ FPSState::PROTO_UNCAPPED };
	FPSRate FPSRate{ FPSRate::PROTO_FPS_NORMAL };
};

namespace Proto
{
	class SettingsManager : public Singleton<SettingsManager>
	{
	public:
		void Init(const WindowSettings& windowSettings) { m_WindowSettings = windowSettings; }
		
		WindowSettings GetWindowSettings() const { return m_WindowSettings; }
		
	private:
		WindowSettings m_WindowSettings{};
	};
}