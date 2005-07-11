//
// File: Border.h
// Created by:
//
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

#ifndef _BORDER_H_
#define _BORDER_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "SDL/SDL.h"
#include "Control.h"

class Texture;

class Border : public Control
{
public:
	Border();
	Border( int x, int y, unsigned short gump );
	Border( int x, int y, unsigned short gump, int flags );
	virtual ~Border();
  
	void setGump( unsigned short data ) { generated = false; _gump = data; }
	unsigned short gump() const { return _gump; }

	virtual void Draw( GumpHandler *gumps );

private:
	unsigned short _gump;
	Texture *textures[9];
	bool generated;
	void generateTextures();
};


#endif
