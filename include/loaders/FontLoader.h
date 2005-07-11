/*****
 *
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
 *
 *****/

#if !defined( __FONTLOADER_H__ )
#define __FONTLOADER_H__

#include <map>
#include <string>

struct stChar
{
	unsigned char width;
	unsigned char height;
	unsigned char header; // Unknown
	unsigned char *redmask; // red mask pixel data (for hueing only)
	unsigned int *pixels; // Pixel Data
};

struct stFont
{
	unsigned char maxHeight;
	stChar chars[224]; // The first 32 characters are NOT included (control chars)
};

class cFontLoader
{
private:
	std::map< unsigned short, stFont > fonts;
public:
	cFontLoader ();
	virtual ~cFontLoader();

   void Init (std::string filename);
   void DeInit ();

  const stFont *getFont( unsigned short id ) const;
};

extern cFontLoader pFontLoader;

#endif
