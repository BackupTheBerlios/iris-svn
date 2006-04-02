#include "Font.h"


Font::Font( int iId, std::string sFileName, std::string sFontName, int iSize, int iHue ) : m_kFont( NULL ),
		m_iId( iId ), m_sFileName( sFileName ), m_sFontName( sFontName ), m_iSize( iSize ), m_iHue( iHue )
{
	m_kFont = TTF_OpenFont( m_sFileName.c_str(), m_iSize );

	if ( !m_kFont )
	{
		std::cerr << "TTF Load failed: " << SDL_GetError() << std::endl;
		return;
	}
}


Font::~Font()
{
	TTF_CloseFont( m_kFont );
}


TTF_Font *Font::GetFont() const
{
	return m_kFont;
}


int Font::GetId() const
{
	return m_iId;
}
