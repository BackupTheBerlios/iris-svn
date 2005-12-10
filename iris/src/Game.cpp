//
// File: Game.cpp
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


#include <iostream>
#include <stdio.h>
#include <math.h>
#include <cassert>

#include "Game.h"
#include "Debug.h"
#include "Exception.h"
#include "Config.h"
#include "Geometry.h"

#include "loaders/StaticModelLoader.h"
#include "loaders/UOMap.h"
//#include "loaders/CompressedMap.h"
#include "loaders/GroundTextures.h"
#include "loaders/GumpLoader.h"
#include "loaders/ArtLoader.h"
#include "loaders/FontLoader.h"
#include "loaders/HueLoader.h"
#include "loaders/SkillLoader.h"
#include "loaders/TileDataLoader.h"
#include "loaders/TileDataBuffer.h"
#include "loaders/StitchinLoader.h"
#include "loaders/ClilocLoader.h"
#include "loaders/ModelInfoLoader.h"
#include "loaders/VerdataLoader.h"
#include "loaders/MultisLoader.h"
#include "loaders/MapInfo.h"

#include "renderer/SDLScreen.h"
#include "renderer/TextureBuffer.h"
#include "renderer/DynamicObjects.h"
#include "renderer/Characters.h"

#include "renderer/particles/ParticleEngine.h"
#include "renderer/particles/ParticleLoader.h"

#include "renderer/3D/Renderer3D.h"
#include "renderer/3D/MapBuffer3D.h"
#include "renderer/3D/Light3D.h"

#include "net/Client.h"

#include "granny/GrannyLoader.h"

#include "gui/GUIHandler.h"
#include "gui/TextManager.h"

#include "csl/CSLHandler.h"
#include "Macros.h"

using namespace std;

extern SDLScreen *SDLscreen;

Game pGame;

Game::Game()
{
	m_MouseLastTick = SDL_GetTicks();
	m_AOSToolTip = false;

	// Default values
	pRenderer = NULL;
	m_paused = true;

	for ( int i = 0; i < 3; i++ )
		cursor3d[i] = 0;

	cursorx = 0;
	cursory = 0;
	button_left = false;
	button_right = false;

	cursor_character = 0;

	drag_id = 0;
	drag_model = 0;
	
	timer_func = "";
    timer = 0;

	m_click_mode = CLICK_NORMAL;

	callback_OnStatusDrag = NULL;
	callback_OnDynamicDrag = NULL;
	callback_OnAOSTooltip = NULL;

	pMapLoader = NULL;
	pGrannyLoader = NULL;
	pMultisLoader = NULL;
}

Game::~Game()
{
	//SiENcE: not needed
	//DeInit();
}


/* Callback handlers */
void OnAddDynamic( cDynamicObject * object )
{
	pGame.AddDynamic( object );
}

void OnDeleteDynamic( cDynamicObject * object )
{
	pGame.DelDynamic( object );
}

void OnAddCharacter( cCharacter * character )
{
	pGame.AddCharacter( character );
}

void OnDeleteCharacter( cCharacter * character )
{
	pGame.DelCharacter( character );
}


void Game::Init (void)
{
  //SiENcE: just to verify that everything is correct deinit; Logout is normally done via iris.csl script
  DeInit ();

  pMapLoader = NULL;
  pGrannyLoader = NULL;
  pMultisLoader = NULL;

  pTextManager = NULL;

  try
  {
    pDebug.Log ("SYS | Initializing Iris....");

    InitRenderer (nConfig::mulpath);

    pCamera.Reset ();
    pTextManager = new cTextManager;


    CreateSinTable ();

    pDynamicObjectList.Clear ();
    pDynamicObjectList.OnAdd (OnAddDynamic);
    pDynamicObjectList.OnDelete (OnDeleteDynamic);

    pCharacterList.Clear ();
    pCharacterList.OnAdd (OnAddCharacter);
    pCharacterList.OnDelete (OnDeleteCharacter);

    pDebug.Log ("    | -> loading particles");
    pParticleLoader.Init ("xml/particles.xml");

    pDebug.Log ("    | -> renderer");
    pRenderer = new Renderer3D ();

    pRenderer->Init ();

    SetPosition (nConfig::startx, nConfig::starty, nConfig::startz);
    SetPause (true);


    m_click_mode = CLICK_NORMAL;
  }
  catch (...)
  {
    DeInit ();
    throw;
  }
}


