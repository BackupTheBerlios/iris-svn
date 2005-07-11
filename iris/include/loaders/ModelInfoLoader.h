//
// File: ModelInfoLoader.h
// Created by: ArT-iX ArT-iX@libero.it
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

#ifndef _MODELINFO_LOADER_
#define _MODELINFO_LOADER_

#ifdef WIN32
#include <windows.h>
#endif

#include "SDL/SDL.h"
#include <string>
#include <map>

struct cModelInfoEntry
{
 int id;
 int scalex;
 int scaley;
 int scalez;
 int alpha;
 int defhue;
 int alt_body;
};

class cModelInfoLoader
{
 private:
     std::map<int, cModelInfoEntry*> model_infos;
 public:
  cModelInfoLoader();
  ~cModelInfoLoader();     
  cModelInfoEntry * GetModelEntry(int id);   
};

extern cModelInfoLoader * pModelInfoLoader;

#endif
