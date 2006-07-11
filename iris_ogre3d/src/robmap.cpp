#include "robmap.h"

#include "renderer/3D/MapBlock3D.h"
#include "include.h"
#include "math.h"
#include "uotype.h"
#include "loaders/Map.h"
#include "renderer/TextureBuffer.h"
#include "renderer/SDLScreen.h"
#include "loaders/StaticModelLoader.h"
#include "loaders/TileDataLoader.h"
#include "loaders/MultisLoader.h"
#include "Geometry.h"
#include "renderer/MapBuffer.h"
#include "ogrewrapper.h"
#include "robinput.h"
#include <Ogre.h>


//// negativ div funktioniert nicht so leicht : abrunden nach 0 oder nach -unendlich? mit ints ? signed, unsigned ? ... alles scheisse =(
// absolute mapblock coordinaten sind >= 0  

/*
 Game.cpp:425:RenderScene
  renderer/3D/Renderer3D.cpp:231:RenderScene
   renderer/3D/Renderer3D.cpp:555:RenderTerrain
    renderer/3D/MapBuffer3D.cpp:29:CreateBlock
     renderer/3D/MapBuffer3D.cpp:66:GetEnvironment
      renderer/MapBuffer.cpp:89:Add
*/

// ***** ***** ***** ***** ***** cRobMapBlock



