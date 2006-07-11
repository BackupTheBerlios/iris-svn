//
// File: Renderer3D.cpp
// Created by: Alexander Oster - tensor@ultima-iris.de
// Modified by: Florian Fischer - seabeams@gmx.de
// 27.07.05 - custom Skybox support added via Maps.xml
//          - and lots of bugfixes
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
#include <time.h>
#include <math.h>
#include <iostream>
#include <cassert>

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
#include "loaders/MapInfo.h"
#include "Exception.h"

#include "include.h"
#include "Config.h"
#include "Logger.h"
#include "Game.h"

#include "Geometry.h"
#include "ogrewrapper.h"
#include <Ogre.h>
#include "robinput.h"
#include "robmap.h"



//extern SDLScreen *SDLscreen;
float water_phase = 0.0f;

// Make it better
float light_angle = 0.4f;
float light_angle_dir = 1.0f;
int light_last_change = 0;

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

char SkyBoxTextureNames[5][12] = {
  "_east.jpg",
  "_top.jpg",
  "_west.jpg",
  "_north.jpg",
  "_south.jpg"};


Renderer3D::Renderer3D ()
{PROFILE
  for (int index = 0; index < 5; index++)
    skyboxtextures[index] = NULL;
  tex_char_shadow = NULL;
  tex_water = NULL;
  m_kGame = Game::GetInstance();
}

Renderer3D::~Renderer3D()
{PROFILE
	m_kGame = NULL;
	DeInit();
}

