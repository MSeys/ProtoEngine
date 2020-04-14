#pragma once
#include "Singleton.h"
#include <unordered_map>

namespace Proto
{
	class Texture2D;
	class Font;
	
	class ContentManager final : public Singleton<ContentManager>
	{
	public:
		void Init(const std::string& data);
		void Destroy();
		
		Texture2D* GetTexture(const std::string& file);
		Font* GetFont(const std::string& file, unsigned int size);

		std::string GetDataPath() const;

	private:
		std::string m_DataPath;

		std::unordered_map<std::string, Texture2D*> m_pTextures;
		std::unordered_map<std::string, Font*> m_pFonts;
		
		void LoadTexture(const std::string& file);
		void LoadFont(const std::string& file, unsigned int size);
	};
}