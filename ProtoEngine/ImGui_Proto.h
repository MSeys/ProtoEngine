#pragma once
#include "Structs.h"
#include <string>

#define ImColor_Blue ImVec4{ 0, 118.f / 255.f, 210.f / 255.f, 1.f }  // NOLINT(cppcoreguidelines-macro-usage)
namespace ImGuiProto
{
	void Position(TextureData& texData, const std::string& address);
	void Size(TextureData& texData, const std::string& address);
	void Alignment(HAlignment& hAlignment, VAlignment& vAlignment);
	void Color(const std::string& text, const TextureData& texData, SDL_Color& color, const std::string& address);

	// Derived from https://github.com/ocornut/imgui/issues/1496
	void BeginGroupPanel(const char* name, const ImVec2& size = ImVec2(-1.0f, -1.0f));
	void EndGroupPanel();

	void BeginComponentPanel(const ImVec2& size, BaseComponent* pCurrComp, BaseComponent** pDelComp, const std::string& address);
	void EndComponentPanel();
}