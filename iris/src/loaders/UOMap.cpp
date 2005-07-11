//
// File: UOMap.cpp
// Created by:  Alexander Oster - tensor@ultima-iris.de
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


#include "iris_endian.h"
#include <iostream>
#include "loaders/UOMap.h"
#include <fstream>
#include <cstring>
#include "Debug.h"
#include "Exception.h"
#include "Config.h"
#include <stdlib.h>             // doc: malloc and free
#include "loaders/MapInfo.h"

using namespace std;

MapLoader *pMapLoader = NULL;


UOMapLoader::UOMapLoader(char *mapfile, char *staticfile, char *staidx,
			 int type)
{
  mapstream = new ifstream(mapfile, ios::in | ios::binary);
  if(!mapstream->is_open()) {
     delete mapstream;
     mapstream = NULL;
     THROWEXCEPTION ("Could not load map file: " + string(mapfile));
  }

  staticstream = new ifstream(staticfile, ios::in | ios::binary);
  if(!staticstream->is_open()) {
    delete mapstream; mapstream = NULL;
    delete staticstream; staticstream = NULL;
    THROWEXCEPTION ("Could not load static file: " + string(staticfile));
  }

  staidxstream = new ifstream(staidx, ios::in | ios::binary);
  if(!staidxstream->is_open()) {
    delete mapstream; mapstream = NULL;
    delete staticstream; staticstream = NULL;
    delete staidxstream; staidxstream = NULL;
    THROWEXCEPTION ("Could not load static index file: " + string(staidx));
  }

  
     cMapInfoEntry * mapinfo_entry = pMapInfoLoader.GetMapInfo(type);
     if (!mapinfo_entry)
          THROWEXCEPTION ("Tried to load unknown map file");
     width = mapinfo_entry->width ();
     height = mapinfo_entry->height ();
     

}

UOMapLoader::~UOMapLoader ()
{
  mapstream->close ();
  delete mapstream;

  staticstream->close ();
  delete staticstream;

  staidxstream->close ();
  delete staidxstream;


}

void UOMapLoader::LoadMapBlock (int x, int y, MulBlock * block)
{
  if ((x >= width) || (y >= height) || (x < 0) || (y < 0))
      {
        memset ((char *) block, 0, sizeof (struct MulBlock));
        return;
      }

  mapstream->seekg (((x * height) + y) * 196 + 4, ios::beg);
  mapstream->read ((char *) block, 192);
  for (int ii = 0; ii < 64; ii++)
      {
        block->cells[ii / 8][ii % 8].tileid =
          IRIS_SwapU16 (block->cells[ii / 8][ii % 8].tileid);
      }
}

/*void UOMapLoader::LoadStaticIdx(int x, int y, stIndexRecord * block)
{
  if((x >= width) || (y >= height) || (x < 0) || (y < 0)) {
    memset((char *) block, 0, sizeof(stIndexRecord));
    return;
  }

  mutex_idx.MutexLock();

  staidxstream->seekg((x * height + y) * 12, ios::beg);
  staidxstream->read((char *) block, 12);

  mutex_idx.MutexUnlock();
}

void UOMapLoader::LoadStatics(int ofs, int len)
{
  mutex_statics.MutexLock();

  staticstream->seekg(ofs, ios::beg);
  staticstream->read((char *) statics, len);

  mutex_statics.MutexUnlock();
}*/

struct staticinfo *UOMapLoader::LoadStatics (int x, int y, int &len)
{
  stIndexRecord block;
  if ((x >= width) || (y >= height) || (x < 0) || (y < 0))
    return NULL;


  staidxstream->seekg ((x * height + y) * 12, ios::beg);
  staidxstream->read ((char *) &block, 12);
  block.offset = IRIS_SwapU32 (block.offset);
  block.length = IRIS_SwapU32 (block.length);
  block.extra = IRIS_SwapU32 (block.extra);

  if (block.length == 0xffffffff)
    return NULL;

  len = block.length / 7;

  struct staticentry *uostatics =
    (struct staticentry *) malloc (block.length);
  struct staticinfo *static_infos =
    (struct staticinfo *) malloc (len * sizeof (struct staticinfo));


  staticstream->seekg (block.offset, ios::beg);
  staticstream->read ((char *) uostatics, block.length);


  struct staticentry *uo_p = uostatics;
  struct staticinfo *info_p = static_infos;

  for (int index = 0; index < len; index++)
      {
        info_p->x = uo_p->x;
        info_p->y = uo_p->y;
        info_p->z = uo_p->z;
        info_p->TileID = IRIS_SwapU16 (uo_p->TileID);
        info_p->Hue = IRIS_SwapI16 (uo_p->Hue);
        info_p->obj_id = index;
        info_p++;
        uo_p++;
      }


  free (uostatics);

  return static_infos;
}


