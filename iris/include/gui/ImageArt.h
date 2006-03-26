//
// File: ImageArt.h
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

#ifndef _IMAGEART_H_
#define _IMAGEART_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "SDL/SDL.h"
#include "Control.h"

// #include "../Fluid/mmgr.h"


class ImageArt : public Control
{
 public:
  ImageArt ();
  ImageArt (int x, int y, int texid);
  ImageArt (int x, int y, int texid, int flags);
  ~ImageArt ();
  void SetTexID (int texid);
  int GetTexID(void);
  void SetHue(int hue){m_hue = hue;}
  int hue(){return m_hue;}
  virtual void Draw(GumpHandler * gumps);

 private:
  int texid;
  int m_hue;
};


#endif
