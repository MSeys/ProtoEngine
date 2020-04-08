#pragma once
#include <string>
#include "SettingsManager.h"

class BaseGame;
struct SDL_Window;

namespace Proto
{
	class Engine
	{
	public:
		Engine(BaseGame* pGame, const RenderSettings& renderSettings = { RenderMode::GAME, { 0, 0 }, { 0, 0 }},
								const WindowSettings& windowSettings = { "ProtoEngine", {640, 480 }, FPSState::PROTO_VSYNC, FPSRate::PROTO_FPS_NORMAL });
		~Engine();
		
		Engine(const Engine& other) = delete;
		Engine(Engine&& other) noexcept = delete;
		Engine& operator=(const Engine& other) = delete;
		Engine& operator=(Engine&& other) noexcept = delete;
		
		void Run() const;
		
	private:
		SDL_Window* m_Window{};

		BaseGame* m_pGame;
		bool m_Exit{ false };
		
		void Initialize();
		void InitializeSDL();
		void InitializeEngineParts();
		void InitializeImGuiStyle();
		
		void Cleanup();
		void CleanupSDL();
		void CleanupEngineParts();
	};
}