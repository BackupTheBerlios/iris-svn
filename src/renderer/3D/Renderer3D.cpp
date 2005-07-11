//
// File: Renderer3D.cpp
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


#include "renderer/3D/Renderer3D.h"
#include "renderer/SDLScreen.h"
#include "renderer/MapBuffer.h"
#include "renderer/DynamicObjects.h"
#include "renderer/Characters.h"
#include "renderer/particles/ParticleEngine.h"
#include "renderer/3D/MapBlock3D.h"
#include "renderer/3D/MapBuffer3D.h"
#include "renderer/3D/SceneMaker.h"
#include "renderer/3D/Light3D.h"

#include "granny/GrannyLoader.h"

#include "loaders/HueLoader.h"
#include "loaders/StaticModelLoader.h"
#include "loaders/StitchinLoader.h"
#include "loaders/ModelInfoLoader.h"

#include "gui/GUIHandler.h"
#include "gui/Label.h"

#include "net/Client.h"

#include "include.h"
#include "Config.h"
#include "Debug.h"
#include "Game.h"

#include "Geometry.h"

#include <time.h>
#include <math.h>
#include <iostream>
#include <cassert>

//#include "cal3d/cal3d.h"

using namespace std;

extern SDLScreen *SDLscreen;
float water_phase = 0.0f;

extern float light_direction[3];
float light_angle = 0.4f;
float light_angle_dir = 1.0f;
int light_last_change = 0;
extern sColor ambient_color;
extern sColor sun_color;

//CalCoreModel m_calCoreModel;
//CalModel m_calModel;

float SkyBoxTexCoords[4][3] =
  { {0.0, 0.0}, {0.0, 1.0}, {1.0, 1.0}, {1.0, 0.0} };
float SkyBoxCoords[5][4][3] = {
  {{1.995f, -2.005f, 2.000f}, {1.995f, -2.005f, -2.000f}, {1.995f, 2.005f, -2.000f}, {1.995f, 2.005f, 2.000f}},   // East
  {{2.005f, -2.005f, 1.995f}, {2.005f, 2.005f, 1.995f}, {-2.005f, 2.005f, 1.995f}, {-2.005f, -2.005f, 1.995f}},   // Top
  {{-1.995f, 2.005f, 2.000f}, {-1.995f, 2.005f, -2.000f}, {-1.995f, -2.005f, -2.000f}, {-1.995f, -2.005f, 2.000f}},     // West
  {{-2.000f, -2.000f, 2.000f}, {-2.000f, -2.000f, -2.000f}, {2.000f, -2.000f, -2.000f}, {2.000f, -2.000f, 2.000f}},     // South
  {{2.000f, 2.000f, 2.000f}, {2.000f, 2.000f, -2.000f}, {-2.000f, 2.000f, -2.000f}, {-2.000f, 2.000f, 2.000f}}    // North
};
/*
char SkyBoxTextureNames[5][35] = { "./textures/skybox/skybox_east.jpg",
  "./textures/skybox/skybox_top.jpg", "./textures/skybox/skybox_west.jpg",
  "./textures/skybox/skybox_north.jpg",
  "./textures/skybox/skybox_south.jpg"
};

char SkyBoxTextureNames[5][35] = { "./textures/skybox/sunset_east.jpg",
  "./textures/skybox/sunset_top.jpg", "./textures/skybox/sunset_west.jpg",
  "./textures/skybox/sunset_north.jpg",
  "./textures/skybox/sunset_south.jpg"
};
*/
char SkyBoxTextureNames[5][36] = { "./textures/skybox/darksun_east.jpg",
  "./textures/skybox/darksun_top.jpg", "./textures/skybox/darksun_west.jpg",
  "./textures/skybox/darksun_north.jpg",
  "./textures/skybox/darksun_south.jpg"
};

Renderer3D::Renderer3D ()
{
  light_direction[0] = 0.1f;
  light_direction[1] = 0.2f;
  light_direction[2] = 0.6f;
  for (int index = 0; index < 5; index++)
    skyboxtextures[index] = NULL;
  tex_char_shadow = NULL;
  tex_water = NULL;
}

Renderer3D::~Renderer3D ()
{
}

