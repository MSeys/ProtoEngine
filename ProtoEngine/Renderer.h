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

		SDL_Renderer* GetSDLRenderer() const { return m_Renderer; }
	private:
		SDL_Renderer* m_Renderer{};
	};
}

