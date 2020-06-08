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

void Proto::Renderer::RenderLineRect(const SDL_Rect& rect, const SDL_Color& color) const
{
	SDL_Rect dst{ rect };
	CalculateRenderOffset(dst.x, dst.y);
	
	SDL_SetRenderDrawColor(GetSDLRenderer(), color.r, color.g, color.b, color.a);
	SDL_RenderDrawRect(GetSDLRenderer(), &dst);
	SDL_SetRenderDrawColor(GetSDLRenderer(), 0, 0, 0, 255);
}

void Proto::Renderer::RenderLinePolygon(const std::vector<glm::vec2>& polygon, const SDL_Color& color) const
{
	for (size_t i{}; i < polygon.size(); i++)
	{
		if(i + 1 == polygon.size())
			RenderLine(polygon[i], polygon[0], color);

		else
			RenderLine(polygon[i], polygon[i + 1], color);
	}
}

void Proto::Renderer::RenderLineCircle(const glm::vec2& center, float radius, const SDL_Color& color) const
{
	// if the first pixel in the screen is represented by (0,0) (which is in sdl)
	// remember that the beginning of the circle is not in the middle of the pixel
	// but to the left-top from it:

	auto error = double(-radius);
	auto x = double(radius) - 0.5;
	auto y = double(0.5);
	float cx = float(center.x) - 0.5f;
	float cy = float(center.y) - 0.5f;

	CalculateRenderOffset(cx, cy);

	while (x >= y)
	{
		SetPixel(int(cx + x), int(cy + y), color);
		SetPixel(int(cx + y), int(cy + x), color);

		if (x != 0)
		{
			SetPixel(int(cx - x), int(cy + y), color);
			SetPixel(int(cx + y), int(cy - x), color);
		}

		if (y != 0)
		{
			SetPixel(int(cx + x), int(cy - y), color);
			SetPixel(int(cx - y), int(cy + x), color);
		}

		if (x != 0 && y != 0)
		{
			SetPixel(int(cx - x), int(cy - y), color);
			SetPixel(int(cx - y), int(cy - x), color);
		}

		error += y;
		++y;
		error += y;

		if (error >= 0)
		{
			--x;
			error -= x;
			error -= x;
		}
	}
}

void Proto::Renderer::RenderFilledCircle(const glm::vec2& center, float radius, const SDL_Color& color) const
{
	for (double dy = 1; dy <= radius; dy += 1.0)
	{
		int cx{ int(center.x) }, cy{ int(center.y) };

		CalculateRenderOffset(cx, cy);
		
		const double dx = floor(sqrt(2.0 * radius * dy - dy * dy));
		SDL_SetRenderDrawColor(GetSDLRenderer(), color.r, color.g, color.b, color.a);
		SDL_RenderDrawLine(GetSDLRenderer(), int(cx - dx), int(cy + dy - radius), int(cx + dx), int(cy + dy - radius));
		SDL_RenderDrawLine(GetSDLRenderer(), int(cx - dx), int(cy - dy + radius), int(cx + dx), int(cy - dy + radius));
	}
}

void Proto::Renderer::RenderLine(const glm::vec2& pointA, const glm::vec2& pointB, const SDL_Color& color) const
{
	glm::vec2 p1{ pointA }, p2{ pointB };
	CalculateRenderOffset(p1.x, p1.y);
	CalculateRenderOffset(p2.x, p2.y);
	
	SDL_SetRenderDrawColor(GetSDLRenderer(), color.r, color.g, color.b, color.a);
	SDL_RenderDrawLine(GetSDLRenderer(), int(p1.x), int(p1.y), int(p2.x), int(p2.y));
	SDL_SetRenderDrawColor(GetSDLRenderer(), 0, 0, 0, 255);
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
			x -= int(ProtoScenes.GetCurrentScene()->GetActiveCamera().x);
			y -= int(ProtoScenes.GetCurrentScene()->GetActiveCamera().y);
		}
	}

	else
	{
		x -= int(ProtoScenes.GetCurrentScene()->GetActiveCamera().x);
		y -= int(ProtoScenes.GetCurrentScene()->GetActiveCamera().y);
	}
}

void Proto::Renderer::CalculateRenderOffset(float& x, float& y) const
{
	if (ProtoSettings.GetRenderMode() == RenderMode::EDITOR)
	{
		if (ProtoSettings.GetEditorRenderMode() == RenderMode::EDITOR)
		{
			x += float(ProtoSettings.GetEditorCamera().x);
			y += float(ProtoSettings.GetEditorCamera().y);
		}

		else
		{
			x -= float(ProtoScenes.GetCurrentScene()->GetActiveCamera().x);
			y -= float(ProtoScenes.GetCurrentScene()->GetActiveCamera().y);
		}
	}

	else
	{
		x -= float(ProtoScenes.GetCurrentScene()->GetActiveCamera().x);
		y -= float(ProtoScenes.GetCurrentScene()->GetActiveCamera().y);
	}
}

void Proto::Renderer::SetPixel(int x, int y, const SDL_Color& color) const
{
	SDL_SetRenderDrawColor(GetSDLRenderer(), color.r, color.g, color.b, color.a);
	SDL_RenderDrawPoint(GetSDLRenderer(), x, y);
	SDL_SetRenderDrawColor(GetSDLRenderer(), 0, 0, 0, 255);
}
