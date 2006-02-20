//
// File: MapBuffer.cpp
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


#include "renderer/MapBuffer.h"
#include <assert.h>
#include "Logger.h"
#include "Config.h"
#include "Exception.h"
#include "include.h"
#include "renderer/Camera.h"

using namespace std;



cMapbufferHandler pMapbufferHandler;

cMapbuffer::cMapbuffer ()
{
  m_roof_z = ROOF_NONE;
}

cMapbuffer::~cMapbuffer ()
{
  Clear ();
}

void cMapbuffer::Clear (void)
{
  MapBuffer_t::iterator iter;

  for (iter = root.begin (); iter != root.end (); iter++)
    delete (*iter).second;

  root.clear ();
}

cMapblock *cMapbuffer::Get (int x, int y)
{
  if ((x < 0) || (y < 0))
    return NULL;

  MapBuffer_t::iterator iter;

  iter = root.find ((Uint32) x << 16 | y);

  if (iter == root.end ())
    return NULL;
  else
    return (*iter).second;
}

void cMapbuffer::Add (cMapblock * block)
{
  assert (block);

  Uint16 x = block->getBlockX ();
  Uint16 y = block->getBlockY ();

  root.insert (make_pair ((Uint32) x << 16 | y, block));
}

void cMapbuffer::FreeBuffer (int radius)
{
  MapBuffer_t::iterator iter;

  int blockx, blocky;
  blockx = pCamera.GetBlockX ();
  blocky = pCamera.GetBlockY ();

  for (iter = root.begin (); iter != root.end (); iter++)
      {
        if ((iter->second->getBlockX () < blockx - radius) ||
            (iter->second->getBlockX () > blockx + radius) ||
            (iter->second->getBlockY () < blocky - radius) ||
            (iter->second->getBlockY () > blocky + radius))
            {
              delete iter->second;
              root.erase (iter);
              return;
            }
      }

}

void cMapbuffer::SetUsageFlag (bool value)
{
  MapBuffer_t::iterator iter;

  for (iter = root.begin (); iter != root.end (); iter++)
    iter->second->set_in_use (value);
}

void cMapbuffer::ResetFader (cFader * fader)
{
  MapBuffer_t::iterator iter;

  for (iter = root.begin (); iter != root.end (); iter++)
    iter->second->ResetFader (fader);
}

void cMapbuffer::UpdateAlpha ()
{
  MapBuffer_t::iterator iter;
  for (iter = root.begin (); iter != root.end (); iter++)
    if (!iter->second->in_use ())
        {
          iter->second->SetAlpha (-255, 255, false);
          if (m_roof_z != ROOF_NONE)

            iter->second->SetAlpha (m_roof_z, Config::GetRoofFadeAlpha(),
                                    false);
        }
}

void cMapbuffer::ResetLight ()
{
  MapBuffer_t::iterator iter;
  for (iter = root.begin (); iter != root.end (); iter++)
    iter->second->ResetLight ();
}


cMapbufferHandler::cMapbufferHandler ()
{
    map_buffer = NULL;
}

cMapbufferHandler::~cMapbufferHandler ()
{
    DeInit ();
}

void cMapbufferHandler::Init (cMapbuffer * map_buffer)
{
    DeInit ();
    this->map_buffer = map_buffer;
}

void cMapbufferHandler::DeInit ()
{
    delete map_buffer;
    map_buffer = NULL;
}

cMapbuffer * cMapbufferHandler::buffer ()
{
      if (!map_buffer)
          THROWEXCEPTION ("Invalid Map buffer access");
      return map_buffer;
}