void Game::DeInit( void )
{
	pDebug.Log("SYS | Deinitializing Iris....");
	pParticleEngine.Reset();
	pParticleLoader.DeInit();

	if ( pRenderer )
		pRenderer->DeInit();
      
	DeInitRenderer();

	pCamera.Reset();

	pDynamicObjectList.Clear();
	pCharacterList.Clear();

	delete pRenderer;
	pRenderer = NULL;
	delete pTextManager;
	pTextManager = NULL;
}


void Game::InitRenderer (std::string mulpath)
{
  string mul_map0 = mulpath + "map0.mul";
  string mul_statics0 = mulpath + "statics0.mul";
  string mul_staidx0 = mulpath + "staidx0.mul";
  string mul_texmaps = mulpath + "texmaps.mul";
  string mul_texidx = mulpath + "texidx.mul";

  pMapLoader = NULL;
  pGrannyLoader = NULL;
  pMacroLoader = NULL;
  pMultisLoader = NULL;

      pDebug.Log ("    | -> mapinfo");
      pMapInfoLoader.Init ("xml/Maps.xml");

      pDebug.Log ("    | -> map");
      pMapLoader =
		  new UOMapLoader((char*) mul_map0.c_str(), (char*) mul_statics0.c_str(), (char*) mul_staidx0.c_str(), 0);
      
	  pDebug.Log ("    | -> ground textures");
	  pGroundTextureLoader.Init (mul_texmaps, mul_texidx);
      
	  pDebug.Log ("    | -> arts");
	  pArtLoader.Init (mulpath + "art.mul", mulpath + "artidx.mul");
    
	  pDebug.Log ("    | -> fonts");
	  pFontLoader.Init (mulpath + "fonts.mul");

	  pDebug.Log ("    | -> hues");
	  pHueLoader.Init (mulpath + "hues.mul");
    
	  pDebug.Log ("    | -> tiledata");
	  pTileDataLoader.Init (mulpath + "tiledata.mul");
      
	  pDebug.Log ("    | -> gumps");
	  pGumpLoader.Init (mulpath + "gumpart.mul", mulpath + "gumpidx.mul");
    
	  pDebug.Log ("    | -> skills");
	  pSkillLoader.Init (mulpath + "skills.mul", mulpath + "Skills.idx");

     pDebug.Log ("    | -> verdata");
     pVerdataLoader.Init (mulpath + "verdata.mul");

    pDebug.Log ("    | -> map buffer");
    pMapbufferHandler.Init (new cMapbuffer3D);

    pDebug.Log ("    | -> tiledata buffer");
    pTileDataBuffer.Clear ();

          pDebug.Log ("    | -> 3D character models");
          if (nConfig::aos)
              {
                pGrannyLoader =
                  new cGrannyLoader ("xml/granny_aos.xml", mulpath);
              }
          else
              {
                pGrannyLoader = new cGrannyLoader ("xml/granny.xml", mulpath);
              }


          pDebug.Log ("    | -> 3D static models");
          pStaticModelLoader.Init ("./data/models.uim");

    if (nConfig::aos)
        {

          pDebug.Log ("    | -> stitchinloader");
          pStitchinLoader.Init (mulpath + "Models/models.txt", mulpath + "stitchin.def");
        }
        
        
    if(nConfig::clilocs)  {
        pDebug.Log ("    | -> clilocs");

        pClilocLoader.Init (mulpath);
        }
  
    

    pDebug.Log ("    | -> model infos");
    pModelInfoLoader.Init ("xml/ModelsInfo.xml");

    pDebug.Log ("    | -> multis");
    pMultisLoader = new cMultisLoader (mulpath + "multi.mul", mulpath + "multi.idx");
    
    pDebug.Log("    | -> macros");
    pMacroLoader = new MacroLoader();

  
}

