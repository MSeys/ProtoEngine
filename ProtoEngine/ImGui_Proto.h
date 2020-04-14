#pragma once
#include "Structs.h"
#include <string>

namespace ImGuiProto
{
	void Position(TextureData& texData, const std::string& address);
	void Size(TextureData& texData, const std::string& address);
	void Alignment(HAlignment& hAlignment, VAlignment& vAlignment);
	void Color(const std::string& text, const TextureData& texData, SDL_Color& color, const std::string& address);
}