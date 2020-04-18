#pragma once
#include <SDL.h>

struct TextureData
{
	float x = 0;
	float y = 0;
	float width = -1;
	float height = -1;
	SDL_Color color = { 255, 255, 255, 255 };
};

struct RenderData
{
	glm::vec2 position;
	glm::vec2 size;
	glm::vec2 rotationCenter;
	float angle;
	SDL_Color color;
};

enum class HAlignment
{
	LEFT, CENTER, RIGHT
};

enum class VAlignment
{
	TOP, CENTER, BOTTOM
};