#pragma once
struct SDL_Texture;
namespace Proto
{
	/**
	 * Simple RAII wrapper for an SDL_Texture
	 */
	class Texture2D
	{
	public:
		SDL_Texture* GetSDLTexture() const;
		explicit Texture2D(SDL_Texture* pTexture, std::string fullPath, std::string relativePath);
		~Texture2D();

		Texture2D(const Texture2D &) = delete;
		Texture2D(Texture2D &&) = delete;
		Texture2D & operator= (const Texture2D &) = delete;
		Texture2D & operator= (const Texture2D &&) = delete;

		std::string GetFullPath() const;
		std::string GetRelativePath() const;

	private:
		std::string m_FullPath;
		std::string m_RelativePath;
		SDL_Texture* m_pTexture;
	};
}
