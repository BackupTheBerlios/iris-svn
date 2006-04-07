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


#include "loaders/UOMap.h"

#include "memguardconf.h"
#include "memguard.h"

MapLoader *pMapLoader = NULL;

UOMapLoader::UOMapLoader( char *mapfile, char *staticfile, char *staidx, int type )
{
	m_mapstream = new std::ifstream( mapfile, std::ios::in | std::ios::binary );
	if ( !m_mapstream->is_open() )
	{
		SAFE_DELETE( m_mapstream );

		THROWEXCEPTION( "Could not load map file: " + std::string(mapfile) );
	}

	m_staticstream = new std::ifstream( staticfile, std::ios::in | std::ios::binary );
	if ( !m_staticstream->is_open() )
	{
		SAFE_DELETE( m_mapstream );
		SAFE_DELETE( m_staticstream );

		THROWEXCEPTION( "Could not load static file: " + std::string( staticfile ) );
	}

	m_staidxstream = new std::ifstream( staidx, std::ios::in | std::ios::binary );
	if ( !m_staidxstream->is_open() )
	{
		SAFE_DELETE( m_mapstream );
		SAFE_DELETE( m_staticstream );
		SAFE_DELETE( m_staidxstream );

		THROWEXCEPTION( "Could not load static index file: " + std::string( staidx ) );
	}

	cMapInfoEntry *mapinfo_entry = MapInfoLoader::GetInstance()->GetMapInfo( type );
	if ( !mapinfo_entry )
	{
          THROWEXCEPTION( "Tried to load unknown map file" );
	}
	m_iWidth = mapinfo_entry->width();
	m_iHeight = mapinfo_entry->height();
}

UOMapLoader::~UOMapLoader()
{
	m_mapstream->close();
	SAFE_DELETE( m_mapstream );

	m_staticstream->close();
	SAFE_DELETE( m_staticstream );

	m_staidxstream->close();
	SAFE_DELETE( m_staidxstream );
}

void UOMapLoader::LoadMapBlock( int x, int y, MulBlock *block )
{
	if ( ( x >= m_iWidth ) || ( y >= m_iHeight ) || ( x < 0 ) || ( y < 0 ) )
	{
		memset( (char *) block, 0, sizeof(struct MulBlock) );
		return;
	}

	m_mapstream->seekg( ( ( x * m_iHeight ) + y ) * 196 + 4, std::ios::beg );
	m_mapstream->read( (char *) block, 192 );
  
	for ( int j = 0; j < 64; j++ )
	{
		block->cells[j / 8][j % 8].tileid =
		IRIS_SwapU16( block->cells[j / 8][j % 8].tileid );
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

struct staticinfo *UOMapLoader::LoadStatics( int x, int y, int &len )
{
	stIndexRecord block;
	if ( ( x >= m_iWidth ) || ( y >= m_iHeight ) || ( x < 0 ) || ( y < 0 ) )
	{
		return NULL;
	}

	m_staidxstream->seekg( ( x * m_iHeight + y ) * 12, std::ios::beg );
	m_staidxstream->read( (char *) &block, 12 );
	block.offset = IRIS_SwapU32( block.offset );
	block.length = IRIS_SwapU32( block.length );
	block.extra = IRIS_SwapU32( block.extra );

	if ( block.length == 0xffffffff )
	{
		return NULL;
	}

	len = block.length / 7;

	struct staticentry *uostatics =
		(struct staticentry *) malloc (block.length, "UOMapLoader::LoadStatics");
	struct staticinfo *static_infos =
		(struct staticinfo *) malloc (len * sizeof (struct staticinfo), "UOMapLoader::LoadStatics");


	m_staticstream->seekg( block.offset, std::ios::beg );
	m_staticstream->read((char *) uostatics, block.length );


	struct staticentry *uo_p = uostatics;
	struct staticinfo *info_p = static_infos;

	for ( int index = 0; index < len; index++ )
	{
		info_p->x = uo_p->x;
		info_p->y = uo_p->y;
		info_p->z = uo_p->z;
		info_p->TileID = IRIS_SwapU16( uo_p->TileID );
		info_p->Hue = IRIS_SwapI16( uo_p->Hue );
		info_p->obj_id = index;
		info_p++;
		uo_p++;
	}
	
	free( uostatics );

	return static_infos;
}
