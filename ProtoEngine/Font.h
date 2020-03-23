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
		explicit Font(const std::string& fullPath, unsigned int size);
		~Font();

		Font(const Font &) = delete;
		Font(Font &&) = delete;
		Font & operator= (const Font &) = delete;
		Font & operator= (const Font &&) = delete;

		std::string GetPath() const;
	private:
		std::string m_FullPath;
		_TTF_Font* m_Font;
	};
}