void Game::DeInitRenderer (void)
{
  if (pMapLoader)
  delete pMapLoader;
  pMapLoader = NULL;
  
  if (pMacroLoader)
  delete pMacroLoader;
  pMacroLoader = NULL;

  pGroundTextureLoader.DeInit ();
  pArtLoader.DeInit ();
  pFontLoader.DeInit ();
  pGumpLoader.DeInit ();
  pHueLoader.DeInit ();

  pTileDataLoader.DeInit ();

  pMapbufferHandler.DeInit ();
  
  pTextureBuffer.Clear ();
  pTileDataBuffer.Clear ();

  pVerdataLoader.DeInit ();

  if (pGrannyLoader)
  delete pGrannyLoader;
  pGrannyLoader = NULL;

  pSkillLoader.DeInit ();
  pStaticModelLoader.DeInit ();

  pLightManager.Clear ();

  pStitchinLoader.DeInit();
  pClilocLoader. DeInit ();
  pModelInfoLoader.DeInit ();
  pMapInfoLoader.DeInit ();

  if (pMultisLoader)
  delete pMultisLoader;
  pMultisLoader = NULL;
}


void Game::RenderScene( void )
{
	if ( !pRenderer )
		return;

	if ( m_paused )
	{
		SDLscreen->ClearScreen ();
		pUOGUI.Draw ();
		SDLscreen->DrawGL ();
		return;
	}

	//if (pCamera)
	if ( drag_id /*pCamera->CamHasChanged_DontReset()) */ )
	{
		//      GrabMousePosition(cursorx, cursory);
		if ( drag_id )
        {
			if ( drag_in_world )
				pRenderer->setDragModel( drag_model, cursor3d[0], cursor3d[1], cursor3d[2] );
			else
				pRenderer->setDragModel(0);
		}
	}
	pRenderer->RenderScene();
}

void Game::Handle( void )
{
	static int counter = 0;
	int new_tick = SDL_GetTicks();
    
    if(timer && !timer_func.empty())
    {
  
     if(new_tick >= timer){
          pCSLHandler.ExecuteFunction((char*)timer_func.c_str());
          timer = 0;
          timer_func == "";
         }
    }
    
	float time = ( new_tick - counter ) / 1000.0;

	if ( time > 1.0 )
		time = 1.0;
	if ( time <= 0.0f )
		time = 0.001f;

	counter = new_tick;

	pUOGUI.HandleMessageQueues ();

	pCharacterList.Handle (time);

	if ( !m_paused )
	{
		pParticleEngine.Handle ();
		pLightManager.Handle (new_tick);

		if ( pRenderer )
			pMapbufferHandler.buffer()->FreeBuffer (pRenderer->GetViewDistance () + 2);
	}

	if ( button_right )
		MoveToMouse();
}

Renderer *Game::GetRenderer( void )
{
	return pRenderer;
}

void Game::OnKeyPress (SDL_keysym * key)
{
}

void Game::HandleMouseMotion( SDL_MouseMotionEvent * event )
{
	cursorx = event->x;
	cursory = event->y;

	if ( !nConfig::mousemotion_timer )
		return;

	if ( SDL_GetTicks () - (unsigned int) m_MouseLastTick > (unsigned int)  nConfig::mousemotion_timer )
	{
		GrabMousePosition (cursorx, cursory);
		cDynamicObject *object;
		cCharacter *character;
		GrabDynamic (cursorx, cursory, &object, &character);

		if ( character )
			pointed_obj = character->id ();
		else if ( object )
			pointed_obj = object->id;
        else
		{
			if( nConfig::is_uox3 )
				pointed_obj = (dword)-1;
			else
				pointed_obj = 0;
		}

        if ( nConfig::aostooltips )
		{
			if ( callback_OnAOSTooltip )
			{
				if ( ( !m_AOSToolTip ) && ( character || object ) )
				{
					if(character){
					int count = character->aostooltips_count();
					 callback_OnAOSTooltip( character->id (), count, cursorx, cursory );
					}
					if(object){
					int count = object->aostooltips_count();
					 callback_OnAOSTooltip( object->id, count, cursorx, cursory );
					}
					m_AOSToolTip = true;
				}
				else
				{
					if (m_AOSToolTip)
					{
						callback_OnAOSTooltip( 0, 0, 0, 0 );
						m_AOSToolTip = false;
					}
				}
			}
		}
		m_MouseLastTick = SDL_GetTicks();
	}
}

