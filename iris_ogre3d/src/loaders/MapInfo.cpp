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
#include "profile.h"




MapInfoLoader *MapInfoLoader::m_sgMapInfoLoader = NULL;

cMapInfoEntry::cMapInfoEntry (int id, int width, int height, std::string name, std::string skybox, int base_id,
                              int r, int g, int b)
{PROFILE
      m_id = id;
      m_width = width;
      m_height = height;
      m_name = name;
      m_skybox = skybox;
      m_base_id = base_id;
      m_fog_r = r;
      m_fog_g = g;
      m_fog_b = b;
}

int cMapInfoEntry::id ()
{PROFILE
    return m_id;
}

int cMapInfoEntry::width ()
{PROFILE
    return m_width;
}

int cMapInfoEntry::height ()
{PROFILE
    return m_height;
}

int cMapInfoEntry::base_id ()
{PROFILE
    return m_base_id;
}

std::string cMapInfoEntry::name ()
{PROFILE
    return m_name;
}

std::string cMapInfoEntry::skybox ()
{PROFILE
    return m_skybox;
}

int cMapInfoEntry::fog_r()
{PROFILE
    return m_fog_r;
}

int cMapInfoEntry::fog_g()
{PROFILE
    return m_fog_g;
}

int cMapInfoEntry::fog_b()
{PROFILE
    return m_fog_b;
}


MapInfoLoader::MapInfoLoader( std::string sFileName )
{PROFILE
	assert( m_sgMapInfoLoader == NULL );
	m_sgMapInfoLoader = this;

	XML::Parser parser;
	XML::Node *mapentries, *document;

	try
	{
		parser.loadData( sFileName );
		document = parser.parseDocument();

		mapentries = document->findNode( "MAPS" );
	}
	catch (...)
	{
		THROWEXCEPTION( "Couldn't find maps.xml" );
	}

	if ( !mapentries )
	{
		THROWEXCEPTION( "Couldn't find maps node." );
	}


	XML::Node *map_node, *value; 

	int idx = 0;

	while ( ( map_node = mapentries->findNode( "MAP", idx ) ) )
	{ 
		int id = map_node->findInteger( "ID" );
		if ( !id )
		{
			id = idx; // default value
		}

		int width = map_node->findInteger( "WIDTH" );;
		int height = map_node->findInteger( "HEIGHT" );
		std::string name = map_node->findString( "NAME" );
		std::string skybox = map_node->findString( "SKYBOX" );

		int base_id = -1;

		if ( ( value = map_node->findNode( "BASE_ID" ) ) )
		{
			base_id = value->asInteger();
		}
		int r = 255, g = 255, b = 255;

		if ( ( value = map_node->findNode( "FOG_COLOR" ) ) )
		{
			value->lookupAttribute( "red", r );
			value->lookupAttribute( "blue", b );
			value->lookupAttribute( "green", g ); 
		}

		cMapInfoEntry *map_entry = new cMapInfoEntry( id, width, height, name, skybox, base_id, r, g, b );
		m_vMaps.insert( std::make_pair( id, map_entry ) );
		idx++;
	}
}


MapInfoLoader::~MapInfoLoader()
{PROFILE
	assert( m_sgMapInfoLoader != NULL );
	m_sgMapInfoLoader = NULL;

	std::map<int, cMapInfoEntry*>::iterator iter;
	for(iter = m_vMaps.begin(); iter != m_vMaps.end(); iter++)
	{
		SAFE_DELETE( iter->second );
	}
	m_vMaps.clear();
}


MapInfoLoader *MapInfoLoader::GetInstance()
{PROFILE
	return m_sgMapInfoLoader;
}


cMapInfoEntry *MapInfoLoader::GetMapInfo( int iId )
{PROFILE
	std::map<int, cMapInfoEntry*>::iterator iter;
	iter = m_vMaps.find( iId );
	if ( iter != m_vMaps.end() )
	{
		return iter->second;
	}
	else
	{
		return NULL;
	}
}

       		
				
