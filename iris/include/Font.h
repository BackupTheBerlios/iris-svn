#ifndef _FONT_H_
#define _FONT_H_

#include <iostream>
#include <string>
#include "SDL/SDL_ttf.h"

// #include "../Fluid/mmgr.h"

/*
 * Our Font information structure.
 */
struct FontInfo
{
	int iId;
	std::string sFileName;
	std::string sFontName;
	int iSize;
	int iHue;
};


class Font
{
public:
	Font( int iId, std::string sFileName, std::string sFontName, int iSize, int iHue );
	~Font();

	TTF_Font *GetFont() const;
	int GetId() const;

private:
	TTF_Font *m_kFont;
	int m_iId;
	std::string m_sFileName;
	std::string m_sFontName;
	int m_iSize;
	int m_iHue;
};

#endif
