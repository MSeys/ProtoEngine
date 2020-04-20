#include "ProtoEnginePCH.h"
#include "InputManager.h"
#include <SDL.h>

#include "Buttons.h"

void Proto::InputManager::Init()
{
	m_ControllerHandler.Init();
	m_KBMHandler.Init();
	InitImGuiIO();
}

void Proto::InputManager::Destroy()
{
	m_ControllerHandler.Destroy();
	m_KBMHandler.Destroy();
}

void Proto::InputManager::Update()
{
	ImGuiIO& io = ImGui::GetIO();
	
	if (!io.WantCaptureKeyboard && !io.WantCaptureMouse && !io.WantTextInput)
	{
		if(ProtoSettings.GetRenderMode() == RenderMode::GAME || ProtoSettings.GetEditorMode() == EditorMode::PLAY)
			m_ControllerHandler.Update();
	}

	m_KBMHandler.UpdateOutPoll();

	int wheel = 0;
	
	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		if (e.type == SDL_QUIT)
		{
			ProtoCommands.ForceExit();
			break;
		}

		if (e.type == SDL_WINDOWEVENT)
		{
			// Based on https://github.com/Tyyppi77/imgui_sdl/blob/master/example.cpp#L38
			if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
			{
				io.DisplaySize.x = static_cast<float>(e.window.data1);
				io.DisplaySize.y = static_cast<float>(e.window.data2);
			}
		}

		else if (e.type == SDL_MOUSEWHEEL)
		{
			// Based on https://github.com/Tyyppi77/imgui_sdl/blob/master/example.cpp#L46
			wheel = e.wheel.y;
		}

		else if(e.type == SDL_TEXTINPUT)
		{
			// Based on https://github.com/ocornut/imgui/blob/master/examples/imgui_impl_sdl.cpp#L107
			io.AddInputCharactersUTF8(e.text.text);
		}

		else if(e.type == SDL_KEYDOWN || e.type == SDL_KEYUP)
		{
			// Based on https://github.com/ocornut/imgui/blob/master/examples/imgui_impl_sdl.cpp#L92
			const int key = e.key.keysym.scancode;
			IM_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));
			io.KeysDown[key] = (e.type == SDL_KEYDOWN);
			io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
			io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
			io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);

			if(ProtoSettings.GetRenderMode() == RenderMode::EDITOR && ProtoSettings.GetEditorMode() == EditorMode::EDIT)
			{
				if (key == SDL_SCANCODE_ESCAPE)
					ProtoCommands.ForceExit();
			}

		}

		if (!io.WantCaptureKeyboard && !io.WantCaptureMouse && !io.WantTextInput)
		{
			if (ProtoSettings.GetRenderMode() == RenderMode::EDITOR && ProtoSettings.GetEditorMode() == EditorMode::EDIT)
			{
				if (e.type == SDL_MOUSEMOTION && e.motion.state & SDL_BUTTON_MMASK)
					ProtoSettings.TranslateEditorCamera(float(e.motion.xrel), float(e.motion.yrel));
			}
			
			if (ProtoSettings.GetRenderMode() == RenderMode::GAME || ProtoSettings.GetEditorMode() == EditorMode::PLAY)
				m_KBMHandler.UpdateInPoll(e);
		}
	}

	int mouseX, mouseY;
	const int buttons = SDL_GetMouseState(&mouseX, &mouseY);

	// Based on https://github.com/Tyyppi77/imgui_sdl/blob/master/example.cpp#L55
	// Setup low-level inputs (e.g. on Win32, GetKeyboardState(), or write to those fields from your Windows message loop handlers, etc.)
	io.DeltaTime = 1.0f / 60.0f;
	io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
	io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
	io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
	io.MouseWheel = static_cast<float>(wheel);
}

void Proto::InputManager::Process()
{
	m_ControllerHandler.Process();
	m_KBMHandler.Process();
}

bool Proto::InputManager::AddControllerButton(int index, int XInput, const std::string& stringifiedXInput)
{
	return m_ControllerHandler.AddInput(index, XInput, stringifiedXInput);
}

bool Proto::InputManager::AddKey(int SDLKey, const std::string& stringifiedSDLKey)
{
	return m_KBMHandler.AddInput(SDLKey, stringifiedSDLKey);
}

ControllerButton& Proto::InputManager::GetControllerButton(int index, int XInput)
{
	return m_ControllerHandler.GetInput(index, XInput);
}

ControllerJoystick& Proto::InputManager::GetControllerJoystick(int index, const StickState& stickState) const noexcept
{
	return m_ControllerHandler.GetInput(index, stickState);
}

ControllerTrigger& Proto::InputManager::GetControllerTrigger(int index, const TriggerPosState& triggerPosState) const noexcept
{
	return m_ControllerHandler.GetInput(index, triggerPosState);
}

Key& Proto::InputManager::GetKey(int SDLKey, bool doesCreate)
{
	return m_KBMHandler.GetInput(SDLKey, doesCreate);
}

MouseButton& Proto::InputManager::GetMouseButton(const StickState& mouseState) const
{
	return m_KBMHandler.GetInput(mouseState);
}

Mouse& Proto::InputManager::GetMouse() const
{
	return m_KBMHandler.GetInput();
}

void Proto::InputManager::InitImGuiIO()
{
	// Based on https://github.com/ocornut/imgui/blob/master/examples/imgui_impl_sdl.cpp#L140	
	ImGuiIO& io = ImGui::GetIO();
	
	io.KeyMap[ImGuiKey_Tab] = SDL_SCANCODE_TAB;
	io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
	io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
	io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
	io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
	io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
	io.KeyMap[ImGuiKey_Insert] = SDL_SCANCODE_INSERT;
	io.KeyMap[ImGuiKey_Delete] = SDL_SCANCODE_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = SDL_SCANCODE_BACKSPACE;
	io.KeyMap[ImGuiKey_Space] = SDL_SCANCODE_SPACE;
	io.KeyMap[ImGuiKey_Enter] = SDL_SCANCODE_RETURN;
	io.KeyMap[ImGuiKey_Escape] = SDL_SCANCODE_ESCAPE;
	io.KeyMap[ImGuiKey_KeyPadEnter] = SDL_SCANCODE_KP_ENTER;
	io.KeyMap[ImGuiKey_A] = SDL_SCANCODE_A;
	io.KeyMap[ImGuiKey_C] = SDL_SCANCODE_C;
	io.KeyMap[ImGuiKey_V] = SDL_SCANCODE_V;
	io.KeyMap[ImGuiKey_X] = SDL_SCANCODE_X;
	io.KeyMap[ImGuiKey_Y] = SDL_SCANCODE_Y;
	io.KeyMap[ImGuiKey_Z] = SDL_SCANCODE_Z;
}
