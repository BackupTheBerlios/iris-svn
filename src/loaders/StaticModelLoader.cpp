//
// File: StaticModelLoader.cpp
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


#include "iris_endian.h"
#include "loaders/StaticModelLoader.h"
#include "loaders/StaticTextureLoader.h"
#include "Debug.h"
#include "Exception.h"
#include "uotype.h"
#include <string.h>

using namespace std;

cStaticModelLoader pStaticModelLoader;

char FILEID_GAMEMODELCOLLECTION[] = "UI3D";

struct sModelCollectionHeader
{
  char Sign[4];
  Uint32 Length;
  Uint32 Version;
  Uint32 ModelTableStart;
  Uint32 ModelTableCount;
  Uint32 TextureStart;
  Uint32 TextureLength;
  Uint32 Reserved[9];
} STRUCT_PACKED;


bool CheckHeaderID (char *ID1, char *ID2)
{
  for (int index = 0; index < 4; index++)
    if (ID1[index] != ID2[index])
      return false;
  return true;
}



cStaticModelLoader::cStaticModelLoader ()
{
    modelstream = NULL;
    modelstream_size = 0;
}

cStaticModelLoader::~cStaticModelLoader ()
{
    DeInit ();    
}

void cStaticModelLoader::Init (string filename)
{
 DeInit ();
  modelstream = new ifstream (filename.c_str (), ios::in | ios::binary);
  if (!modelstream->is_open ())
    THROWEXCEPTION (string ("model file not found: ") + filename);

  sModelCollectionHeader header;

  modelstream->seekg (0, ios::end);
  modelstream_size = modelstream->tellg ();

  if (modelstream_size < sizeof (header))
    THROWEXCEPTION ("invalid model size (<headersize)");

  modelstream->seekg (0, ios::beg);
  modelstream->read ((char *) &header, sizeof (header));
  header.Length = IRIS_SwapU32 (header.Length);
  header.Version = IRIS_SwapU32 (header.Version);
  header.ModelTableStart = IRIS_SwapU32 (header.ModelTableStart);
  header.ModelTableCount = IRIS_SwapU32 (header.ModelTableCount);
  header.TextureStart = IRIS_SwapU32 (header.TextureStart);
  header.TextureLength = IRIS_SwapU32 (header.TextureLength);
  for (int ii = 0; ii < 9; ii++)
    header.Reserved[ii] = IRIS_SwapU32 (header.Reserved[ii]);

  /* Check some header information */
  if (!CheckHeaderID (header.Sign, FILEID_GAMEMODELCOLLECTION))
    THROWEXCEPTION ("invalid model file");
  if (header.Version != GAMEMODEL_VERSION)
    THROWEXCEPTION ("model file has an invalid version!");
  if (modelstream_size != header.Length)
    THROWEXCEPTION ("invalid model file size");

  if ((modelstream_size < header.ModelTableStart) ||
      (modelstream_size <
       (header.ModelTableStart +
        header.ModelTableCount * sizeof (sModelTableEntry)))
      || (modelstream_size < header.TextureStart)
      || (modelstream_size < (header.TextureStart + header.TextureLength)))
    THROWEXCEPTION ("invalid model file");

  modelstream->seekg (header.TextureStart, ios::beg);
  static_texture_loader.Init (modelstream, header.TextureLength);

  modelstream->seekg (header.ModelTableStart, ios::beg);
  sModelTableEntry entry;

  for (unsigned int index = 0; index < header.ModelTableCount; index++)
      {
        modelstream->read ((char *) &entry, sizeof (sModelTableEntry));
        entry.id = IRIS_SwapU32 (entry.id);
        entry.start = IRIS_SwapU32 (entry.start);
        entry.length = IRIS_SwapU32 (entry.length);
        model_entries.insert (make_pair (entry.id, entry));
      }

  map < Uint32, sModelTableEntry >::iterator iter;
  for (iter = model_entries.begin (); iter != model_entries.end (); iter++)
      {
        modelstream->seekg (iter->second.start, ios::beg);

        cStaticModel *model =
          new cStaticModel (modelstream, iter->second.length, &static_texture_loader);
        models.insert (make_pair (iter->second.id, model));
      }
}


void cStaticModelLoader::DeInit ()
{
  map < Uint32, cStaticModel * >::iterator iter;
  for (iter = models.begin (); iter != models.end (); iter++)
    delete iter->second;
  models.clear ();

  static_texture_loader.DeInit ();

  delete modelstream;
  modelstream = NULL;
}

cStaticModel *cStaticModelLoader::getModel (Uint32 id)
{
  ASSERT (modelstream);
  
  map < Uint32, cStaticModel * >::iterator iter;
  iter = models.find (id);

  if (iter != models.end ())
    return iter->second;

  return NULL;
}

Texture * cStaticModelLoader::GetGroundTexture (Uint32 id) // Get replaced ground texture
{
      return static_texture_loader.GetGroundTexture (id);
}
