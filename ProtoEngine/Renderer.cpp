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

void Proto::Renderer::RenderTexture(const Texture2D& texture, const RenderData& data) const
{
	SDL_Rect dst;
	dst.x = static_cast<int>(data.position.x);
	dst.y = static_cast<int>(data.position.y);

	CalculateRenderOffset(dst.x, dst.y);

	dst.w = static_cast<int>(abs(data.size.x));
	dst.h = static_cast<int>(abs(data.size.y));

	SDL_Point rotPoint{ int(data.rotationCenter.x - data.position.x), int(data.rotationCenter.y - data.position.y) };
	SDL_RendererFlip flip{ SDL_FLIP_NONE };

	if (data.size.x < 0)
		flip = SDL_FLIP_HORIZONTAL;

	if (data.size.y < 0)
		flip = SDL_FLIP_VERTICAL;

	if (data.size.x < 0 && data.size.y < 0)
		flip = static_cast<SDL_RendererFlip>(SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
	
	SDL_SetTextureBlendMode(texture.GetSDLTexture(), SDL_BLENDMODE_BLEND);
	SDL_SetTextureColorMod(texture.GetSDLTexture(), data.color.r, data.color.g, data.color.b);
	SDL_SetTextureAlphaMod(texture.GetSDLTexture(), data.color.a);
	
	SDL_RenderCopyEx(GetSDLRenderer(), texture.GetSDLTexture(), nullptr, &dst, data.angle, &rotPoint, flip);
}

void Proto::Renderer::CalculateRenderOffset(int& x, int& y) const
{
	if (ProtoSettings.GetRenderMode() == RenderMode::EDITOR)
	{
		if (ProtoSettings.GetEditorRenderMode() == RenderMode::EDITOR)
		{
			x += int(ProtoSettings.GetEditorCamera().x);
			y += int(ProtoSettings.GetEditorCamera().y);
		}

		else
		{
			x -= int(ProtoScenes.GetActiveScene()->GetActiveCamera().x);
			y -= int(ProtoScenes.GetActiveScene()->GetActiveCamera().y);
		}
	}

	else
	{
		x -= int(ProtoScenes.GetActiveScene()->GetActiveCamera().x);
		y -= int(ProtoScenes.GetActiveScene()->GetActiveCamera().y);
	}
}