int Renderer3D::Init (void)
{
//flo: wozu?
//  DeInit ();
  
  tex_water = new Texture;
  tex_water->LoadFromFileWithTransparency ("./textures/water1.jpg", 200);

  tex_char_shadow = new Texture;
  tex_char_shadow->LoadFromFile ("./textures/char_shadow.png");

  LoadSkyboxTextures ();

  if (pUOGUI)
      {
        pUOGUI->LoadCursor (0, 0x205a);
        pUOGUI->LoadCursor (1, 0x205b);
        pUOGUI->LoadCursor (2, 8305);
        pUOGUI->LoadCursor (3, 8310);
      }


  init_vertex_buffer ();

/*  if(!m_calCoreModel.create("dummy"))
  {
    CalError::printLastError();
    return false;
  }
      
      std::cout << "Loading skeleton..." << std::endl;
      if(!m_calCoreModel.loadCoreSkeleton("data/cally/cally.csf"))
      {
        CalError::printLastError();
        return false;
      }
   
        std::cout << "Loading mesh..." << std::endl;
      if(m_calCoreModel.loadCoreMesh("data/cally/cally_chest.cmf") == -1)
      {
        CalError::printLastError();
        return false;
      }
  m_calCoreModel.getCoreSkeleton()->calculateBoundingBox(&m_calCoreModel);

  // create the model instance from the loaded core model
  if(!m_calModel.create(&m_calCoreModel))
  {
    CalError::printLastError();
    return false;
  }

  // attach all meshes to the model
  int meshId;
  for(meshId = 0; meshId < m_calCoreModel.getCoreMeshCount(); meshId++)
  {
    m_calModel.attachMesh(meshId);
  }

  // set the material set of the whole model
  m_calModel.setMaterialSet(0); */

  return (true);
}


void Renderer3D::LoadSkyboxTextures ()
{
  for (int index = 0; index < 5; index++)
      {
        skyboxtextures[index] = new Texture ();
        skyboxtextures[index]->LoadFromFile (SkyBoxTextureNames[index]);
      }
}


int Renderer3D::DeInit (void)
{
  for (int index = 0; index < 5; index++)
    if (skyboxtextures[index])
        {
          delete skyboxtextures[index];
          skyboxtextures[index] = NULL;
        }

  delete tex_water;
  tex_water = NULL;

  delete tex_char_shadow;
  tex_char_shadow = NULL;

  printf ("DeInit\n");

  free_vertex_buffer ();



  // destroy the model instance
  /*m_calModel.destroy();

     // destroy the core model instance
     m_calCoreModel.destroy(); */

  return (true);
}


extern int m_count;

/*void renderMesh ()
{
  // get the renderer of the model
  CalRenderer *pCalRenderer;
  pCalRenderer = m_calModel.getRenderer();
  
  m_calModel.update(0.1f);

  glPushMatrix ();
//  glLoadIdentity ();
  glTranslatef(0,0,-50);
  glScalef(-0.1,0.1,0.4);
  glDisable(GL_BLEND);
  glDisable(GL_ALPHA_TEST);
  // begin the rendering loop
  if(!pCalRenderer->beginRendering()) return;

  // get the number of meshes
  int meshCount;
  meshCount = pCalRenderer->getMeshCount();
  //glDisable(GL_TEXTURE_2D);
  glColor4f(1.0f,1.0f,1.0f,1.0f);
  //glDisable(GL_CULL_FACE);

  // render all meshes of the model
  int meshId;
  for(meshId = 0; meshId < meshCount; meshId++)
  {
    // get the number of submeshes
    int submeshCount;
    submeshCount = pCalRenderer->getSubmeshCount(meshId);

    // render all submeshes of the mesh
    int submeshId;
    for(submeshId = 0; submeshId < submeshCount; submeshId++)
    {
      // select mesh and submesh for further data access
      if(pCalRenderer->selectMeshSubmesh(meshId, submeshId))
      {
        unsigned char meshColor[4];
        GLfloat materialColor[4];

        // get the transformed vertices of the submesh
        static float meshVertices[30000][3];
        int vertexCount;
        vertexCount = pCalRenderer->getVertices(&meshVertices[0][0]);

        // get the transformed normals of the submesh
        static float meshNormals[30000][3];
        pCalRenderer->getNormals(&meshNormals[0][0]);

        // get the texture coordinates of the submesh
        static float meshTextureCoordinates[30000][2];
        int textureCoordinateCount;
        textureCoordinateCount = pCalRenderer->getTextureCoordinates(0, &meshTextureCoordinates[0][0]);
        

        // get the faces of the submesh
        static CalIndex meshFaces[50000][3];
        int faceCount;
        faceCount = pCalRenderer->getFaces(&meshFaces[0][0]);

    glEnableClientState (GL_VERTEX_ARRAY);
    glEnableClientState (GL_TEXTURE_COORD_ARRAY);
    glDisableClientState (GL_COLOR_ARRAY);
        // set the vertex and normal buffers
        glVertexPointer(3, GL_FLOAT, 0, &meshVertices[0][0]);
        //glNormalPointer(GL_FLOAT, 0, &meshNormals[0][0]);

/*        // set the texture coordinate buffer and state if necessary
        if((pCalRenderer->getMapCount() > 0) && (textureCoordinateCount > 0))
        {
          glEnable(GL_TEXTURE_2D);
          glEnableClientState(GL_TEXTURE_COORD_ARRAY);
          glEnable(GL_COLOR_MATERIAL);

          // set the texture id we stored in the map user data
          glBindTexture(GL_TEXTURE_2D, (GLuint)pCalRenderer->getMapUserData(0));

          // set the texture coordinate buffer
          glColor3f(1.0f, 1.0f, 1.0f);
        }*/

  //glBindTexture(GL_TEXTURE_2D, 0);
  /*      glTexCoordPointer(2, GL_FLOAT, 0, &meshTextureCoordinates[0][0]);
     // draw the submesh
     if(sizeof(CalIndex)==2)
     glDrawElements(GL_TRIANGLES, faceCount * 3, GL_UNSIGNED_SHORT, &meshFaces[0][0]);
     else
     glDrawElements(GL_TRIANGLES, faceCount * 3, GL_UNSIGNED_INT, &meshFaces[0][0]);




     // disable the texture coordinate state if necessary
     /*        if((pCalRenderer->getMapCount() > 0) && (textureCoordinateCount > 0))
     {
     glDisable(GL_COLOR_MATERIAL);
     glDisableClientState(GL_TEXTURE_COORD_ARRAY);
     glDisable(GL_TEXTURE_2D);
     } */

