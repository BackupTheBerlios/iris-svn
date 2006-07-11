//
// File: robmap.h
// Created by: Robert Noll - ghoulsblade@schattenkind.net
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

/**
 Manages static geometry to avoid lag on mapblock change
 */
#ifndef _ROBMAP_H_
#define _ROBMAP_H_

#include <list>
#include <OgrePrerequisites.h>

#define ROBMAP_CHUNK_SIZE 4  // size of one chunk, should be 2^n
#define ROBMAP_RECENTER_DIST 10  // if cur position is off by more than 10 chunks, clear cache and recenter

// the whole cache consists of (ROBMAP_RECENTER_DIST*2)*(ROBMAP_RECENTER_DIST*2)  chunks
// every chunk consists of ROBMAP_CHUNK_SIZE*ROBMAP_CHUNK_SIZE blocks
// every block is equivalent to one MapBlock3D


class cRobMap { public:
	class cRobMapBlock { public:			
		std::string						msTerrainMeshName;
		Ogre::ManualObject*				mpManualObj;
		Ogre::Entity*					mpTerrainEntity;
		std::list<Ogre::Entity*>		mlEntities;
		std::map<struct sStaticObject*,Ogre::Entity*>		mlObjectEntityMap;
			
		cRobMapBlock	(int iBlockX,int iBlockY,Ogre::StaticGeometry* pStaticGeo); 	///< coords in absolute map-blocks, x,y >= 0
		~cRobMapBlock	();
	};
	
	class cRobMapChunk { public:
		int						miX,miY;
		Ogre::StaticGeometry* 	mpStaticGeom;
		cRobMapBlock*			mMapBlocks[ROBMAP_CHUNK_SIZE][ROBMAP_CHUNK_SIZE];
		
		cRobMapChunk	(const int iX,const int iY); ///< coords in chunks
		~cRobMapChunk	();
	};
	
	inline static cRobMap& GetSingleton () {
		static cRobMap* mSingleton = 0;
		if (!mSingleton) mSingleton = new cRobMap();
		return *mSingleton;
	}
	
	static int					iOriginX,iOriginY; ///< coords in chunks
	std::list<cRobMapChunk*>	mlCache;
	
	cRobMap();
	
	
	void	UpdateCache			(int iMidX,int iMidY,int iRad=2); 	///< coords in absolute map-blocks, x,y >= 0
	
	cRobMapChunk*	GetChunk	(const int iX,const int iY); 	///< coords in chunks, returns 0 if not found
	void		LoadChunk		(const int iX,const int iY); 	///< coords in chunks
	void		ClearCache		();
};

#endif
