#pragma once
#include <SDL.h>

struct TextureFrame
{
	glm::vec2 dstPosition;
	glm::vec2 dstSize;

	glm::vec2 srcPosition;
	glm::vec2 srcSize;

	glm::vec2 scale;
	glm::vec2 pivot;

	SDL_Color color;

	void Save(rapidxml::xml_document<>& doc, rapidxml::xml_node<>* pParent);
	static TextureFrame Load(rapidxml::xml_node<>* pComp);
};

struct RenderData
{
	glm::vec2 originPosition;
	glm::vec2 originScale;
	float originAngle;
	
	glm::vec2 position;
	glm::vec2 scale;
	glm::vec2 pivot;
	
	glm::vec2 size;
	SDL_Color color;
};

struct FrameRenderData
{
	glm::vec2 originPosition;
	glm::vec2 originScale;
	float originAngle;

	TextureFrame frame;
};