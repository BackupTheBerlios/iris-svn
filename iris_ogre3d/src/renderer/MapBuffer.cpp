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
#include "robmap.h"




MapbufferHandler pMapbufferHandler;

Mapbuffer::Mapbuffer ()
{PROFILE
	m_roof_z = ROOF_NONE;
}

Mapbuffer::~Mapbuffer ()
{PROFILE
	Clear();
}

void Mapbuffer::Clear (void)
{PROFILE
	//cRobMap::GetSingleton().ClearCache();
	MapBuffer_t::iterator iter;

	for ( iter = root.begin (); iter != root.end (); iter++ )
	{
		delete (*iter).second;
	}

	root.clear();
}

/// called every frame, calls ALL mapblocks, not only visible ones...
void Mapbuffer::StepAll (void) {PROFILE
	MapBuffer_t::iterator iter;

	for ( iter = root.begin (); iter != root.end (); iter++ )
	{
		(*iter).second->Step();
	}
}

cMapblock *Mapbuffer::Get( int x, int y )
{PROFILE
	//cRobMap::GetSingleton().GetBlock(x,y);
	if ( ( x < 0 ) || ( y < 0 ) )
	{
		return NULL;
	}
	
	MapBuffer_t::iterator iter;

	iter = root.find( (Uint32) x << 16 | y );

	if ( iter == root.end() )
	{
		return NULL;
	}
	else
	{
		return (*iter).second;
	}
}

void Mapbuffer::Add (cMapblock * block)
{PROFILE
	
	//PROFILE_PRINT_STACKTRACE
	
  assert (block);

  Uint16 x = block->getBlockX ();
  Uint16 y = block->getBlockY ();

  root.insert (std::make_pair ((Uint32) x << 16 | y, block));
}

void Mapbuffer::FreeBuffer (int radius)
{PROFILE
	// obsolete
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

void Mapbuffer::SetUsageFlag (bool value)
{PROFILE
  MapBuffer_t::iterator iter;

  for (iter = root.begin (); iter != root.end (); iter++)
    iter->second->set_in_use (value);
}

void Mapbuffer::ResetFader (cFader * fader)
{PROFILE
  MapBuffer_t::iterator iter;

  for (iter = root.begin (); iter != root.end (); iter++)
    iter->second->ResetFader (fader);
}

void Mapbuffer::UpdateAlpha ()
{PROFILE
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

void Mapbuffer::ResetLight ()
{PROFILE
  MapBuffer_t::iterator iter;
  for (iter = root.begin (); iter != root.end (); iter++)
    iter->second->ResetLight ();
}


MapbufferHandler::MapbufferHandler ()
{PROFILE
    map_buffer = NULL;
}

MapbufferHandler::~MapbufferHandler ()
{PROFILE
    DeInit ();
}

void MapbufferHandler::Init (Mapbuffer * map_buffer)
{PROFILE
    DeInit ();
    this->map_buffer = map_buffer;
}

void MapbufferHandler::DeInit ()
{PROFILE
    delete map_buffer;
    map_buffer = NULL;
}

Mapbuffer *MapbufferHandler::buffer ()
{PROFILE
      if ( !map_buffer )
	  {
          THROWEXCEPTION ("Invalid Map buffer access");
	  }
      return map_buffer;
}