cRobMap::cRobMapBlock::cRobMapBlock	(int iBlockX,int iBlockY,Ogre::StaticGeometry* mpStaticGeom) { PROFILE
	//printf("cRobMapBlock(%d,%d)\n",iBlockX,iBlockY);
	
	
	int origin_abs_x = cRobMap::iOriginX * ROBMAP_CHUNK_SIZE;
	int origin_abs_y = cRobMap::iOriginY * ROBMAP_CHUNK_SIZE;
	Ogre::Vector3 vRobMapOriginOffset(8.0*(iBlockX-origin_abs_x),8.0*(iBlockY-origin_abs_y),0);
	
	cMapblock3D *block = reinterpret_cast < cMapblock3D * >(pMapbufferHandler.buffer ()->CreateBlock (iBlockX, iBlockY));
	
	
	// create manual object convert to mesh for terrain itself
	if (1) {
		msTerrainMeshName = cOgreWrapper::GetUniqueName();
		Texture *oldtexture;
		Texture *newtexture;
		mpManualObj = cOgreWrapper::GetSingleton().mSceneMgr->createManualObject(cOgreWrapper::GetUniqueName());	
		int vertexcount;
		oldtexture = 0;
		//vertex*	firstv = &block->ground_vertieces[0][0];
		//printf("FirstVertex(%f,%f,%f)\n",firstv->x,firstv->y,firstv->z);
		for (int tx = 0; tx < 8; tx++) for (int ty = 0; ty < 8; ty++) {
			newtexture = TextureBuffer::GetInstance()->GetGroundTexture (block->groundids[ty][tx]);
			//if (oldtexture) newtexture = oldtexture;  // speed check hack
			if (newtexture)  {
				if (oldtexture != newtexture) {
					if (oldtexture) mpManualObj->end();
					mpManualObj->begin(newtexture->GetGroundMaterial());
					oldtexture = newtexture;
					vertexcount = 0;
				}
			} else	{ 
				printf("Failed to load ground texture %d for %d,%d\n",block->groundids[ty][tx],tx,ty);
				continue;
			}
			
			vertex*	vertices[] = {	&block->ground_vertieces[ty][tx],
									&block->ground_vertieces[ty + 1][tx],
									&block->ground_vertieces[ty + 1][tx + 1],
									&block->ground_vertieces[ty][tx + 1],
									};
			//0	3
			//1	2
			int		indices[] = { 0,2,1, 0,3,2 };
			for (int i=0;i<6;++i) {				
				vertex* curvertex = vertices[indices[i]]; // float u, v, x, y, z; 
				mpManualObj->position(			curvertex->x,
												curvertex->y,
												curvertex->z );
				mpManualObj->normal(			0.0, // TODO : where to get the real normal ?!?
												0.0,
												1.0);
				mpManualObj->textureCoord(		curvertex->u,
												curvertex->v);
				mpManualObj->index(vertexcount++);
			}
		}
		if (oldtexture) mpManualObj->end();
		//mpManualObj->setCastShadows(false);
		
		mpManualObj->convertToMesh(msTerrainMeshName);
		//mpSceneNode->attachObject(mpManualObj);
		
		mpTerrainEntity = cOgreWrapper::GetSingleton().mSceneMgr->createEntity(cOgreWrapper::GetUniqueName(),msTerrainMeshName);
		mpTerrainEntity->setCastShadows(false);
		
		cOgreUserObjectWrapper* pUserObject = new cOgreUserObjectWrapper();
		pUserObject->mpMapBlock3D = block;
		pUserObject->mpStaticObject = 0;
		pUserObject->mpEntity = 0;
		pUserObject->mpDynamicObject = 0;
		pUserObject->mpCharacter = 0;
		mpManualObj->setUserObject(pUserObject); // TODO : add to mpTerrainEntity instead ???
	}
	
	//mpStaticGeom->reset();
	//mpStaticGeom->destroy();
	//mpStaticGeom->addSceneNode(mpSceneNode);
	
	// add terrain
	mpStaticGeom->addEntity(mpTerrainEntity,vRobMapOriginOffset);
	
	// add statics
	for (unsigned int i = 0; i < block->objects.count (); i++) {
		struct sStaticObject *object = block->objects.Get (i);
		//Ogre::Entity*& objentity = mlObjectEntityMap[object];
		Ogre::Entity* objentity = 0;
		cStaticModel *model = StaticModelLoader::GetInstance()->getModel(object->tileid);
		if (model) { // (!object->clip)
			const char* meshname = model->GetOgreMeshName();
			if (meshname) {
				objentity = cOgreWrapper::GetSingleton().mSceneMgr->createEntity(cOgreWrapper::GetUniqueName(),meshname);
				mlEntities.push_back(objentity);
				
				cOgreUserObjectWrapper* pUserObject = new cOgreUserObjectWrapper();
				pUserObject->mpMapBlock3D = block;
				pUserObject->mpStaticObject = object;
				pUserObject->mpEntity = objentity;
				pUserObject->mpDynamicObject = 0;
				pUserObject->mpCharacter = 0;
				objentity->setUserObject(pUserObject);
				
				mpStaticGeom->addEntity(objentity,vRobMapOriginOffset+Ogre::Vector3(object->x,object->y,object->z * 0.1f));
			}
		} else {
			printf("missing model for tileid %d\n",object->tileid);
		}
	}
}

cRobMap::cRobMapBlock::~cRobMapBlock	() { PROFILE
	
}

// ***** ***** ***** ***** ***** cRobMapChunk

cRobMap::cRobMapChunk::cRobMapChunk		(const int iX,const int iY) : miX(iX), miY(iY) { PROFILE
	printf("cRobMapChunk(%d,%d)\n",miX,miY);
	mpStaticGeom = cOgreWrapper::GetSingleton().mSceneMgr->createStaticGeometry(cOgreWrapper::GetUniqueName());
	
	int chunk_abs_x = miX * ROBMAP_CHUNK_SIZE;
	int chunk_abs_y = miY * ROBMAP_CHUNK_SIZE;
	int x,y;
	for (y=0;y<ROBMAP_CHUNK_SIZE;++y)
	for (x=0;x<ROBMAP_CHUNK_SIZE;++x) {
		mMapBlocks[y][x] = new cRobMapBlock(chunk_abs_x + x,chunk_abs_y + y,mpStaticGeom);
	}
	
	mpStaticGeom->build();
}