/*glBegin(GL_TRIANGLES);
glColor3f(1.0f, 1.0f, 1.0f);
int i;
for (i = 0; i < faceCount; i++) {
    glTexCoord2fv(meshTextureCoordinates[meshFaces[i][0]]);
    glVertex3fv(meshVertices[meshFaces[i][0]]);
    glTexCoord2fv(meshTextureCoordinates[meshFaces[i][1]]);
    glVertex3fv(meshVertices[meshFaces[i][1]]);
    glTexCoord2fv(meshTextureCoordinates[meshFaces[i][2]]);
    glVertex3fv(meshVertices[meshFaces[i][2]]);
}
/*int vertexId;
for(vertexId = 0; vertexId < vertexCount; vertexId++)
{
//  glVertex3f(meshVertices[vertexId][0], meshVertices[vertexId][1], meshVertices[vertexId][2]);
/*const float scale = 0.3f;
   printf("%.2f %.2f %.2f\n", meshVertices[vertexId][0], meshVertices[vertexId][1], meshVertices[vertexId][2]);
  glVertex3f(meshVertices[vertexId][0], meshVertices[vertexId][1], meshVertices[vertexId][2]);
  glVertex3f(meshVertices[vertexId][0] + meshNormals[vertexId][0] * scale, meshVertices[vertexId][1] + meshNormals[vertexId][1] * scale, meshVertices[vertexId][2] + meshNormals[vertexId][2] * scale);
}*/
//glEnd();

// DEBUG-CODE //////////////////////////////////////////////////////////////////
/*
*/
////////////////////////////////////////////////////////////////////////////////

/*      }

    }
  }
  // clear vertex array state
  //glDisableClientState(GL_NORMAL_ARRAY);
  //glDisableClientState(GL_VERTEX_ARRAY);

  / reset the lighting mode
  if(bLight)
  {
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
  }

  // reset the global OpenGL states
  glDisable(GL_DEPTH_TEST);*/

  /*// reset wireframe mode if necessary
     if(bWireframe)
     {
     glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
     } */

  // end the rendering
/*  pCalRenderer->endRendering();
  glPopMatrix ();
    glEnable(GL_TEXTURE_2D);
}*/


