#pragma once
#include <SDL.h>
#include "Structs.h"
#include "Singleton.h"

struct SDL_Window;
struct SDL_Renderer;

namespace Proto
{
	class Texture2D;
	/**
	 * Simple RAII wrapper for the SDL renderer
	 */
	class Renderer final : public Singleton<Renderer>
	{
	public:
		void Init(SDL_Window* window);
		void Destroy();

		void RenderTexture(const Texture2D& texture, const RenderData& data) const;
		void RenderSprite(const Texture2D& texture, const FrameRenderData& data) const;
		void RenderLineRect(const SDL_Rect& rect, const SDL_Color& color) const;
		void RenderLinePolygon(const std::vector<glm::vec2>& polygon, const SDL_Color& color) const;
		void RenderLineCircle(const glm::vec2& center, float radius, const SDL_Color& color) const;
		void RenderFilledCircle(const glm::vec2& center, float radius, const SDL_Color& color) const;
		void RenderLine(const glm::vec2& pointA, const glm::vec2& pointB, const SDL_Color& color) const;
		
		SDL_Renderer* GetSDLRenderer() const { return m_Renderer; }
		
	private:
		SDL_Renderer* m_Renderer{};

		void CalculateRenderOffset(int& x, int& y) const;
		void CalculateRenderOffset(float& x, float& y) const;
		void SetPixel(int x, int y, const SDL_Color& color) const;

		// For Rendering of Circles, I've looked at https://gist.github.com/derofim/912cfc9161269336f722
		// Slightly modified to work with my standard
	};
}