int Renderer3D::Init (void)
{PROFILE
  tex_water = new Texture();
  tex_water->LoadFromFileWithTransparency ("./textures/water1.png", 200);

  tex_char_shadow = new Texture();
  tex_char_shadow->LoadFromFile ("./textures/char_shadow.png");

//removed because Skybox don't look good with dynamic lightning and fog
//  LoadSkyboxTextures ();

  pUOGUI.LoadCursor (0, 0x205a);
  pUOGUI.LoadCursor (1, 0x205b);
  pUOGUI.LoadCursor (2, 8305);
  pUOGUI.LoadCursor (3, 8310);

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

void Renderer3D::LoadSkyboxTextures (int map)
{PROFILE
  for (int index = 0; index < 5; index++)
      {
       if(skyboxtextures[index])
        delete skyboxtextures[index];
      }   
  char skyboxset[50]; 
  string skyboxtemp = "./textures/skybox/skybox";
  string skyboxtype;

  cMapInfoEntry * mapinfo_entry = MapInfoLoader::GetInstance()->GetMapInfo(map);
  if(!mapinfo_entry)
   THROWEXCEPTION ("unknown map!");

  if(mapinfo_entry->skybox () != "")
    skyboxtemp = mapinfo_entry->skybox ();

  for (int index = 0; index < 5; index++)
      {
        skyboxtextures[index] = new Texture ();

        skyboxtype=skyboxtemp + SkyBoxTextureNames[index];
        strcpy( skyboxset, skyboxtype.c_str() );

        skyboxtextures[index]->LoadFromFile (skyboxset);
      }
}

void Renderer3D::DeInit( void )
{PROFILE
	for (int index = 0; index < 5; index++)
	{
		if (skyboxtextures[index])
		{
			delete skyboxtextures[index];
			skyboxtextures[index] = NULL;
        }
	}

	delete tex_water;
	tex_water = NULL;

	delete tex_char_shadow;
	tex_char_shadow = NULL;

	free_vertex_buffer();

	// destroy the model instance
	/*
	m_calModel.destroy();

	// destroy the core model instance
	m_calCoreModel.destroy();
	*/
}



void Renderer3D::RenderScene( void )
{PROFILE
	
	
	if ( pClient )
	{
		cCharacter *player_character = pClient->player_character();
		if ( player_character )
		{
			float player_x, player_y, player_z;
			pCamera.GetGamePosition( cam_x, cam_y, cam_z );
			player_character->getPosition( player_x, player_y, player_z );
			//printf("player_character->getPosition = (%0.1f,%0.1f,%0.1f)\n",player_x, player_y, player_z);

			if ( ( player_x != cam_x ) || ( player_y != cam_y ) || ( player_z != cam_z ) )
			{
				
				pCamera.SetX( -( player_x - (int)( player_x / 8.0f ) * 8 ) );
				pCamera.SetY( -( player_y - (int)( player_y / 8.0f ) * 8 ) );
				pCamera.SetZ( -player_z );

				pCamera.SetBlockX( (int) player_x / 8 );
				pCamera.SetBlockY( (int) player_y / 8 );
			}
			if ( Config::GetPerspective() == 1 && ( !pCamera.forceRotation() ) )
			{
				pCamera.SetAngleZ( -player_character->angle() + 180 );
			}
			
			/*
			glTranslatef (0.0f, 0.0f, -5.0f - pCamera.GetZoom());
			glRotatef (pCamera.GetAngleX() - 45.0, 	1.0, 0.0, 0.0);
			glRotatef (pCamera.GetAngleY(), 		0.0, 1.0, 0.0);
			glRotatef (-pCamera.GetAngleZ() + 0.0, 	0.0, 0.0, 1.0);
			glScalef (-1.0, 1.0, 1.0f);
			glTranslatef (pCamera.GetX() - 0.5f, pCamera.GetY() - 0.5f, (pCamera.GetZ() - 15.0f) * 0.1f);
			*/
			
			static Ogre::Quaternion viewahead(Ogre::Degree(90),Ogre::Vector3::UNIT_X);
			static Ogre::Quaternion curview;
			Ogre::Quaternion playerdir(Ogre::Degree(180 + pClient->player_character()->angle()),Ogre::Vector3::UNIT_Z);
			
			static int lastmousex = 0;
			static int lastmousey = 0;
			static float lookangle_h = 0;
			static float lookangle_v = 0;
			
			if (cInput::bKeys[cInput::kkey_mouse1] && cInput::bKeys[cInput::kkey_mouse2]) {
				static float mousesensitivity = 0.02;
				lookangle_h -= float(cInput::iMouse[0] - lastmousex) * mousesensitivity;
				lookangle_v -= float(cInput::iMouse[1] - lastmousey) * mousesensitivity;
				curview =	Ogre::Quaternion(Ogre::Radian(lookangle_h),Ogre::Vector3::UNIT_Z) *
							Ogre::Quaternion(Ogre::Radian(lookangle_v),Ogre::Vector3::UNIT_X) *
							viewahead;
			} else {
				lookangle_h = 0;
				lookangle_v = 0;
				curview = viewahead;
			}
			lastmousex = cInput::iMouse[0];
			lastmousey = cInput::iMouse[1];
			
			static Ogre::Quaternion camrot;
			camrot = playerdir * curview;
			cOgreWrapper::GetSingleton().SetCameraRot( camrot );
			// -pClient->player_character()->angle()
			
			/*
			cOgreWrapper::GetSingleton().SetCameraRot( 
				Ogre::Quaternion(Ogre::Degree(pCamera.GetAngleX() - 45.0)	,Ogre::Vector3::UNIT_X) *
				Ogre::Quaternion(Ogre::Degree(pCamera.GetAngleY())			,Ogre::Vector3::UNIT_Y) *
				Ogre::Quaternion(Ogre::Degree(-pCamera.GetAngleZ() + 0.0)	,Ogre::Vector3::UNIT_Z) 
				);
			*/
			
			int camoff_x = pCamera.GetBlockX() - cRobMap::iOriginX * ROBMAP_CHUNK_SIZE;
			int camoff_y = pCamera.GetBlockY() - cRobMap::iOriginY * ROBMAP_CHUNK_SIZE;
			cOgreWrapper::GetSingleton().SetCameraPos(Ogre::Vector3(
				8.0*camoff_x - (pCamera.GetX() - 0.5f), 
				8.0*camoff_y - (pCamera.GetY() - 0.5f), 
				-(pCamera.GetZ() - 15.0f) * 0.1f
				));
				
				
			//printf("SetCameraPos(%f,%f)\n",8.0*camoff_x - (pCamera.GetX() - 0.5f), 8.0*camoff_y - (pCamera.GetY() - 0.5f));
				
			if (cInput::bKeys[cInput::kkey_i]) cOgreWrapper::GetSingleton().mCamera->moveRelative(Ogre::Vector3(0,0,5));
			if (cInput::bKeys[cInput::kkey_k]) cOgreWrapper::GetSingleton().mCamera->moveRelative(Ogre::Vector3(0,0,-5));
				
			/*
			//cOgreWrapper::GetSingleton().CameraLookAt(Ogre::Vector3(
			cOgreWrapper::GetSingleton().SetCameraPos(Ogre::Vector3(
				( player_x - (int)( player_x / 8.0f ) * 8 ) ,
				( player_y - (int)( player_y / 8.0f ) * 8 ) ,
				10
				// -player_z  + 21  // +x is a hack, don't know how these coords work yets..
				));
				*/
		}
	}
	
	bool do_culling = true; // TODO : FIXME later...
	RenderTerrain( do_culling );
	RenderDynamics( do_culling );
	RenderCharacters( do_culling );
	
	pUOGUI.Draw();
	
	return;
	
	//fog_view_dist = (float) (view_distance - 2.5f) * 8 + pCamera.GetZoom();
	
	/*
	// TODO : ghoulsblade convert to ogre
	glFogf( GL_FOG_START, (float)fog_view_dist );
	glFogf( GL_FOG_END, (float)fog_view_dist + 8 );
	*/
	
	//printf ("Fogdistance_START: %i\n", (int) fog_view_dist);

	//flush_vertex_buffer();

	//HandleStaticFaders();

	// glEnable( GL_CULL_FACE ); // replaced by ogre
	flag_cullbackfaces = true;

	// SDLScreen::GetInstance()->ClearScreen();  // ghoulsblade: deaktivated for ogre

	// SDLScreen::GetInstance()->SetLight( m_lightlevel );
	//glDisable( GL_LIGHTING );


	current_ticks = SDL_GetTicks();

	/*
	if ( current_ticks - light_last_change > 150 )
	{
		// Dirty hack (set real time instead of current_ticks / 250)
   	     int worldtime = (current_ticks / 250) % 256;
       
		// This is a temporary day cycle
		light_angle += ( current_ticks - light_last_change ) * 0.0001f * light_angle_dir;
		light_angle = (float) worldtime / 256.0f * 3.14159f;
		if (light_angle <= 0.1f) light_angle = 0.1f;
		if (light_angle >= 3.0f) light_angle = 3.0f;
		m_light_direction[0] = cos( 0.2f ) * sin( light_angle );
		m_light_direction[1] = cos( light_angle ) * sin( 0.2f );
		m_light_direction[2] = sin( light_angle );
        
		// Normalize (NOTE: Multiplication is faster than division).
		lightNormalize = 1 / sqrt( m_light_direction[0] * m_light_direction[0] + 
			m_light_direction[1] * m_light_direction[1] + m_light_direction[2] * m_light_direction[2] );
		m_light_direction[0] *= lightNormalize;
		m_light_direction[1] *= lightNormalize;
		m_light_direction[2] *= lightNormalize;
		
		m_sun_light_color = world_environment().getSunLightColor (worldtime);
		m_ambient_light_color = world_environment().getAmbientLightColor (worldtime);

//		printf ("%i %i %i %i %.2f\n", (current_ticks / 250) % 256, m_sun_light_color.colorRGB.r, m_sun_light_color.colorRGB.g, m_sun_light_color.colorRGB.b, light_angle);

		if ( light_angle > 3.14159f * 0.9f )
		{
			light_angle_dir = -1.0f;
		}

		if (light_angle < 3.14159f * 0.1f)
		{
			light_angle_dir = 1.0f;
		}
		( (Mapbuffer3D *)pMapbufferHandler.buffer() )->SetLightColorAndDirection (m_ambient_light_color, m_sun_light_color, m_light_direction);
		( (Mapbuffer3D *)pMapbufferHandler.buffer() )->SetRecalcAmbientLightFlag();
		pDynamicObjectList.SetRecalcAmbientLightFlag ();
		
		sColor fogcolor = world_environment().getFogColor (worldtime);
    	SDLScreen::GetInstance()->SetFogColor (fogcolor.colorRGB.r, fogcolor.colorRGB.g, fogcolor.colorRGB.b);

        light_last_change = current_ticks;
	}
	*/


	do_culling = pCamera.CamHasChanged() != 0;

	// Do our Skybox
	// pCamera.PlaceGLRotationMatrix(); // ghoulsblade : deactivated

//	RenderSkybox();

	// SDLScreen::GetInstance()->ClearZBuffer(); // ghoulsblade : deaktivated for ogre

	// pCamera.PlaceGLMatrix();  // ghoulsblade : deactivated

	// pCamera.FetchMatrix();  // ghoulsblade : deactivated

	/* // ghoulsblade : deactivated
	if ( do_culling )
	{
		ExtractFrustum();
	}
	*/

	//check is only if you are in 1st person (roofs shouldn't fade out)
	/*if(nConfig::roof_fade_time < 0)
		nConfig::roof_fade_alpha = 255;
	else
		nConfig::roof_fade_alpha = 1;*/
 
	static int old_z = ROOF_NONE;
	//static int force_fadein = 0;

	
	/* // ghoulsblade : deactivated
	if ( do_culling || ( old_z == ROOF_WAIT ) )
	{
		int z;// = GetRoofHeight ();
	
		if ( Config::GetRoofFade() )
		{
			z = GetRoofHeight ();
		}
		else
		{
			z = ROOF_NONE;
			//force_fadein = 1;
		}

		if ( z != old_z )
		{
			pMapbufferHandler.buffer()->setRoofZ( z );
			pMapbufferHandler.buffer()->UpdateAlpha();

			if ( static_faders.size() )
			{
				old_z = ROOF_WAIT;
			}
			else
			{
				cFader *fader = new cFader( 255.0f, Config::GetRoofFadeAlpha(), Config::GetRoofFadeTime() );
				fader->Start();

				int count = SetFader( z, 255, fader, false );
				if ( count )
				{
					static_faders.push_back( fader );
				}
				else
				{
					delete fader;
				}

				fader = new cFader( Config::GetRoofFadeAlpha(), 255.0f, Config::GetRoofFadeTime() );
				fader->Start();
				count = SetFader( z, Config::GetRoofFadeAlpha(), fader, true );

				if ( count )
				{
					static_faders.push_back( fader );
				}
				else
				{
					delete fader;
				}
				old_z = z;
			}
		}
	}
	*/

	/* ghoulsblade : deactivated
	if ( do_culling )
	{
		pMapbufferHandler.buffer()->SetUsageFlag( false );
	}
	*/


/*
	if ( drag_model )
	{
		RenderDragModel();
	}

	render_vertex_buffer();
	RenderWater( do_culling );

	// must be rendered last because of Char Shadows

	//  does not work correct
	pParticleEngine.Render();

	//render_vertex_buffer_transparent();
*/

	/* TODO : ghoulsblade : port me to ogre, opengl commands are not available directly
	glMatrixMode( GL_MODELVIEW_MATRIX );
	*/
	

	// SDLScreen::GetInstance()->DrawGL(); // ghoulsblade: deaktivated for ogre
}


/* Skybox Renderer */
void Renderer3D::RenderSkybox ()
{PROFILE
/* TODO : ghoulsblade : port me to ogre, opengl commands are not available directly
	for ( int side = 0; side < 5; side++ )
	{
		if ( skyboxtextures[side] )
		{
			glBindTexture( GL_TEXTURE_2D, skyboxtextures[side]->GetGLTex() );

			//Just a Test for correct Skybox ... GL_EXT Feature
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x812F  GL_CLAMP_TO_EDGE); 
			//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x812F  GL_CLAMP_TO_EDGE);

			glBegin( GL_QUADS );
			for ( int vertex = 0; vertex <= 3; vertex++ )
			{
				glTexCoord2fv( SkyBoxTexCoords[vertex] );
				glVertex3fv( SkyBoxCoords[side][vertex] );
			}
			glEnd();
		}
	}
	*/
}

void Renderer3D::RenderTerrain (bool do_culling) {PROFILE
	int blockx = pCamera.GetBlockX ();
	int blocky = pCamera.GetBlockY ();
	
	cRobMap::GetSingleton().UpdateCache(blockx,blocky,view_distance);

	/*
  for (int y = -view_distance; y < view_distance; y++)
    for (int x = -view_distance; x < view_distance; x++)
        {
          cMapblock3D *block = reinterpret_cast < cMapblock3D * >(pMapbufferHandler.buffer ()->CreateBlock (blockx + x, blocky + y));
          if (block)
              {
                block->DrawStep (x, y, do_culling, x * 8.0f, y * 8.0f);
              }
        }
	pMapbufferHandler.buffer()->StepAll();
	*/
}

/* Water Renderer */
void Renderer3D::RenderWater (bool do_culling)
{PROFILE/* TODO : ghoulsblade : port me to ogre, opengl commands are not available directly
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
  float r = (float) m_ambient_light_color.colorRGB.r / 255.0f * 2.0f;
  if (r > 1.0f) r = 1.0f;
  float g = (float) m_ambient_light_color.colorRGB.g / 255.0f * 2.0f;
  if (g > 1.0f) g = 1.0f;
  float b = (float) m_ambient_light_color.colorRGB.g / 255.0f * 2.0f;
  if (b > 1.0f) b = 1.0f;
  glColor4f (r, g, b, 0.9f);

  for (int y = -view_distance; y < view_distance; y++)
    for (int x = -view_distance; x < view_distance; x++)
        {
          cMapblock3D *block =
            reinterpret_cast <
            cMapblock3D * >(pMapbufferHandler.buffer ()->CreateBlock (blockx + x, blocky + y));
          if (block)
              {
                glPushMatrix ();
                glTranslatef (x * 8.0f, y * 8.0f, 0.0f);
                block->RenderWater (x, y, do_culling);
                glPopMatrix ();
              }
        }
	*/
}


void Renderer3D::RenderDynamics( bool do_culling )
{PROFILE
	int blockx = pCamera.GetBlockX();
	int blocky = pCamera.GetBlockY();

	int min_x = (blockx - view_distance) * 8;
	int min_y = (blocky - view_distance) * 8;
	int max_x = (blockx + view_distance) * 8;
	int max_y = (blocky + view_distance) * 8;
	int dx = blockx * 8;
	int dy = blocky * 8;
	
	dx = 8 * ( cRobMap::iOriginX * ROBMAP_CHUNK_SIZE);
	dy = 8 * ( cRobMap::iOriginY * ROBMAP_CHUNK_SIZE);
	
	dynamiclist_t *dynamics = pDynamicObjectList.GetList();
	dynamiclist_t::iterator iter;
	
	
	static bool debug_hidedynamics = false;
	static bool debug_hidedynamics_keydown = false;
	if (cInput::bKeys[cInput::kkey_8]) {
		if (!debug_hidedynamics_keydown) {
			debug_hidedynamics = !debug_hidedynamics;
			debug_hidedynamics_keydown = true;
		}
	} else debug_hidedynamics_keydown = false;

	for ( iter = dynamics->begin(); iter != dynamics->end(); iter++ )
	{
		cDynamicObject *object = iter->second;
		bool bVisible = ( object->x >= min_x ) && ( object->y >= min_y ) && ( object->x <= max_x ) && ( object->y <= max_y );
		if (debug_hidedynamics) bVisible = false;
		
		if ( bVisible )
		{
			if ( object->fader )
			{
				int act_alpha = (int)( object->fader->value() + 0.5f );
				object->alpha = act_alpha;
			}

			/*
			cStaticModel *model = StaticModelLoader::GetInstance()->getModel( object->model );
			if ( model )
			{
				if ( object->motive() )
				{
					if ( object->RecalcAmbientLightFlag() )
					{
						object->motive()->CalcAmbientLight( m_ambient_light_color, m_sun_light_color, m_light_direction );
					}
					object->motive()->PrepareModelForRendering();
				}
				model->Render( object->x - dx, object->y - dy, object->z * 0.1f, object->alpha );
			}*/
		}
		
		object->DrawStep(object->x - dx, object->y - dy, object->z, object->alpha,bVisible);
		//printf("object->DrawStep final(%f,%f) obj(%f,%f) d(%f,%f)\n",(float)(object->x - dx), (float)(object->y - dy),(float)(object->x), (float)(object->y),(float)(dx), (float)(dy));
	}
}



void Renderer3D::RenderCharacters (bool do_culling)
{PROFILE
	//TODO : ghoulsblade : port me to ogre, opengl commands are not available directly
  	if (!pClient)
    return;

  int blockx = pCamera.GetBlockX ();
  int blocky = pCamera.GetBlockY ();

  int min_x = (blockx - view_distance) * 8;
  int min_y = (blocky - view_distance) * 8;
  int max_x = (blockx + view_distance) * 8;
  int max_y = (blocky + view_distance) * 8;
  int dx = blockx * 8;
  int dy = blocky * 8;
	dx = 8 * ( cRobMap::iOriginX * ROBMAP_CHUNK_SIZE);
	dy = 8 * ( cRobMap::iOriginY * ROBMAP_CHUNK_SIZE);

  float colr, colg, colb;

  characterlist_t *characters = pCharacterList.GetList ();
  characterlist_t::iterator iter;
  Uint32 currentticks = SDL_GetTicks ();

	bool bVisible;
	
	

	static bool debug_hidechars = false;
	static bool debug_hidechars_keydown = false;
	if (cInput::bKeys[cInput::kkey_9]) {
		if (!debug_hidechars_keydown) {
			debug_hidechars = !debug_hidechars;
			debug_hidechars_keydown = true;
		}
	} else debug_hidechars_keydown = false;
	
  // glEnable (GL_CULL_FACE); // ghoulsblade : deactivated
  for (iter = characters->begin (); iter != characters->end (); iter++)
      {
			cCharacter *character = iter->second;
			
		  bVisible = !((character->id() == pClient->player_charid()) && Config::GetHideSelf() ) &&
					(	(character->x () >= min_x) && (character->y () >= min_y) && 
						(character->x () <= max_x) && (character->y () <= max_y));
			
		  if (debug_hidechars) bVisible = false;
		  
		  if  (!character->mpGrannyWrapper) 
				character->mpGrannyWrapper = cOgreWrapper::GetSingleton().CreateOgreGrannyWrapper();
		
		  cOgreGrannyWrapper* pGrannyWrapper = character->mpGrannyWrapper;
			
		  
		  pGrannyWrapper->DrawStep(bVisible,character->fx () - dx, character->fy () - dy, character->fz () * 0.1f);
		  
        if (bVisible) {
				cModelInfoEntry *modelinfo = ModelInfoLoader::GetInstance()->GetModelEntry(character->body());

              float alpha = 1.0f;
              float scalex = 1.0f, scaley = 1.0f, scalez = 1.0f;
              
              scalex = ModelInfoLoader::GetInstance()->getScaleFactor();
              scaley = ModelInfoLoader::GetInstance()->getScaleFactor();
              scalez = ModelInfoLoader::GetInstance()->getScaleFactor();
              
              int defhue = 0, altbody = 0;
              
              if(modelinfo)
              {
               scalex = modelinfo->scalex ();
               scaley = modelinfo->scaley ();
               scalez = modelinfo->scalez ();
               alpha = modelinfo->alpha () / 255.0f;
               defhue = modelinfo->defhue ();
               altbody = modelinfo->alt_body ();
              }

              float matrix[16];
			  
			  
              pGrannyWrapper->glPushMatrix ();
              pGrannyWrapper->glLoadIdentity ();
			  
			  /*
			  orig : 
              glScalef (-1.0f * scalex, 1.0f * scaley, 1.0f * scalez);
              glTranslatef (-0.5f, 0.5f, 0.0f);
              glRotatef (-character->angle (), 0.0f, 0.0f, 1.0f);
			  */
			  
			pGrannyWrapper->glScalef (scalex, scaley, scalez); // ghoulsblade : added for testing
			pGrannyWrapper->glTranslatef (0.5, 0.5, 0.0f); // ghoulsblade : added for testing
			pGrannyWrapper->glRotatef (character->angle (), 0.0f, 0.0f, 1.0f);
			
			  
			//pGrannyWrapper->glScalef (-1.0f * scalex, 1.0f * scaley, 1.0f * scalez); // ghoulsblade : deactivated
			
			  
			  //pGrannyWrapper->glTranslatef (-0.5f, 0.5f, 0.0f); // ghoulsblade : deactivated
			//pGrannyWrapper->glTranslatef (0.5f, 0.5f, 0.0f); // ghoulsblade : added for testing
			//pGrannyWrapper->glRotatef (-character->angle (), 0.0f, 0.0f, 1.0f);
			pGrannyWrapper->glGetFloatv (cOgreGrannyWrapper::GL_MODELVIEW_MATRIX, matrix);
               
			  
			  cCharacterLight *light = character->light ();
			  assert (light);
			  
			  /* // ghoulsblade : deactivated
              float invmatrix[16];
              InvertMatrix (matrix, invmatrix);
              light->Generate (character->fx () - dx, character->fy () - dy,
                               character->fz () * 0.1f, blockx, blocky,
                               m_ambient_light_color, m_sun_light_color, m_light_direction,
                               invmatrix);
			  */
			  
              pGrannyWrapper->glPopMatrix ();
              pGrannyWrapper->glPushMatrix ();

			  /*
				pCamera.SetX( -( player_x - (int)( player_x / 8.0f ) * 8 ) );
				pCamera.SetY( -( player_y - (int)( player_y / 8.0f ) * 8 ) );
				pCamera.SetZ( -player_z );
				cOgreWrapper::GetSingleton().SetCameraPos(Ogre::Vector3(
					-(pCamera.GetX() - 0.5f), 
					-(pCamera.GetY() - 0.5f), 
					-(pCamera.GetZ() - 15.0f) * 0.1f
					));
				pCamera.SetBlockX( (int) player_x / 8 );
				pCamera.SetBlockY( (int) player_y / 8 );
			  int dx = blockx * 8;
			  int dy = blocky * 8;
					
					
				cOgreWrapper::GetSingleton().SetCameraPos(Ogre::Vector3(
					(( player_x - dx ) + 0.5f), 
					(( player_y - dy ) + 0.5f), 
					(player_z + 15.0f) * 0.1f
					));
			  */
              //pGrannyWrapper->glTranslatef (character->fx () - dx, character->fy () - dy, character->fz () * 0.1f); // ghoulsblade : deactivated
							
							
				//printf("scale (%0.1f,%0.1f,%0.1f)\n",1.0f * scalex, 1.0f * scaley, 1.0f * scalez);
				//printf("char (%0.1f,%0.1f,%0.1f)\n",character->fx (),character->fy (),character->fz ());

				// shadow
				/* // ghoulsblade : deactivated
              glDisable (GL_ALPHA_TEST);
              glBindTexture (GL_TEXTURE_2D, tex_char_shadow->GetGLTex ());
              glBegin (GL_QUADS);
              glTexCoord2f (0.0f, 0.0f);glVertex3f (1.0f, 0.0f, 0.01f);
              glTexCoord2f (1.0f, 0.0f);glVertex3f (0.0f, 0.0f, 0.01f);
              glTexCoord2f (1.0f, 1.0f);glVertex3f (0.0f, 1.0f, 0.01f);
              glTexCoord2f (0.0f, 1.0f);glVertex3f (1.0f, 1.0f, 0.01f);
              glEnd ();
              glEnable (GL_ALPHA_TEST);
			  */

              pGrannyWrapper->glMultMatrixf (matrix);
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

                    if ((m_kGame->GetPointedObj () == character->id ())
                        || (pClient->GetEnemy () == character->id ()))
                      pHueLoader.GetRGBHue ((int) character->
                                             getHighlightColor (), colr, colg,
                                             colb);
                    else
                      pHueLoader.GetRGBHue (character->hue (), colr, colg,
                                             colb);
#if 0
                    if((m_kGame->GetPointedObj() == character->id()) || (pClient->GetEnemy() == character->id())) 
                                              SDLScreen::GetInstance()->SetHue((int) character->getHighlightColor());
                                           else
                                              SDLScreen::GetInstance()->SetHue(character->hue());
#endif											  

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

					if ((Config::GetAOS())
                        && (character->body () == 400
                            || character->body () == 401))
                        {

                          std::vector < int >bodyparts(13,0);

                          for (int layer = 0; layer < 25; layer++)
                              {
                                cCharacterEquip *eqp =
                                  character->GetEquip (layer);
                                if (eqp)
                                    {

                                      int mod_id = eqp->anim ();

                                      cModelEntry *modelentry =
                                        pStitchinLoader.GetModel (mod_id);
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
                                            removelist.push_back (rlist.at(r));
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

                              //Harkon: 
                              //always render hands for recal hands matrix
                              bodyparts[3] = 0;

                              pGrannyLoader->Render (pGrannyWrapper,body, anim_type, curtime,
                                                 light, colr, colg, colb,
                                                 alpha, bodyparts,
                                                 &left_matrix, &right_matrix,
                                                 character->isCorpse ());


                        }
                    else
                      pGrannyLoader->Render ( pGrannyWrapper,body,
                                             anim_type, curtime, light, colr,
                                             colg, colb, alpha, &left_matrix,
                                             &right_matrix,
                                             character->isCorpse ());
					
					// calculates right_matrix and left_matrix
					pGrannyWrapper->glPushMatrix ();
                    pGrannyWrapper->glMultMatrixf (right_matrix.matrix);
                    pGrannyWrapper->glGetFloatv (cOgreGrannyWrapper::GL_MODELVIEW_MATRIX, right_matrix.matrix);
                    pGrannyWrapper->glPopMatrix ();

                    pGrannyWrapper->glPushMatrix ();
                    pGrannyWrapper->glMultMatrixf (left_matrix.matrix);
                    pGrannyWrapper->glGetFloatv (cOgreGrannyWrapper::GL_MODELVIEW_MATRIX, left_matrix.matrix);
                    pGrannyWrapper->glPopMatrix ();
					
                    for (int layer = 0; layer < 25; layer++)
                        {
                          cCharacterEquip *equip =
                            character->GetEquip (layer);
                          if (equip)
                              {
                                if ((m_kGame->GetPointedObj () == character->id ())
                                    || (pClient->GetEnemy () == character->id ()))
                                  pHueLoader.GetRGBHue ((int) character->
                                                         getHighlightColor (),
                                                         colr, colg, colb);
                                else
                                  pHueLoader.GetRGBHue (equip->hue (), colr,
                                                         colg, colb);
                                int anim = equip->anim ();

                                if ((Config::GetAOS())
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
                                        pGrannyLoader->Render (pGrannyWrapper,base_id | anim,
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
                                  pGrannyLoader->Render (pGrannyWrapper,base_id | anim,
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
							printf("mounttile != NULL\n");
                          mount = new cMount (mounttile->model ());
                          mountmodel = mount->GetMountType ();
                          if ((m_kGame->GetPointedObj () == character->id ())
                              || (pClient->GetEnemy () == character->id ()))
                            pHueLoader.GetRGBHue ((int) character->
                                                   getHighlightColor (), colr,
                                                   colg, colb);
                          else
                            pHueLoader.GetRGBHue (mounttile->hue (), colr,
                                                   colg, colb);
                          pGrannyLoader->Render ( pGrannyWrapper,mountmodel,
                                                 mountanim, curtime, light,
                                                 colr, colg, colb, alpha,
                                                 &left_matrix, &right_matrix);
                        }
					SDLScreen::GetInstance()->SetHue ();
			// TODO (#1#): Temporary fix for a bad memory leak...need to find a better solution in the future 

           if(mount)
            delete mount;
                  }
              character->setAnimtime (curtime);
              pGrannyWrapper->glPopMatrix ();
            }
      }
}


void Renderer3D::RenderDragModel ()
{PROFILE


  int blockx = pCamera.GetBlockX () * 8;
  int blocky = pCamera.GetBlockY () * 8;

  cStaticModel *model = StaticModelLoader::GetInstance()->getModel (drag_model);
  if (model)
    model->Render (dragposition[0] - blockx, dragposition[1] - blocky,
                   dragposition[2] * 0.1f, 255);

}


void Renderer3D::GrabDynamic (int x, int y, cDynamicObject ** r_object,
                              cCharacter ** r_character)
{PROFILE
  if (r_object)
    *r_object = NULL;
  if (r_character)
    *r_character = NULL;

  {
    // Obtain Cam Coords and PickRay

    float vecPickRayOrigin[3];
    float vecPickRayDir[3];

    //pCamera.PlaceGLMatrix ();  // ghoulsblade : deactivated
   // pCamera.CreatePickRay (x, y, vecPickRayOrigin, vecPickRayDir);  // ghoulsblade : deactivated

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
    dynamiclist_t *dynamics = pDynamicObjectList.GetList ();
    dynamiclist_t::iterator iter;

/*
// ghoulsblade : deactivated

    for (iter = dynamics->begin (); iter != dynamics->end (); iter++)
        {
          cDynamicObject *object = iter->second;
		  cStaticModel *model = StaticModelLoader::GetInstance()->getModel (object->model);
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
            pCharacterList.CheckRay (vecPickRayOrigin, vecPickRayDir,

                                      -deltax, -deltay, 0.0f, lambda);
          if (character)        // if character is nearer than found object
            if (lambda < distance)
                {
                  if ( Config::GetHideSelf() && character->id() == pClient->player_charid())
                   return; 
                  *r_character = character;
                  return;
                }
        }

    if (r_object)
      *r_object = picked_object;

		*/

  }
}


// Picks a 3D Position (ground or statics) by targetting on the screen in (x, y)
// max_z:  z-Maximum of the objects (used for hidden stages)
// cursor3d: contains result position after call
// art: contains result artid after call
void Renderer3D::GrabMousePosition (int x, int y, int max_z, int cursor3d[3], int & artid)
{PROFILE
  // Obtain Cam Coords and PickRay
  float vecPickRayOrigin[3];
  float vecPickRayDir[3];

  // Creates a Ray
 // pCamera.PlaceGLMatrix (); // ghoulsblade : deactivated
 // pCamera.CreatePickRay (x, y, vecPickRayOrigin, vecPickRayDir); // ghoulsblade : deactivated

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

  // dummy variables containing result objects
  sStaticObject *picked_object = NULL;
  int picked_x = -1;
  int picked_y = -1;
  int picked_z = 0;
  
  // current distance, in order to get the nearest item
  float distance = 1000000.0f;

/*
 // ghoulsblade : deactivated
  // Check blocks in view distance
  for (int y = -view_distance; y < view_distance; y++)
    for (int x = -view_distance; x < view_distance; x++)
        {
          cMapblock3D *block =
            reinterpret_cast <

            cMapblock3D * >(pMapbufferHandler.buffer ()->CreateBlock (blockx + x, blocky + y));
            
             if (block)
              {
                  float act_distance;
                  int z;
                  
                  // Check for static objects
                  sStaticObject *result =
                     block->CheckRay (vecPickRayOrigin, vecPickRayDir,
                                   act_distance, x * 8.0f, y * 8.0f, max_z);
                   if (result)  // we have hit a static item
                      if (act_distance < distance) // and it is nearer than any other
                          {
                            distance = act_distance; // note it
                            picked_object = result;
                            picked_x = -1;
                            picked_y = -1;
                          }
    
                  
                  
                   // Check for ground items
                   int id =
                      block->CheckRayOnGround (vecPickRayOrigin, vecPickRayDir,
                                             act_distance, x * 8.0f, y * 8.0f,
                                            z);
                   if (id != 255) // we have hit a ground item
                       {
                          if (act_distance < distance) // and it is nearer than any other
                            {
                               distance = act_distance; // note it
                               picked_object = NULL;
                               picked_x = (id % 8) + (blockx + x) * 8;
                               picked_y = (id / 8) + (blocky + y) * 8;
                               picked_z = z;
                            }
                       }
                   


              }
        }

  // and set results
  artid = 0;
  if (picked_object)
      {
        cursor3d[0] = picked_object->x + picked_object->blockx * 8;
        cursor3d[1] = picked_object->y + picked_object->blocky * 8;
        cursor3d[2] = picked_object->z;
        artid = picked_object->tileid;
      }

  if (picked_x != -1)
      {
        cursor3d[0] = picked_x;
        cursor3d[1] = picked_y;
        cursor3d[2] = picked_z;
      }
	  */
	  
	  // ghoulsblade ugly hack to get it to compile and work somehow, todo : recode picking using ogre ray-scene-querry
        cursor3d[0] = 0;
        cursor3d[1] = 0;
        cursor3d[2] = 0;
}

void Renderer3D::AddDynamic (cDynamicObject * object)
{PROFILE
}

void Renderer3D::DelDynamic (cDynamicObject * object)
{PROFILE
}
