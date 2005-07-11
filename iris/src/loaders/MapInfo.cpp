//
// File: MapInfo.cpp
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
 

#include "loaders/MapInfo.h"

#include "Config.h"
#include "Debug.h"
#include <iostream>
#include "xml.h"

#include "Exception.h"

using namespace std;



cMapInfoLoader pMapInfoLoader;

cMapInfoEntry::cMapInfoEntry (int id, int width, int height, std::string name, std::string skybox, int base_id)
{
      m_id = id;
      m_width = width;
      m_height = height;
      m_name = name;
      m_skybox = skybox;
      m_base_id = base_id;
}

int cMapInfoEntry::id ()
{
    return m_id;
}

int cMapInfoEntry::width ()
{
    return m_width;
}

int cMapInfoEntry::height ()
{
    return m_height;
}

int cMapInfoEntry::base_id ()
{
    return m_base_id;
}

std::string cMapInfoEntry::name ()
{
    return m_name;
}

std::string cMapInfoEntry::skybox ()
{
    return m_skybox;
}


cMapInfoLoader::cMapInfoLoader()
{
}
  
cMapInfoLoader::~cMapInfoLoader()
{
    DeInit ();
}
  
void cMapInfoLoader::Init (std::string filename)
{
    XML::Parser parser;
	  XML::Node *mapentries, *document;

     try
     {
		  parser.loadData(filename);
		  document = parser.parseDocument();
		  
		  mapentries = document->findNode( "MAPS" );
     }
     catch (...) {
			  THROWEXCEPTION ("Couldn't find maps.xml");
     }

         if (!mapentries)
			  THROWEXCEPTION ("Couldn't find maps node.");
	  
  
       XML::Node *map_node, *value; 
       
         int idx = 0;
     
	  while ((map_node = mapentries->findNode("MAP", idx))) { 
 	      int id = map_node->findInteger("ID");
         if (!id) id = idx; // default value

          int width = map_node->findInteger("WIDTH");;
          int height = map_node->findInteger("HEIGHT");
          std::string name = map_node->findString("NAME");
          std::string skybox = map_node->findString("SKYBOX");
          
          int base_id = -1;
           if ((value = map_node->findNode("BASE_ID")))
				base_id = value->asInteger();

      
           cMapInfoEntry * map_entry = new cMapInfoEntry (id, width, height, name, skybox, base_id);
           maps.insert(make_pair(id, map_entry));
             idx++;
       }
      }

void cMapInfoLoader::DeInit ()    
{
 std::map<int, cMapInfoEntry*>::iterator iter;
 for(iter = maps.begin(); iter != maps.end(); iter++)
  delete iter->second;
  maps.clear();
}

cMapInfoEntry * cMapInfoLoader::GetMapInfo(int id)
{
 std::map<int, cMapInfoEntry*>::iterator iter;
 iter = maps.find(id);
 if(iter != maps.end())
  return iter->second;
 else
  return NULL; 
}

       		
				
