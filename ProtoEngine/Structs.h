#pragma once
#include <SDL.h>

namespace Proto
{
	struct TextureData
	{
		float x = 0;
		float y = 0;
		float width = -1;
		float height = -1;
		SDL_Color color = { 255, 255, 255, 255 };
	};
}