int Renderer3D::RenderScene (void)
{

  if (!pMapbuffer)
    return false;

  float fog_view_dist = (float) (view_distance - 2.5f) * 8 + pCamera.GetZoom();
  glFogf (GL_FOG_START, (float) fog_view_dist);
  glFogf (GL_FOG_END, (float) fog_view_dist + 8);

  flush_vertex_buffer ();

  HandleStaticFaders ();

  glEnable (GL_CULL_FACE);
  flag_cullbackfaces = true;

  SDLscreen->ClearScreen ();

  SDLscreen->SetLight (m_lightlevel);
  glDisable (GL_LIGHTING);


  int current_ticks = SDL_GetTicks ();
  if (current_ticks - light_last_change > 15000)
      {
        // This is a temporary day cycle
        light_angle +=
          (current_ticks - light_last_change) * 0.000001f * light_angle_dir;
//        light_angle = 0.2;
        light_direction[0] = cos (0.6f) * sin (light_angle);
        light_direction[1] = cos (light_angle) * sin (0.6f);
        light_direction[2] = sin (light_angle);
        // Normalize
        float l =
          sqrt (light_direction[0] * light_direction[0] +
                light_direction[1] * light_direction[1] +
                light_direction[2] * light_direction[2]);
        light_direction[0] /= l;
        light_direction[1] /= l;
        light_direction[2] /= l;

        if (light_angle > 3.14159f * 0.9f)
          light_angle_dir = -1.0f;
        if (light_angle < 3.14159f * 0.1f)
          light_angle_dir = 1.0f;
        ((cMapbuffer3D *) pMapbuffer)->SetRecalcAmbientLightFlag ();
        pDynamicObjectList->SetRecalcAmbientLightFlag ();


        light_last_change = current_ticks;
      }


  float cam_x, cam_y, cam_z;
  if (pClient)
      {
        cCharacter *player_character = pClient->player_character ();
        if (player_character)
            {
              float player_x, player_y, player_z;
              pCamera.GetGamePosition (cam_x, cam_y, cam_z);
              player_character->getPosition (player_x, player_y, player_z);

              if ((player_x != cam_x) || (player_y != cam_y)
                  || (player_z != cam_z))
                  {
                    pCamera.SetX (-(player_x - (int) (player_x / 8.0f) * 8));
                    pCamera.SetY (-(player_y - (int) (player_y / 8.0f) * 8));
                    pCamera.SetZ (-player_z);

                    pCamera.SetBlockX ((int) player_x / 8);
                    pCamera.SetBlockY ((int) player_y / 8);


                  }
              if (nConfig::perspective == 1)

                pCamera.SetAngleZ (-player_character->angle () + 180);

            }

      }


  bool do_culling = pCamera.CamHasChanged () != 0;

  // Do our Skybox

  pCamera.PlaceGLRotationMatrix ();

  RenderSkybox ();

  SDLscreen->ClearZBuffer ();


  pCamera.PlaceGLMatrix ();

  pCamera.FetchMatrix ();

  if (do_culling)
    ExtractFrustum ();


  static int old_z = ROOF_NONE;
  if (do_culling || (old_z == ROOF_WAIT))
      {
        int z = GetRoofHeight ();


        if (z != old_z)
            {
              if (pMapbuffer)
                pMapbuffer->setRoofZ (z);
              pMapbuffer->UpdateAlpha ();
              if (static_faders.size ())
                  {
                    old_z = ROOF_WAIT;
                  }
              else
                  {
                    cFader *fader =
                      new cFader (255.0f, nConfig::roof_fade_alpha,
                                  nConfig::roof_fade_time);
                    fader->Start ();
                    int count = SetFader (z, 255, fader, false);
                    if (count)
                      static_faders.push_back (fader);
                    else
                      delete fader;

                    fader =
                      new cFader (nConfig::roof_fade_alpha, 255.0f,
                                  nConfig::roof_fade_time);
                    fader->Start ();
                    count =
                      SetFader (z, nConfig::roof_fade_alpha, fader, true);
                    if (count)
                      static_faders.push_back (fader);
                    else
                      delete fader;
                    old_z = z;
                  }
            }
      }

  if (do_culling)
    pMapbuffer->SetUsageFlag (false);

  RenderTerrain (do_culling);
  RenderDynamics (do_culling);
  if (drag_model)
    RenderDragModel ();
  render_vertex_buffer ();
  RenderWater (do_culling);

  // must be rendered last because of Char Shadows
  RenderCharacters (do_culling);

//  does not work correct
  if (pParticleEngine)
    pParticleEngine->Render ();

  render_vertex_buffer_transparent ();

  glMatrixMode (GL_MODELVIEW_MATRIX);

  if (pUOGUI)
    pUOGUI->Draw ();

  return SDLscreen->DrawGL ();
}

/* Skybox Renderer */
void Renderer3D::RenderSkybox ()
{
  for (int side = 0; side < 5; side++)
    if (skyboxtextures[side])
        {
          glBindTexture (GL_TEXTURE_2D, skyboxtextures[side]->GetGLTex ());
          glBegin (GL_QUADS);
          for (int vertex = 0; vertex <= 3; vertex++)
              {
                glTexCoord2fv (SkyBoxTexCoords[vertex]);
                glVertex3fv (SkyBoxCoords[side][vertex]);
              }
          glEnd ();
        }
}

void Renderer3D::RenderTerrain (bool do_culling)
{
  int blockx = pCamera.GetBlockX ();
  int blocky = pCamera.GetBlockY ();

  for (int y = -view_distance; y < view_distance; y++)

    for (int x = -view_distance; x < view_distance; x++)
        {
          cMapblock3D *block =
            reinterpret_cast <
            cMapblock3D * >(pMapbuffer->CreateBlock (blockx + x, blocky + y));
          if (block)
              {
                block->Render (x, y, do_culling, x * 8.0f, y * 8.0f);
              }
        }
}