void
  Game::HandleClick (int x, int y, unsigned int buttonstate,
                     bool double_click)
{
/*	float vecP[3];
	vecP[0] = -2.0f;
	vecP[1] = -2.0f;
	vecP[2] = -1.0f;
	int tx, ty;
	pCamera->GetRenderCoords(vecP, tx, ty); */
  if ((buttonstate == SDL_BUTTON_LEFT) && pClient)
      {

        cDynamicObject *object;
        cCharacter *character;
        switch (click_mode ())

            {
            case CLICK_NORMAL:
              GrabDynamic (x, y, &object, &character);
              if (object)
                  {
                    if (double_click)
                      pClient->Send_DoubleClick (object->id);
                    else
                        {
                          pClient->Send_Click (object->id);
                          if (nConfig::popup == 1)

                            pClient->Send_PopupRequest (object->id, x, y);
                        }
                  }
              else if (character)
                  {
                    if (double_click)
                        {
                          if (pClient->warmode ())
                            pClient->Send_AttackRequest (character->id ());
                          else
                            pClient->Send_DoubleClick (character->id ());
                        }
                    else
                        {
                          pClient->Send_Click (character->id ());
                          if (nConfig::popup == 1)
                            pClient->Send_PopupRequest (character->
                                                        id (), x, y);
                        }
                  }
              break;
            case CLICK_TARGET_ID:
              GrabDynamic (x, y, &object, &character);
              if (object)
                pClient->Send_Target ((Uint32) cursorid (),
                                      (Uint32) object->id);
              if (character)
                pClient->Send_Target ((Uint32) cursorid (),
                                      (Uint32) character->id ());
              m_click_mode = CLICK_NORMAL;
              break;
            case CLICK_TARGET_XYZ:
              GrabMousePosition (x, y);
              pClient->Send_Target ((Uint32) cursorid (), cursor3d[0],
                                    cursor3d[1], cursor3d[2]);
              m_click_mode = CLICK_NORMAL;
              break;
            }


      }

}

void Game::ItemClick2D (Uint32 id, bool double_click)
{
  switch (click_mode ())
      {
      case CLICK_NORMAL:
        if (double_click)
          pClient->Send_DoubleClick (id);
        else
          pClient->Send_Click (id);
        break;

      case CLICK_TARGET_ID:
        pClient->Send_Target ((Uint32) cursorid (), (Uint32) id);
        m_click_mode = CLICK_NORMAL;
        break;
      }
}


void Game::SetPosition (int x, int y, int z)
{
  pCamera.SetX (-(x % 8));
  pCamera.SetY (-(y % 8));
  pCamera.SetZ (-z);

  pCamera.SetBlockX (x / 8);
  pCamera.SetBlockY (y / 8);
}


void game_OnGameStart( void )
{
	pGame.SetPause( false );
}

void game_OnTeleport (int x, int y, int z)
{
	pGame.SetPosition( x, y, z );
}

void game_OnDragCancel( void )
{
	pGame.DragCancel();
}

void game_OnDrag( unsigned int id, unsigned int model )
{
	pGame.Drag( id, model );
}


void game_OnItemClick( unsigned int id, bool double_click )
{
	pGame.ItemClick2D( id, double_click );
}

void game_OnTarget( unsigned int cursorid, unsigned int type )
{
	if ( type == TARGET_ID )
	{
		// printf("Targetting Entity...\n"); 
		pGame.click_mode (CLICK_TARGET_ID);
	}
	else
	{
		// printf("Targetting Position...\n");
		pGame.click_mode (CLICK_TARGET_XYZ);
	}
	
	pGame.cursorid (cursorid);
}


void Game::Connect (void (*error_callback) (unsigned int error))
{
  Disconnect ();
  pClient = new cClient (error_callback);
  pClient->OnGameStart (game_OnGameStart);
  pClient->OnTeleport (game_OnTeleport);
  pClient->OnDragCancel (game_OnDragCancel);
  pClient->OnTarget (game_OnTarget);
        pUOGUI.OnDrag (game_OnDrag);
        pUOGUI.OnItemClick (game_OnItemClick);
}

void Game::Disconnect ()
{
  if (pClient)
      {
        pDebug.Log ("NET | Disconnecting....");
        delete pClient;
        pClient = NULL;
      }
}

