//
// File: ModelInfoLoader.cpp
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


#include "loaders/ModelInfoLoader.h"

#include "Config.h"
#include "Debug.h"
#include <iostream>
#include "xml.h"
#include "iris_endian.h"

using namespace std;

cModelInfoLoader *pModelInfoLoader = NULL;

cModelInfoLoader::cModelInfoLoader ()
{

  XML::Parser parser;
  XML::Node * models, *document;

  try
  {
    parser.loadData ("./xml/ModelsInfo.xml");
    document = parser.parseDocument ();

    models = document->findNode ("MODELS");

    if (!models)
      throw "Couldn't find models node.";
  }
  catch (...)
  {
    pDebug.Log ("Couldn't load ModelsInfo");
    return;
  }

  XML::Node * model_node, *value;

  int idx = 0;

  while ((model_node = models->findNode ("MODEL", idx)))
      {
        int scalex = 10;
        int scaley = 10;
        int scalez = 10;
        int alpha = 255;
        int defhue = 0;
        int altbody = 0;
        value = model_node->findNode ("ID");

        Uint32 id = (value != NULL) ? value->asInteger () : 0;

        if ((value = model_node->findNode ("SCALE_X")))
          scalex = value->asInteger ();
        if ((value = model_node->findNode ("SCALE_Y")))
          scaley = value->asInteger ();
        if ((value = model_node->findNode ("SCALE_Z")))
          scalez = value->asInteger ();
        if ((value = model_node->findNode ("ALPHA")))
          alpha = value->asInteger ();
        if ((value = model_node->findNode ("DEFAULT_HUE")))
          defhue = value->asInteger ();
        if ((value = model_node->findNode ("ALT_BODY")))
          altbody = value->asInteger ();
        cModelInfoEntry *modelinfo = new cModelInfoEntry;
        
        modelinfo->scalex = IRIS_SwapI32(scalex);

        modelinfo->scaley = IRIS_SwapI32(scaley);
        modelinfo->scalez = IRIS_SwapI32(scalez);
        modelinfo->alpha = IRIS_SwapI32(alpha);
        modelinfo->defhue = IRIS_SwapI32(defhue);
        modelinfo->alt_body = IRIS_SwapI32(altbody);
        model_infos.insert (make_pair ((int) id, modelinfo));
        idx++;
      }
}

cModelInfoEntry *cModelInfoLoader::GetModelEntry (int id)
{
  std::map < int, cModelInfoEntry * >::iterator iter;
  iter = model_infos.find (id);
  if (iter != model_infos.end ())
    return iter->second;
  else
    return NULL;
}

cModelInfoLoader::~cModelInfoLoader()
{
 std::map<int, cModelInfoEntry*>::iterator iter;
  for (iter = model_infos.begin (); iter != model_infos.end (); iter++)
    delete iter->second;
  model_infos.clear();  
}