/* Water Renderer */
void Renderer3D::RenderWater (bool do_culling)
{
  if (!tex_water)
    return;

  static unsigned int lasttime = 0;

  unsigned int currenttime = SDL_GetTicks ();

  if (lasttime)
    water_phase += (float) (currenttime - lasttime) * 0.001f;

  lasttime = currenttime;

  if (water_phase >= 2.0f)
    (water_phase -= 2.0f);

  int blockx = pCamera.GetBlockX ();
  int blocky = pCamera.GetBlockY ();

  glBindTexture (GL_TEXTURE_2D, tex_water->GetGLTex ());

  for (int y = -view_distance; y < view_distance; y++)
    for (int x = -view_distance; x < view_distance; x++)
        {
          cMapblock3D *block =
            reinterpret_cast <
            cMapblock3D * >(pMapbuffer->CreateBlock (blockx + x, blocky + y));
          if (block)
              {
                glPushMatrix ();
                glTranslatef (x * 8.0f, y * 8.0f, 0.0f);
                block->RenderWater (x, y, do_culling);
                glPopMatrix ();
              }
        }

}


void Renderer3D::RenderDynamics (bool do_culling)
{
  if (!pDynamicObjectList)
    return;

  int blockx = pCamera.GetBlockX ();
  int blocky = pCamera.GetBlockY ();

  int min_x = (blockx - view_distance) * 8;
  int min_y = (blocky - view_distance) * 8;
  int max_x = (blockx + view_distance) * 8;
  int max_y = (blocky + view_distance) * 8;
  int dx = blockx * 8;
  int dy = blocky * 8;

  dynamiclist_t *dynamics = pDynamicObjectList->GetList ();
  dynamiclist_t::iterator iter;

  for (iter = dynamics->begin (); iter != dynamics->end (); iter++)
      {
        cDynamicObject *object = iter->second;
        if ((object->x >= min_x) && (object->y >= min_y)
            && (object->x <= max_x) && (object->y <= max_y))
            {
              if (object->fader)
                  {
                    int act_alpha = (int) (object->fader->value () + 0.5f);
                    object->alpha = act_alpha;
                  }

              cStaticModel *model =
                pStaticModelLoader.getModel (object->model);
              if (model)
                  {
                    if (object->motive ())
                        {
                          if (object->RecalcAmbientLightFlag ())
                            object->motive ()->
                              CalcAmbientLight (ambient_color, sun_color,
                                                light_direction);
                          object->motive ()->PrepareModelForRendering ();
                        }
                    model->Render (object->x - dx, object->y - dy,
                                   object->z * 0.1f, object->alpha);
                  }

            }
      }
}



