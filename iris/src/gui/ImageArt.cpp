//
// File: ImageArt.cpp
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

#include "loaders/ArtLoader.h"
#include "gui/ImageArt.h"
#include "Logger.h"
#include "Config.h"
#include "renderer/TextureBuffer.h"

using namespace std;

ImageArt::ImageArt ()
{
  Control::Control ();
  texid = 0;
  m_hue = 0;
}

ImageArt::ImageArt (int x, int y, int texid)
{
  Control::Control ();
  SetTexID (texid);
  SetPosition (x, y);
  control_type = CONTROLTYPE_IMAGEART;
  m_hue = 0;
}

ImageArt::ImageArt (int x, int y, int texid, int flags)
{
  Control::Control ();
  SetTexID (texid);
  SetPosition (x, y);
  SetFlags (flags);
  control_type = CONTROLTYPE_IMAGEART;
}

ImageArt::~ImageArt ()
{
}

void ImageArt::SetTexID (int texid)
{
  this->texid = texid;
}

int ImageArt::GetTexID (void)
{
  return texid;
}

void ImageArt::Draw (GumpHandler * gumps)
{

  Control::Draw (gumps);


        Texture *texture;

        //if (texid >= 16384) 

        //    texture = pTextureBuffer->GetArtTexture(texid);
        //texture = pArtLoader->LoadArt(texid + 16384, true, false, 0);
//    else

        //    texture = pTextureBuffer->GetGroundTexture(texid);

        texture = pTextureBuffer.GetArtTexture (texid + 16384);

        if (texture)
            {
//Logger::WriteLine("TEXTURE OK");
              if ((!GetWidth ()) || (!GetHeight ()))
                  {             //Logger::WriteLine("TEXTURE RESIZE OK");
                    SetSize (texture->GetRealWidth (),
                             texture->GetRealHeight ());
                  }
              //DrawRect(10, 10, texture->GetWidth(), texture->GetHeight(), texture, 250, true);
              //DrawRect(GetX(), GetY(), GetWidth(), GetHeight(), texture, GetAlpha(), true);
              if (m_hue != 0)
                DrawRectHued (GetX (), GetY (), texture->GetWidth (),
                              texture->GetHeight (), texture, m_hue,
                              GetAlpha (), true);
              else
                DrawRect (GetX (), GetY (), texture->GetWidth (),
                          texture->GetHeight (), texture, GetAlpha (), true);
            }

}
