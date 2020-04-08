#pragma once
#include <SDL.h>

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

		void RenderTexture(const Texture2D& texture, float x, float y, const SDL_Color& color = { 255, 255, 255, 255 }) const;
		void RenderTexture(const Texture2D& texture, float x, float y, float width, float height, const SDL_Color& color = { 255, 255, 255, 255 }) const;

		SDL_Renderer* GetSDLRenderer() const { return m_Renderer; }
	private:
		SDL_Renderer* m_Renderer{};
	};
}

