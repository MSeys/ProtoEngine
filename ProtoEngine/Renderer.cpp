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
	const SDL_RendererFlip flip{ SDL_FLIP_NONE };

	glm::vec2 totalPosition{ data.originPosition + data.position };
	const glm::vec2 totalScale{ data.originScale * data.scale };

	SDL_SetTextureBlendMode(texture.GetSDLTexture(), SDL_BLENDMODE_BLEND);
	SDL_SetTextureColorMod(texture.GetSDLTexture(), data.color.r, data.color.g, data.color.b);
	SDL_SetTextureAlphaMod(texture.GetSDLTexture(), data.color.a);

	// Offset based on pivot (accounting for scale)
	totalPosition -= data.pivot * data.size * totalScale;

	SDL_Point rotPoint{ int(data.originPosition.x - totalPosition.x), int(data.originPosition.y - totalPosition.y) };
	CalculateRenderOffset(totalPosition.x, totalPosition.y);
	
	SDL_Rect dst;
	dst.x = int(totalPosition.x);
	dst.y = int(totalPosition.y);
	dst.w = int(data.size.x * totalScale.x);
	dst.h = int(data.size.y * totalScale.y);

	// flip using destination rect instead (+ small cheat to see flipped version)
	float angle{ data.originAngle };
	if (dst.w < 0 || dst.h < 0)
		angle += 0.00001f;
	
	SDL_RenderCopyEx(GetSDLRenderer(), texture.GetSDLTexture(), nullptr, &dst, angle, &rotPoint, flip);
}

void Proto::Renderer::RenderSprite(const Texture2D& texture, const FrameRenderData& data) const
{
	const SDL_RendererFlip flip{ SDL_FLIP_NONE };

	glm::vec2 totalPosition{ data.originPosition + data.frame.dstPosition };
	const glm::vec2 totalScale{ data.originScale * data.frame.scale };

	SDL_SetTextureBlendMode(texture.GetSDLTexture(), SDL_BLENDMODE_BLEND);
	SDL_SetTextureColorMod(texture.GetSDLTexture(), data.frame.color.r, data.frame.color.g, data.frame.color.b);
	SDL_SetTextureAlphaMod(texture.GetSDLTexture(), data.frame.color.a);

	// Offset based on pivot (accounting for scale)
	totalPosition -= data.frame.pivot * data.frame.dstSize * totalScale;

	SDL_Point rotPoint{ int(data.originPosition.x - totalPosition.x), int(data.originPosition.y - totalPosition.y) };
	CalculateRenderOffset(totalPosition.x, totalPosition.y);

	SDL_Rect dst;
	dst.x = int(totalPosition.x);
	dst.y = int(totalPosition.y);
	dst.w = int(data.frame.dstSize.x * totalScale.x);
	dst.h = int(data.frame.dstSize.y * totalScale.y);

	SDL_Rect src;
	src.x = int(data.frame.srcPosition.x);
	src.y = int(data.frame.srcPosition.y);
	src.w = int(data.frame.srcSize.x);
	src.h = int(data.frame.srcSize.y);

	// flip using destination rect instead (+ small cheat to see flipped version)
	float angle{ data.originAngle };
	if (dst.w < 0 || dst.h < 0)
		angle += 0.00001f;
	
	SDL_RenderCopyEx(GetSDLRenderer(), texture.GetSDLTexture(), &src, &dst, angle, &rotPoint, flip);
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
		SDL_SetRenderDrawColor(GetSDLRenderer(), 0, 0, 0, 255);
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
