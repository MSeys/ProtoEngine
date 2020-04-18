#include "ProtoEnginePCH.h"

#include "Engine.h"

#include <chrono>
#include <thread>
#include <SDL.h>

#include <utility>

#include "BaseGame.h"

Proto::Engine::Engine(BaseGame* pGame, const RenderSettings& renderSettings, const WindowSettings& windowSettings)
	: m_pGame(pGame)
{
	// ProtoSettings
	ProtoSettings.Init(renderSettings, windowSettings);
	
	Initialize();
}

Proto::Engine::~Engine()
{
	Cleanup();
}

void Proto::Engine::Run() const
{
	ProtoTime.SetStartTime(std::chrono::steady_clock::now());

	float fixedTimer{};
	while(!m_Exit)
	{
		ProtoTime.SetCurrTime(std::chrono::steady_clock::now());
		ProtoTime.UpdateTime();
		ProtoTime.SetStartTime(ProtoTime.GetCurrTime());

		ProtoInput.Update();
		ProtoInput.Process();
		
		ProtoScenes.Update();

		fixedTimer += ProtoTime.DeltaTime;
		while(fixedTimer >= ProtoTime.FixedDeltaTime)
		{
			ProtoScenes.FixedUpdate();
			fixedTimer -= ProtoTime.FixedDeltaTime;
		}
		
		ImGui::NewFrame();
		SDL_RenderClear(ProtoRenderer.GetSDLRenderer());

		if (ProtoSettings.GetRenderSettings().RenderMode == RenderMode::EDITOR)
		{
			ProtoEditor.DrawDocks();
			ProtoEditor.Draw();
		}
		
		ProtoScenes.Draw();

		ImGui::Render();
		ImGuiSDL::Render(ImGui::GetDrawData());

		SDL_RenderPresent(ProtoRenderer.GetSDLRenderer());

		ProtoCommands.ResetInputData();
	}
}

void Proto::Engine::Initialize()
{
	InitializeSDL();
	InitializeEngineParts();

	// Game
	m_pGame->RootInitialize();

	InitializeImGuiStyle();
}

void Proto::Engine::InitializeSDL()
{
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
		throw std::runtime_error(std::string("SDL_Init Error: ") + SDL_GetError());

	m_Window = SDL_CreateWindow(
		ProtoSettings.GetWindowSettings().Title.c_str(),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		int(ProtoSettings.GetWindowSize().x),
		int(ProtoSettings.GetWindowSize().y),
		SDL_WINDOW_OPENGL
	);

	if (!m_Window)
		throw std::runtime_error(std::string("SDL_CreateWindow Error: ") + SDL_GetError());
}

void Proto::Engine::InitializeEngineParts()
{
	// ProtoRenderer
	ProtoRenderer.Init(m_Window);

	// ProtoEditor
	ProtoEditor.Init();
	
	// ProtoCommands
	ProtoCommands.Init();
	ProtoCommands.GetCommand(COMMAND_EXIT).SetExecuteData(&m_Exit);
	
	// ProtoInput
	ProtoInput.Init();
	ProtoInput.AddKey(SDLK_ESCAPE, STRINGIFY(SDLK_ESCAPE));
	ProtoInput.GetKey(SDLK_ESCAPE).SetCommand(ButtonState::Pressed, COMMAND_EXIT);
	
	// ProtoResource
	ProtoContent.Init("../Data/");

	// ProtoAudio
	ProtoAudio::Init();
}

