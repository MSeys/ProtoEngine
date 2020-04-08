#include "ProtoEnginePCH.h"
#include "Renderer.h"
#include <SDL.h>
#include "Texture2D.h"

void Proto::Renderer::Init(SDL_Window * window)
{
	if(ProtoSettings.GetWindowSettings().FPSState == FPSState::PROTO_VSYNC)
		m_Renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	else
		m_Renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	
	if (!m_Renderer) 
	{
		throw std::runtime_error(std::string("SDL_CreateRenderer Error: ") + SDL_GetError());
	}

	ImGui::CreateContext();
	ImGuiSDL::Initialize(m_Renderer, int(ProtoSettings.GetWindowSize().x), int(ProtoSettings.GetWindowSize().y));
}

void Proto::Renderer::Destroy()
{
	if (m_Renderer != nullptr)
	{
		ImGuiSDL::Deinitialize();
		SDL_DestroyRenderer(m_Renderer);
		m_Renderer = nullptr;

		ImGui::DestroyContext();
	}
}

void Proto::Renderer::RenderTexture(const Texture2D& texture, const float x, const float y, const SDL_Color& color) const
{
	SDL_Rect dst;	
	dst.x = static_cast<int>(x);
	dst.y = static_cast<int>(y);

	if (ProtoSettings.GetRenderSettings().RenderMode == RenderMode::EDITOR)
	{
		dst.x += int(ProtoSettings.GetRenderSettings().GameRenderOffset.x);
		dst.y += int(ProtoSettings.GetRenderSettings().GameRenderOffset.y);
	}
	
	SDL_QueryTexture(texture.GetSDLTexture(), nullptr, nullptr, &dst.w, &dst.h);

	SDL_SetTextureColorMod(texture.GetSDLTexture(), color.r, color.g, color.b);
	SDL_SetTextureAlphaMod(texture.GetSDLTexture(), color.a);
	SDL_RenderCopy(GetSDLRenderer(), texture.GetSDLTexture(), nullptr, &dst);
}

void Proto::Renderer::RenderTexture(const Texture2D& texture, const float x, const float y, const float width, const float height, const SDL_Color& color) const
{
	SDL_Rect dst;
	dst.x = static_cast<int>(x);
	dst.y = static_cast<int>(y);

	if (ProtoSettings.GetRenderSettings().RenderMode == RenderMode::EDITOR)
	{
		dst.x += int(ProtoSettings.GetRenderSettings().GameRenderOffset.x);
		dst.y += int(ProtoSettings.GetRenderSettings().GameRenderOffset.y);
	}
	
	dst.w = static_cast<int>(width);
	dst.h = static_cast<int>(height);
	SDL_SetTextureColorMod(texture.GetSDLTexture(), color.r, color.g, color.b);
	SDL_RenderCopy(GetSDLRenderer(), texture.GetSDLTexture(), nullptr, &dst);
}
