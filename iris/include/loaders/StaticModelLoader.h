//
// File: StaticModelLoader.h
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

#ifndef _STATICMODELLOADER_H_
#define _STATICMODELLOADER_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "irisgl.h"

#include <map>
#include <string>

#include "SDL/SDL.h"
#include "uotype.h"

#include <iostream>
#include <fstream>

#include "StaticModels.h"
#include "StaticTextureLoader.h"

//#include "../Fluid/mmgr.h"

struct sModelTableEntry {
  Uint32 id;
  Uint32 start;
  Uint32 length;
  Uint8 md5sum[16];
} STRUCT_PACKED;


class cStaticModelLoader
{
private:
    std::map <Uint32, sModelTableEntry> model_entries;
    std::map <Uint32, cStaticModel *> models;
    
    std::ifstream * modelstream;
    Uint32 modelstream_size;

    cStaticTextureLoader static_texture_loader;
    
public:
    cStaticModelLoader ();
   ~cStaticModelLoader ();

   void Init (std::string filename);
   void DeInit ();
   
   cStaticModel * getModel (Uint32 id);
   Texture * GetGroundTexture (Uint32 id); // Get replaced ground texture

protected:
};

extern	cStaticModelLoader pStaticModelLoader;

#endif //_STATICMODELLOADER_H_