void Proto::Engine::InitializeImGuiStyle()
{
	ImGuiStyle* style = &ImGui::GetStyle();
	ImVec4* colors = style->Colors;
	
	colors[ImGuiCol_FrameBg] = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.200f, 0.200f, 0.200f, 1.000f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.280f, 0.280f, 0.280f, 1.000f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.148f, 0.148f, 0.148f, 1.000f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.195f, 0.195f, 0.195f, 1.000f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.160f, 0.160f, 0.160f, 1.000f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.277f, 0.277f, 0.277f, 1.000f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.300f, 0.300f, 0.300f, 1.000f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImColor_Blue;
	colors[ImGuiCol_CheckMark] = ImColor_Blue;
	colors[ImGuiCol_SliderGrab] = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
	colors[ImGuiCol_SliderGrabActive] = ImColor_Blue;
	colors[ImGuiCol_Button] = colors[ImGuiCol_FrameBg];
	colors[ImGuiCol_ButtonHovered] = colors[ImGuiCol_FrameBgHovered];
	colors[ImGuiCol_ButtonActive] = colors[ImGuiCol_FrameBgActive];
	colors[ImGuiCol_Header] = ImVec4(0.313f, 0.313f, 0.313f, 1.000f);
	colors[ImGuiCol_HeaderHovered] = ImColor_Blue;
	colors[ImGuiCol_HeaderActive] = ImColor_Blue;
	colors[ImGuiCol_Separator] = colors[ImGuiCol_Border];
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.391f, 0.391f, 0.391f, 1.000f);
	colors[ImGuiCol_SeparatorActive] = ImColor_Blue;
	colors[ImGuiCol_ResizeGrip] = ImVec4(1.000f, 1.000f, 1.000f, 0.250f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(1.000f, 1.000f, 1.000f, 0.670f);
	colors[ImGuiCol_ResizeGripActive] = ImColor_Blue;
	colors[ImGuiCol_Tab] = colors[ImGuiCol_FrameBg];
	colors[ImGuiCol_TabHovered] = ImColor_Blue;
	colors[ImGuiCol_TabActive] = ImColor_Blue;
	colors[ImGuiCol_TabUnfocused] = colors[ImGuiCol_Tab];
	colors[ImGuiCol_TabUnfocusedActive] = colors[ImGuiCol_TabActive];
	colors[ImGuiCol_DockingPreview] = ImVec4(ImColor_Blue.x, ImColor_Blue.y, ImColor_Blue.z, 0.781f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.180f, 0.180f, 0.180f, 1.000f);
	colors[ImGuiCol_PlotLines] = ImVec4(0.469f, 0.469f, 0.469f, 1.000f);
	colors[ImGuiCol_PlotLinesHovered] = ImColor_Blue;
	colors[ImGuiCol_PlotHistogram] = ImVec4(0.586f, 0.586f, 0.586f, 1.000f);
	colors[ImGuiCol_PlotHistogramHovered] = ImColor_Blue;
	colors[ImGuiCol_TextSelectedBg] = ImVec4(1.000f, 1.000f, 1.000f, 0.156f);
	colors[ImGuiCol_DragDropTarget] = ImColor_Blue;
	colors[ImGuiCol_NavHighlight] = ImColor_Blue;
	colors[ImGuiCol_NavWindowingHighlight] = ImColor_Blue;
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.000f, 0.000f, 0.000f, 0.586f);

	style->ChildRounding = 4.0f;
	style->FrameBorderSize = 1.0f;
	style->FrameRounding = 2.0f;
	style->GrabMinSize = 7.0f;
	style->PopupRounding = 2.0f;
	style->ScrollbarRounding = 12.0f;
	style->ScrollbarSize = 13.0f;
	style->TabBorderSize = 1.0f;
	style->TabRounding = 0.0f;
	style->WindowRounding = 4.0f;
}

void Proto::Engine::Cleanup()
{
	// Game
	SafeDelete(m_pGame);
	
	CleanupEngineParts();
	CleanupSDL();
}

void Proto::Engine::CleanupSDL()
{
	SDL_DestroyWindow(m_Window);
	m_Window = nullptr;
	SDL_Quit();
}

void Proto::Engine::CleanupEngineParts()
{
	// ProtoContent
	ProtoContent.Destroy();
	
	// ProtoRenderer
	ProtoRenderer.Destroy();

	// ProtoInput
	ProtoInput.Destroy();

	// ProtoCommands
	ProtoCommands.Destroy();
}