void Renderer3D::RenderCharacters (bool do_culling)
{
  if (!pCharacterList || !pClient)
    return;

  int blockx = pCamera.GetBlockX ();
  int blocky = pCamera.GetBlockY ();

  int min_x = (blockx - view_distance) * 8;
  int min_y = (blocky - view_distance) * 8;
  int max_x = (blockx + view_distance) * 8;
  int max_y = (blocky + view_distance) * 8;
  int dx = blockx * 8;
  int dy = blocky * 8;

  float colr, colg, colb;

  characterlist_t *characters = pCharacterList->GetList ();
  characterlist_t::iterator iter;
  Uint32 currentticks = SDL_GetTicks ();

  glEnable (GL_CULL_FACE);
  for (iter = characters->begin (); iter != characters->end (); iter++)
      {
        cCharacter *character = iter->second;
        if ((character->x () >= min_x) && (character->y () >= min_y)
            && (character->x () <= max_x) && (character->y () <= max_y))
            {

              cModelInfoEntry * modelinfo = pModelInfoLoader->GetModelEntry(character->body());

              float alpha = 1.0f;
              float scalex = 1.0f, scaley = 1.0f, scalez = 1.0f;
              int defhue = 0, altbody = 0;
                                 if(modelinfo)
                                 {
                                  scalex = modelinfo->scalex / 10.0f;
                                  scaley = modelinfo->scaley / 10.0f;
                                  scalez = modelinfo->scalez / 10.0f;
                                  alpha = modelinfo->alpha / 255.0f;
                                  defhue = modelinfo->defhue;
                                  altbody = modelinfo->alt_body;
                                 }

              float matrix[16], invmatrix[16];
              glPushMatrix ();
              glLoadIdentity ();
              glScalef (-1.0f * scalex, 1.0f * scaley, 1.0f * scalez);
              glTranslatef (-0.5f, 0.5f, 0.0f);
              glRotatef (-character->angle (), 0.0f, 0.0f, 1.0f);
              glGetFloatv (GL_MODELVIEW_MATRIX, matrix);
              InvertMatrix (matrix, invmatrix);

              cCharacterLight *light = character->light ();
              assert (light);
              light->Generate (character->fx () - dx, character->fy () - dy,
                               character->fz () * 0.1f, blockx, blocky,
                               ambient_color, sun_color, light_direction,
                               invmatrix);

              glPopMatrix ();

              glPushMatrix ();

              glTranslatef (character->fx () - dx, character->fy () - dy,
                            character->fz () * 0.1f);

              glDisable (GL_ALPHA_TEST);
              glBindTexture (GL_TEXTURE_2D, tex_char_shadow->GetGLTex ());
              glBegin (GL_QUADS);
              glTexCoord2f (0.0f, 0.0f);
              glVertex3f (1.0f, 0.0f, 0.01f);
              glTexCoord2f (1.0f, 0.0f);
              glVertex3f (0.0f, 0.0f, 0.01f);
              glTexCoord2f (1.0f, 1.0f);
              glVertex3f (0.0f, 1.0f, 0.01f);
              glTexCoord2f (0.0f, 1.0f);
              glVertex3f (1.0f, 1.0f, 0.01f);
              glEnd ();
              glEnable (GL_ALPHA_TEST);

              glMultMatrixf (matrix);
              float curtime = character->animtime ();
              float delta_time =
                (currentticks - character->lastanim ()) * 0.001f;
              if (delta_time > 0.1f)
                delta_time = 0.1f;
              character->setLastAnim (currentticks);
              curtime += delta_time;

              int anim_type = character->animtype ();

              if (character->isCorpse ())
                anim_type = 22;

              int body = character->body ();

              if (pGrannyLoader)
                  {
                    Uint32 base_id = character->body () << 16;
                    if (character->body () == 402
                        || character->body () == 403)
                      alpha = 0.4f;

                    if ((pGame.GetPointedObj () == character->id ())
                        || (pClient->GetEnemy () == character->id ()))
                      pHueLoader.GetRGBHue ((int) character->
                                             getHighlightColor (), colr, colg,
                                             colb);
                    else
                      pHueLoader.GetRGBHue (character->hue (), colr, colg,
                                             colb);
/*                                                if((pGame.GetPointedObj() == character->id()) || (pClient->GetEnemy() == character->id()))
                                                        SDLscreen->SetHue((int) character->getHighlightColor());
                                                else
                                                        SDLscreen->SetHue(character->hue());*/

                    if (defhue != 0)
                      pHueLoader.GetRGBHue (defhue, colr, colg, colb);
                    bool found = pGrannyLoader->isFound (body);

                    if ((altbody != 0) && (!found))
                      body = altbody;

                    GrnMatrix left_matrix;
                    GrnMatrix right_matrix;

                    std::vector < int >removelist;
                    std::map < int, int >replacelist;
                    std::map < int, int >::iterator iter;
                    if ((nConfig::aos)
                        && (character->body () == 400
                            || character->body () == 401))
                        {

                          std::vector < int >bodyparts;
                          for (int bp = 0; bp < 13; bp++)
                            bodyparts.push_back (0);

                          for (int layer = 0; layer < 25; layer++)
                              {
                                cCharacterEquip *eqp =
                                  character->GetEquip (layer);
                                if (eqp)
                                    {

                                      int mod_id = eqp->anim ();

                                      cModelEntry *modelentry =
                                        pStitchinLoader->GetModel (mod_id);
                                      if (!modelentry)
                                        continue;

                                      int *covers = modelentry->GetCovers ();

                                      for (int c = 0; c < 13; c++)
                                          {
                                            if (covers[c] == 1)
                                                {
                                                  bodyparts[c] = 1;
                                                }
                                          }

                                      std::vector < int >rlist =
                                        modelentry->GetRemoveList ();

                                      for (unsigned int r = 0; r < rlist.size (); r++)
                                          {
                                            removelist.push_back (rlist.
                                                                  at (r));
                                          }
                                      std::map < int, int >replist =
                                        modelentry->GetReplaceList ();
                                      for (iter = replist.begin ();
                                           iter != replist.end (); iter++)
                                          {
                                            replacelist.
                                              insert (make_pair
                                                      (iter->first,
                                                       iter->second));
                                          }
                                    }
                              }
                          pGrannyLoader->Render (body, anim_type, curtime,
                                                 light, colr, colg, colb,
                                                 alpha, bodyparts,
                                                 &left_matrix, &right_matrix,
                                                 character->isCorpse ());


                        }
                    else
                      pGrannyLoader->Render ( body,
                                             anim_type, curtime, light, colr,
                                             colg, colb, alpha, &left_matrix,
                                             &right_matrix,
                                             character->isCorpse ());
                    glPushMatrix ();
                    glMultMatrixf (right_matrix.matrix);
                    glGetFloatv (GL_MODELVIEW_MATRIX, right_matrix.matrix);
                    glPopMatrix ();

                    glPushMatrix ();
                    glMultMatrixf (left_matrix.matrix);
                    glGetFloatv (GL_MODELVIEW_MATRIX, left_matrix.matrix);
                    glPopMatrix ();



                    for (int layer = 0; layer < 25; layer++)
                        {
                          cCharacterEquip *equip =
                            character->GetEquip (layer);
                          if (equip)
                              {
                                if ((pGame.GetPointedObj () ==
                                     character->id ())
                                    || (pClient->GetEnemy () ==
                                        character->id ()))
                                  pHueLoader.GetRGBHue ((int) character->
                                                         getHighlightColor (),
                                                         colr, colg, colb);
                                else
                                  pHueLoader.GetRGBHue (equip->hue (), colr,
                                                         colg, colb);
                                int anim = equip->anim ();

                                if ((nConfig::aos)
                                    && (character->body () == 400
                                        || character->body () == 401))
                                    {
                                      iter = replacelist.find (anim);
                                      if (iter != replacelist.end ())
                                        anim = iter->second;
                                      bool found = false;
                                      for (unsigned int r = 0; r < removelist.size ();
                                           r++)
                                          {
                                            if (removelist.at (r) == anim)
                                                {
                                                  found = true;

                                                  break;
                                                }
                                          }
                                      if (!found)
                                        pGrannyLoader->Render (base_id | anim,
                                                               anim_type,
                                                               curtime, light,
                                                               colr, colg,
                                                               colb, alpha,
                                                               &left_matrix,
                                                               &right_matrix,
                                                               character->
                                                               isCorpse ());
                                    }
                                else
                                  pGrannyLoader->Render (base_id | anim,
                                                         anim_type, curtime,
                                                         light, colr, colg,
                                                         colb, alpha,
                                                         &left_matrix,
                                                         &right_matrix,
                                                         character->
                                                         isCorpse ());

                              }
                        }
                    cCharacterEquip *mounttile;
                    cMount *mount = NULL;
                    mounttile = character->GetEquip (25);
                    Uint16 mountmodel;
                    int mountanim = 4;
                    if (character->moving ())
                        {
                          if (character->direction () & 0x80)
                            mountanim = 2;
                          else
                            mountanim = 0;
                        }
                    if (mounttile != NULL)
                        {
                          mount = new cMount (mounttile->model ());
                          mountmodel = mount->GetMountType ();
                          if ((pGame.GetPointedObj () == character->id ())
                              || (pClient->GetEnemy () == character->id ()))
                            pHueLoader.GetRGBHue ((int) character->
                                                   getHighlightColor (), colr,
                                                   colg, colb);
                          else
                            pHueLoader.GetRGBHue (mounttile->hue (), colr,
                                                   colg, colb);
                          pGrannyLoader->Render ( mountmodel,
                                                 mountanim, curtime, light,
                                                 colr, colg, colb, alpha,
                                                 &left_matrix, &right_matrix);
                        }
                    SDLscreen->SetHue ();
           /* TODO (#1#): Temporary fix for a bad memory leak...need to find a
                           better solution in the future */

           if(mount)
            delete mount;
                  }
              character->setAnimtime (curtime);
              glPopMatrix ();
            }


      }



}

