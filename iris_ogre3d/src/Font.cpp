/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "Font.h"
#include "profile.h"





Font::Font( int iId, std::string sFileName, std::string sFontName, int iSize, int iHue ) : m_kFont( NULL ),
		m_iId( iId ), m_sFileName( sFileName ), m_sFontName( sFontName ), m_iSize( iSize ), m_iHue( iHue )
{ PROFILE
	m_kFont = TTF_OpenFont( m_sFileName.c_str(), m_iSize );

	if ( !m_kFont )
	{
		std::cerr << "TTF Load failed: " << SDL_GetError() << std::endl;
		return;
	}
}


Font::~Font()
{ PROFILE
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