cRobMap::cRobMapChunk::~cRobMapChunk	() { PROFILE
	printf("DELETE cRobMapChunk(%d,%d)\n",miX,miY);
	int x,y;
	for (y=0;y<ROBMAP_CHUNK_SIZE;++y)
	for (x=0;x<ROBMAP_CHUNK_SIZE;++x) {
		delete mMapBlocks[y][x];
		mMapBlocks[y][x] = 0;
	}
	
	cOgreWrapper::GetSingleton().mSceneMgr->destroyStaticGeometry(mpStaticGeom->getName());
	mpStaticGeom = 0;
}

// ***** ***** ***** ***** ***** cRobMap

int			cRobMap::iOriginX = 0;
int			cRobMap::iOriginY = 0;

cRobMap::cRobMap		() {}
	

cRobMap::cRobMapChunk*	cRobMap::GetChunk		(const int iX,const int iY) { PROFILE
	for (std::list<cRobMapChunk*>::iterator itor=mlCache.begin();itor!=mlCache.end();++itor) 
		if ((*itor)->miX == iX && (*itor)->miY == iY) return (*itor);
	return 0;
}

void	cRobMap::LoadChunk		(const int iX,const int iY) { PROFILE
	if (iX < 0 || iY < 0) return; // must be positive
	cRobMapChunk* o = GetChunk(iX,iY);
	if (!o) { o = new cRobMapChunk(iX,iY); mlCache.push_front(o); }
}

/// these coordinates are in absolute map-blocks, all others are in chunks
void	cRobMap::UpdateCache	(int iMidX,int iMidY,int iRad) { PROFILE
	// transform coordinates from map-blocks to chunks
	
	int minx = (iMidX-iRad) / ROBMAP_CHUNK_SIZE;
	int miny = (iMidY-iRad) / ROBMAP_CHUNK_SIZE;
	int maxx = (iMidX+iRad) / ROBMAP_CHUNK_SIZE;
	int maxy = (iMidY+iRad) / ROBMAP_CHUNK_SIZE;
	
	// warning ! negative coords are bad, as rounding is towards 0, not towards -infinity... 
	// however that is no problem, as the uo map-blocks have positive coords
	iMidX /= ROBMAP_CHUNK_SIZE;
	iMidY /= ROBMAP_CHUNK_SIZE;
	iRad  /= ROBMAP_CHUNK_SIZE;
	
	// recenter if too far away, to avoid float-rounding-errors
	if (std::abs(iOriginX-iMidX) > ROBMAP_RECENTER_DIST || std::abs(iOriginY-iMidY) > ROBMAP_RECENTER_DIST) {
		printf("RecenterMap (%d,%d)\n",iMidX*ROBMAP_CHUNK_SIZE,iMidY*ROBMAP_CHUNK_SIZE);
		ClearCache();
		iOriginX = iMidX;
		iOriginY = iMidY;
	}
		
	// release invisible chunks
	for (std::list<cRobMapChunk*>::iterator itor=mlCache.begin();itor!=mlCache.end();) 
		if ((*itor)->miX < minx || (*itor)->miY < miny || (*itor)->miX > maxx || (*itor)->miY > maxy) {
			delete (*itor); // release chunk
			(*itor) = 0;
			mlCache.erase(itor++); // increment the iterator before it becomes invalid, and remove chunk pointer from list
		} else ++itor;
		
	// load visible chunks into cache if not already there
	int x,y;
	for (y=miny;y<=maxy;++y)
	for (x=minx;x<=maxx;++x)
		LoadChunk(x,y);
}

void	cRobMap::ClearCache		() { PROFILE
	PROFILE_PRINT_STACKTRACE
	printf("cRobMap::ClearCache\n");
	for (std::list<cRobMapChunk*>::iterator itor=mlCache.begin();itor!=mlCache.end();++itor) 
		delete (*itor); // release chunk
	mlCache.clear();
}
