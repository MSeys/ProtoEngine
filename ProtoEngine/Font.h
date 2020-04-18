#pragma once
#include <string>

struct _TTF_Font;
namespace Proto
{
	/**
	 * Simple RAII wrapper for an _TTF_Font
	 */
	class Font
	{
	public:
		_TTF_Font* GetFont() const;
		explicit Font(const std::string& fullPath, std::string relativePath, unsigned int size);
		~Font();

		Font(const Font &) = delete;
		Font(Font &&) = delete;
		Font & operator= (const Font &) = delete;
		Font & operator= (const Font &&) = delete;

		std::string GetFullPath() const;
		std::string GetRelativePath() const;
		unsigned int& GetSize();
		
	private:
		std::string m_FullPath;
		std::string m_RelativePath;
		_TTF_Font* m_Font;
		unsigned int m_Size;
	};
}
