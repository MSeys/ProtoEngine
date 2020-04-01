#pragma once
#include <string>

class BaseGame;
struct SDL_Window;

namespace Proto
{
	struct WindowSettings
	{
		std::string title;
		int windowWidth;
		int windowHeight;
	};
	
	class Engine
	{
	public:
		Engine(BaseGame* pGame, WindowSettings windowSettings = { "ProtoEngine", 640, 480 });
		~Engine();
		
		void Run(); 
		
	private:
		static const int MsPerFrame = 16; //16 for 60 fps, 33 for 30 fps
		SDL_Window* m_Window{};

		BaseGame* m_pGame;
		WindowSettings m_WindowSettings;
		bool m_Exit{ false };
		
		void Initialize();
		void InitializeSDL();
		void InitializeEngineParts() const;
		
		void Cleanup();
		void CleanupSDL();
		void CleanupEngineParts();
		
	};
}