void Game::Walk (Uint8 direction)
{
  if (pClient)
    pClient->Walk (direction);
}

void Game::Walk_Simple (Uint8 action)
{
  if (pClient)
    pClient->Walk_Simple (action);
}

void Game::GrabMousePosition( int x, int y, int max_z )
{
	if ( m_paused || !pRenderer )
		return;

	int cursor_char = 0;

	pRenderer->GrabMousePosition (x, y, max_z, cursor3d, &cursor_char);

	cursor_character = cursor_char;
}

void Game::GrabDynamic( int x, int y, cDynamicObject ** r_object, cCharacter ** r_character )
{
	if ( r_object )
		*r_object = NULL;
	if ( r_character )
		*r_character = NULL;
	if ( m_paused || !pRenderer )
		return;

	pRenderer->GrabDynamic (x, y, r_object, r_character);
}


void Game::HandleMouseDown (int x, int y, int button)
{
  if (paused ())
    return;
  cursorx = x;
  cursory = y;
  GrabMousePosition (cursorx, cursory);
  if (button == 1)
    button_left = true;
  if ((button == 2) || (button == 3))
    button_right = true;
}


void Game::HandleMouseUp (int x, int y, int button)
{
  if (paused ())
    return;
  cursorx = x;
  cursory = y;
  GrabMousePosition (cursorx, cursory);
  if (button == 1)
    button_left = false;
  if ((button == 2) || (button == 3))
    button_right = false;

}

void Game::MoveToMouse ()
{
  if (!pClient)
    return;


  if (SDL_GetModState () & (KMOD_LSHIFT | KMOD_RSHIFT))
    return;

  if (SDL_GetModState () & (KMOD_LALT | KMOD_RALT))
    return;

  int run_flag = 0;

  int px, py, pz;
  pClient->GetPlayerPosition (px, py, pz);

  if ((cursorx > nConfig::width * 0.80f) || (cursorx < nConfig::width * 0.20f)
      || (cursory > nConfig::height * 0.80f)
      || (cursory < nConfig::height * 0.20f))
    run_flag = 0x80;

  GrabMousePosition (cursorx, cursory, pz + 20);
  int dx = cursor3d[0] - px;
  int dy = cursor3d[1] - py;
  float angle = 0.0f;

  if (dx)
  {
   float m = (float) dy / (float) dx;
   angle = atan (m) / 3.14159f * 180.0f;
   if (dx < 0)
      angle = 180.0f + angle;
   if (angle < 0.0f)
      angle += 360.0f;
  }
  else if (dy > 0)
    angle = 90.0f;
  else if (dy < 0)
    angle = 270.0f;
  angle += 22.5f;

  int dir = (int) ((angle + 90.0f) / 45.0f);

  if (dir > 7)
    dir -= 8;
  
  if(nConfig::perspective == 0 && !pCamera.forceRotation())
    pClient->Walk (dir | run_flag);
}

void Game::HandleDrag( int x, int y )
{
	if ( m_paused || !pClient )
		return;

	if ( drag_id )
		return;

	cDynamicObject *object;
	cCharacter *character;
	GrabDynamic (x, y, &object, &character);

	if (object)
	{
		drag_id = object->id;
		drag_model = object->model;
		SetDragInWorld (true);
		pRenderer->setDragModel( drag_model, cursor3d[0], cursor3d[1], cursor3d[2] );
		pClient->Send_PickupRequest( object->id );
	}
  
	if ( character && callback_OnStatusDrag )
		callback_OnStatusDrag( character->id (), cursorx, cursory );

	pUOGUI.SetDragging( true );
}


void Game::DragCancel()
{
	drag_id = 0;
    pUOGUI.LoadDragCursor(0);
	if ( pRenderer )
		pRenderer->setDragModel(0);
	pUOGUI.SetDragging( false );
}