void Renderer3D::RenderDragModel ()
{


  int blockx = pCamera.GetBlockX () * 8;
  int blocky = pCamera.GetBlockY () * 8;

  cStaticModel *model = pStaticModelLoader.getModel (drag_model);
  if (model)
    model->Render (dragposition[0] - blockx, dragposition[1] - blocky,
                   dragposition[2] * 0.1f, 255);

}


void Renderer3D::GrabDynamic (int x, int y, cDynamicObject ** r_object,
                              cCharacter ** r_character)
{
  if (r_object)
    *r_object = NULL;
  if (r_character)
    *r_character = NULL;

  if (!pDynamicObjectList || !pCharacterList)
    return;

  {
    // Obtain Cam Coords and PickRay

    float vecPickRayOrigin[3];
    float vecPickRayDir[3];

    pCamera.PlaceGLMatrix ();
    pCamera.CreatePickRay (x, y, vecPickRayOrigin, vecPickRayDir);

    int CamBlockX, CamBlockY;
    float CamX, CamY, CamZ;

    CamBlockX = pCamera.GetBlockX ();
    CamBlockY = pCamera.GetBlockY ();
    CamX = vecPickRayOrigin[0] + CamBlockX * 8.0f;
    CamY = vecPickRayOrigin[1] + CamBlockY * 8.0f;
    CamZ = vecPickRayOrigin[2];

    CamBlockX = (int) CamX / 8;
    CamBlockY = (int) CamY / 8;
    CamX -= CamBlockX * 8.0f;
    CamY -= CamBlockY * 8.0f;

    // Check Blocks against ray

    int blockx = pCamera.GetBlockX ();
    int blocky = pCamera.GetBlockY ();
    int deltax = blockx * 8;
    int deltay = blocky * 8;

    cDynamicObject *picked_object = NULL;

    float distance = 1000000.0f;
    float lambda;
    dynamiclist_t *dynamics = pDynamicObjectList->GetList ();
    dynamiclist_t::iterator iter;

    for (iter = dynamics->begin (); iter != dynamics->end (); iter++)
        {
          cDynamicObject *object = iter->second;
          cStaticModel *model = pStaticModelLoader.getModel (object->model);
          if (model && (object->alpha == 255))
              {
                if (model->
                    CheckRay (vecPickRayOrigin, vecPickRayDir,
                              object->x - deltax, object->y - deltay,
                              object->z, lambda))
                  if (lambda < distance)
                      {
                        distance = lambda;
                        picked_object = object;
                      }
              }
        }


    if (r_character)
        {
          cCharacter *character =
            pCharacterList->CheckRay (vecPickRayOrigin, vecPickRayDir,

                                      -deltax, -deltay, 0.0f, lambda);
          if (character)        // if character is nearer than found object
            if (lambda < distance)
                {
                  *r_character = character;
                  return;
                }
        }

    if (r_object)
      *r_object = picked_object;


  }
}

