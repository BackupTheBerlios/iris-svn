//
// File: MapInfo.h
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

#ifndef _MAPINFO_
#define _MAPINFO_

#ifdef WIN32
#include <windows.h>
#endif

#include <string>
#include <map>

//#include "../Fluid/mmgr.h"

class cMapInfoEntry
{
    private:
        int m_id;
        int m_width;
        int m_height;
        std::string m_name;
        std::string m_skybox;
        int m_base_id;
        int m_fog_r;
        int m_fog_g;
        int m_fog_b;
    public:
       int id ();
       int width ();
       int height ();
       int base_id ();
       std::string name ();
       std::string skybox ();
       
       int fog_r();
       int fog_g();
       int fog_b();
       
       cMapInfoEntry (int id, int width, int height, std::string name, std::string skybox, int base_id, int r, int g, int b);
};

class cMapInfoLoader
{
 private:
     std::map<int, cMapInfoEntry*> maps;
 public:
  cMapInfoLoader();
  ~cMapInfoLoader();

  void Init (std::string filename);
  void DeInit ();

  cMapInfoEntry * GetMapInfo(int id);   
};

extern cMapInfoLoader pMapInfoLoader;

#endif