void Game::Drag( Uint32 id, Uint16 model )
{
	if ( m_paused || !pClient )
		return;

	if (drag_id)
		return;
	
	drag_id = id;

	drag_model = model;
	SetDragInWorld(false);
	cDynamicObject *obj = pDynamicObjectList.Get(id);

	if (obj->itemcount > 1)
	{
		Uint32 cont = obj->parent;
        
		if (callback_OnDynamicDrag)
		{
			callback_OnDynamicDrag( id, model, obj->itemcount, obj->x, obj->y, cont );
			return;
		}
	}

	pRenderer->setDragModel( drag_model, cursor3d[0], cursor3d[1], cursor3d[2] );
	pClient->Send_PickupRequest( id );
	pUOGUI.SetDragging( true );
}

void Game::OnStatusDrag( void (*callback) ( Uint32 charid, int mousex, int mousey ) )
{
	callback_OnStatusDrag = callback;
}

void Game::OnDynamicDrag( void (*callback) ( Uint32 id, Uint16 model, int count, int x, int y, Uint32 container ) )
{
	callback_OnDynamicDrag = callback;
}

void Game::OnAOSTooltip( void (*callback) ( Uint32 id, int count, int x, int y ) )
{
	callback_OnAOSTooltip = callback;
}

void Game::UpdateDragMode (int mousex, int mousey)
{
  if (drag_id)
      {
        GrabMousePosition (mousex, mousey);
        Uint32 containerid = 0;
        bool value =
          !pUOGUI.FindDragContainer (mousex, mousey, &containerid);
        value = value || (!containerid);  /* if gui element is not a container -> put to world */
        if (value)
          value = value && (!cursor_character); // if abough character, show 2D cursor


        SetDragInWorld (value);

      }
  else
    SetDragInWorld (true);
}

void Game::SetDragInWorld (bool value)
{

    if (value != drag_in_world)
        {
          if (value)            // True = Drag in World = Don't show any 2D Cursor
            pUOGUI.LoadDragCursor (0);
          else
            pUOGUI.LoadDragCursor (drag_model);
        }

  drag_in_world = value;
}

bool Game::CheckDragDrop( int mousex, int mousey )
{
	if (!drag_id)
		return false;

	Uint32 containerid = 0;
	Uint32 charid = 0;
	int drop_x = 0, drop_y = 0;


	assert (pClient);

	if ( pUOGUI.FindDragContainer( mousex, mousey, &containerid, &drop_x, &drop_y, &charid ) )
	{
		if ( charid )
		{
			pClient->Send_ItemEquipReq( drag_id, charid, drag_model );
		}
        else if ( containerid )
		{
			pUOGUI.AdjustDropPosition( drop_x, drop_y );
			pClient->Send_DropRequest( drag_id, drop_x, drop_y, 0, containerid );
		}
	}
	else
	{
		if (cursor_character)
		{
			pClient->Send_DropRequest( drag_id, 0, 0, 0, cursor_character );
		}
		else
		{
			pClient->Send_DropRequest( drag_id, cursor3d[0], cursor3d[1], cursor3d[2] );
		}
	}

	pUOGUI.SetDragging( false );

	pUOGUI.LoadDragCursor( 0 );

	if ( pRenderer )
		pRenderer->setDragModel( 0 );

	drag_id = 0;
	drag_model = 0;

	return true;
}


void Game::AddDynamic( cDynamicObject * object )
{
	if ( pRenderer )
		pRenderer->AddDynamic( object );
}

void Game::DelDynamic( cDynamicObject * object )
{
	if ( pRenderer )
		pRenderer->DelDynamic( object );
}

void Game::AddCharacter( cCharacter * character )
{
	if ( pRenderer )
		pRenderer->AddCharacter (character);
}

void Game::DelCharacter( cCharacter * character )
{
	if ( pRenderer )
		pRenderer->DelCharacter( character );
}

void Game::SendPickup( int id, int model, int count )
{
	drag_id = id;
	drag_model = model;
	pRenderer->setDragModel( drag_model, cursor3d[0], cursor3d[1], cursor3d[2] );
	pClient->Send_PickupRequest( id, count );
	pUOGUI.SetDragging( true );
}

void Game::DrawAOSTooltip( int id, int count, int x, int y )
{
	if ( callback_OnAOSTooltip )
		callback_OnAOSTooltip( id, count, cursorx, cursory );
}

void Game::SetTimerFunction(std::string function_name, int time)
{

 timer_func = function_name;
 timer = SDL_GetTicks() + time;

}
