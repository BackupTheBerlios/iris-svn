//
// File: Image.cpp
// Created by: Alexander Oster - tensor@ultima-iris.de
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

#include "gui/Image.h"
#include "loaders/GumpLoader.h"
#include "Logger.h"
#include "Config.h"
#include "ogrewrapper.h"
#include <Ogre.h>
#include <OgreOverlay.h>
#include <OgreOverlayManager.h>
#include <OgrePanelOverlayElement.h>
#include <OgreTextAreaOverlayElement.h>
#include <cassert>




using namespace std;

Image::Image () : Control() {PROFILE
  gump = 0;
  tiled = false;
  tiled_w = 0;
  tiled_h = 0;
}

Image::Image (int x, int y, int gump) : Control() {PROFILE
  SetGump (gump);
  SetPosition (x, y);
  tiled = false;
  tiled_w = 0;
  tiled_h = 0;
  tiled_tex = NULL;
}

Image::Image (int x, int y, int gump, int flags) : Control() {PROFILE
  SetGump (gump);
  SetPosition (x, y);
  SetFlags (flags);
  tiled = false;
  tiled_w = 0;
  tiled_h = 0;
  tiled_tex = NULL;
}

Image::~Image ()
{PROFILE
  delete tiled_tex;
  tiled_tex = NULL;
}

void Image::SetGump (int gump)
{PROFILE
  this->gump = gump;
}

int Image::GetGump (void)
{PROFILE
  return gump;
}


void Image::CreateGFX		(GumpHandler *gumps) { PROFILE
	if (0) printf("Image::CreateGFX\n"); 
	if (mlRect.size() > 0) return;
	mlRect.push_back(Control::CreateRect());
}

void Image::DestroyGFX		() { PROFILE Control::DestroyGFX(); }

void Image::DrawStep 		(GumpHandler * gumps) { PROFILE
	if (mbNeedGfxUpdate) {
		Texture *texture;
		if (!tiled)
				texture = LoadGump (gump, gumps, false);
		else	texture = tiled_tex;
		if (texture) {
			if ((!GetWidth ()) || (!GetHeight ()))
				SetSize (texture->GetRealWidth (), texture->GetRealHeight ());
			UpdateRect(mlRect[0],GetX(),GetY(),GetWidth(),GetHeight(),texture,GetAlpha());
		} else { 
			mlRect[0]->hide();
		}
	}
}


void Image::Draw (GumpHandler * gumps)
{PROFILE
	printf("Image::Draw\n");
  Control::Draw (gumps);
  Texture *texture;

  if (!tiled)
    texture = LoadGump (gump, gumps, false);
  else
    texture = tiled_tex;

  if (texture)
      {
        if ((!GetWidth ()) || (!GetHeight ()))
          SetSize (texture->GetRealWidth (), texture->GetRealHeight ());
        DrawRect (GetX (), GetY (), GetWidth (), GetHeight (), texture,
                  GetAlpha ());
      }
}


bool Image::CheckPixel (int x, int y)
{PROFILE

  Texture *texture = NULL;

  texture = LoadGump (gump, &pGumpHandler, false);

  if (texture)
    return texture->CheckPixel (x, y);

  return true;
}

void Image::SetTiled (int width, int height)
{PROFILE
  delete tiled_tex;
  tiled_tex = NULL;
  
  tiled = true;
  tiled_w = width;
  tiled_h = height;
  tiled_tex = pGumpLoader.LoadGumpTiled (gump, tiled_w, tiled_h);
}