void Renderer3D::GrabMousePosition (int x, int y, int max_z, int cursor3d[3],
                                    int *cursor_character)
{
  if (!pCharacterList )
    return;

  // Obtain Cam Coords and PickRay

  float vecPickRayOrigin[3];
  float vecPickRayDir[3];

  pCamera.PlaceGLMatrix ();
  pCamera.CreatePickRay (x, y, vecPickRayOrigin, vecPickRayDir);

  int CamBlockX, CamBlockY;
  float CamX, CamY, CamZ;

  CamBlockX = pCamera.GetBlockX ();
  CamBlockY = pCamera.GetBlockY ();
  CamX = vecPickRayOrigin[0] + CamBlockX * 8.0f;
  CamY = vecPickRayOrigin[1] + CamBlockY * 8.0f;
  CamZ = vecPickRayOrigin[2];

  CamBlockX = (int) CamX / 8;
  CamBlockY = (int) CamY / 8;
  CamX -= CamBlockX * 8.0f;
  CamY -= CamBlockY * 8.0f;

  // Check Blocks against ray

  int view_distance = GetViewDistance ();

  int blockx = pCamera.GetBlockX ();
  int blocky = pCamera.GetBlockY ();

  sStaticObject *picked_object = NULL;
  int picked_x = -1;
  int picked_y = -1;
  int picked_z = 0;
  float distance = 1000000.0f;

  for (int y = -view_distance; y < view_distance; y++)
    for (int x = -view_distance; x < view_distance; x++)
        {
          cMapblock3D *block =
            reinterpret_cast <
            cMapblock3D * >(pMapbuffer->CreateBlock (blockx + x, blocky + y));
          if (block)
              {
                float act_distance;
                int z;
                sStaticObject *result =
                  block->CheckRay (vecPickRayOrigin, vecPickRayDir,
                                   act_distance, x * 8.0f, y * 8.0f, max_z);
                if (result)
                  if (act_distance < distance)
                      {
                        distance = act_distance;
                        picked_object = result;
                        picked_x = -1;
                        picked_y = -1;
                      }
                int id =
                  block->CheckRayOnGround (vecPickRayOrigin, vecPickRayDir,
                                           act_distance, x * 8.0f, y * 8.0f,
                                           z);
                if (id != 255)
                    {
                      if (act_distance < distance)
                          {
                            distance = act_distance;
                            picked_object = NULL;
                            picked_x = (id % 8) + (blockx + x) * 8;
                            picked_y = (id / 8) + (blocky + y) * 8;
                            picked_z = z;
                          }
                    }


              }
        }

  cursor_character = 0;

  if (picked_object)
      {
        cursor3d[0] = picked_object->x + picked_object->blockx * 8;
        cursor3d[1] = picked_object->y + picked_object->blocky * 8;
        cursor3d[2] = picked_object->z + picked_object->height;
      }

  if (picked_x != -1)
      {
        cursor3d[0] = picked_x;
        cursor3d[1] = picked_y;
        cursor3d[2] = picked_z;
      }

/*			float act_distance;
			cCharacter * character = pCharacterList->CheckRay(vecPickRayOrigin, vecPickRayDir, - blockx * 8.0f, - blocky * 8.0f, 0.0f, act_distance);
			if (character && (act_distance < distance)) {
                             distance = act_distance;
                             if (cursor_character)
                                 *cursor_character =  character->id();
                        }*/
}

void Renderer3D::AddDynamic (cDynamicObject * object)
{
}

void Renderer3D::DelDynamic (cDynamicObject * object)
{
}
