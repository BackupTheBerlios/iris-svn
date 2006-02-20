//
// File: SDLEvent.h
// Created by: Gustav Nylander - blobba@ultima-iris.de
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
#include "SDLEvent.h"
#include "renderer/SDLScreen.h"
#include "renderer/Renderer.h"
#include "renderer/Camera.h"
#include "net/Client.h"
#include "gui/GUIHandler.h"
#include "Logger.h"
#include <math.h>
#include "Config.h"
#include "Game.h"
#include <cassert>
#include "Config.h"
#include "Macros.h"
#include "csl/CSLHandler.h"

using namespace std;

extern SDLScreen *SDLscreen;

SDLEvent::SDLEvent ()
{
  lasttick = 0;
  last_click = 0;
  lastx = 0;
  lasty = 0;
  lastbutton = 0;
  quit = 0;
  clickdown_x = 0;
  clickdown_y = 0;
  dragging = false;
}

SDLEvent::~SDLEvent ()
{
  // TODO: put destructor code here

}

void SDLEvent::PollEvent ()
{

  /* handle the events in the queue */
  //SDL_WarpMouse( 320, 240 );

  unsigned int currenttime = SDL_GetTicks ();

  if (last_click)
    if ((currenttime - last_click) >= CLICK_TIME)
        {
          Game::GetInstance()->HandleClick (lastx, lasty, lastbutton, false);
//      printf("Single Click!\n");
          last_click = 0;
        }

  while (SDL_PollEvent (&event))
      {
        HandleEvent (event, currenttime);
      }
  HandleMovement ();

}

void SDLEvent::WaitEvent ()
{

  unsigned int currenttime = SDL_GetTicks ();

  while (!quit && SDL_WaitEvent (&event))
      {
        HandleEvent (event, currenttime);
      }

}
void SDLEvent::HandleEvent (SDL_Event event, unsigned int currenttime)
{
  gui_message msg;
  switch (event.type)
      {
/*
      case SDL_ACTIVEEVENT:
        //Something's happend with our focus
        //If we lost focus or we are iconified, we
        //shouldn't draw the screen
        if ( event.active.gain == 0 )
           isActive = false;
        else
            isActive = TRUE;
        break;
*/
      case SDL_VIDEORESIZE:
        // handle resize event
#ifndef WIN32
        // Do not reinitialize window in Win32
        SDLscreen->screen = SDL_SetVideoMode (event.resize.w,
                                              event.resize.h,
                                              Config::GetBPP(),
                                              SDLscreen->videoFlags);
        if (!SDLscreen->screen)
            {
              cerr << "Could not get a surface after resize: " <<
                SDL_GetError () << endl;
              exit (1);
            };
#endif
        SDLscreen->ResizeWindow (event.resize.w, event.resize.h);
        break;
      case SDL_KEYDOWN:
        /* handle key presses */
        HandleKeyPress (&event.key.keysym);
        break;
      case SDL_MOUSEMOTION:
        /* handle mouse movements */
        HandleMouseMotion (&event.motion);
        break;
      case SDL_MOUSEBUTTONDOWN:
        if ((event.button.button == 4))
          pCamera.ChangeZoom (-0.8f);    // Handle mouse wheel up
        if ((event.button.button == 5))
          pCamera.ChangeZoom (0.8f);     // Handle mouse wheel down

      case SDL_MOUSEBUTTONUP:
        if ((event.button.button == 4) || (event.button.button == 5))
          break;                // Do nothing on mouse wheel event


        if (event.type == SDL_MOUSEBUTTONDOWN)
          msg.type = MESSAGE_MOUSEDOWN;

        else
            {
              msg.type = MESSAGE_MOUSEUP;
              pUOGUI.SetDragging (false);
              dragging = false;
              if (Game::GetInstance()->CheckDragDrop (event.button.x, event.button.y))
                break;
            }
        msg.mouseevent.x = event.button.x - pUOGUI.GetX ();
        msg.mouseevent.y = event.button.y - pUOGUI.GetY ();
        msg.mouseevent.button = event.button.button;
        if (!pUOGUI.HandleMessage (&msg))
            {
              if (event.type == SDL_MOUSEBUTTONUP)
                  {
                    Game::GetInstance()->HandleMouseUp (event.button.x, event.button.y,
                                         event.button.button);
                    if (!last_click)
                        {
                          last_click = currenttime;
                          lastx = event.button.x;
                          lasty = event.button.y;
                          lastbutton = event.button.button;
                        }
                    else
                        {
                          if ((currenttime - last_click) < CLICK_TIME)
                              {
                                Game::GetInstance()->HandleClick (lastx, lasty,
                                                   event.button.button, true);
                                //printf("Double Click!\n");
                              }
                          last_click = 0;
                        }
                  }
              else
                  {
                    Game::GetInstance()->HandleMouseDown (event.button.x, event.button.y,
                                           event.button.button);
                    clickdown_x = event.button.x;
                    clickdown_y = event.button.y;
                  }
            }

        break;

      case SDL_QUIT:
        /* handle quit requests */
        quit = true;
        break;
      default:
        break;
      }
}

/* function to handle key press events */
void SDLEvent::HandleKeyPress (SDL_keysym * keysym)
{
  gui_message msg;
  MacroEntry * entry = pMacroLoader->GetMacro((int)keysym->sym);
  if(entry/* && (SDL_GetModState() == entry->keymod)*/)
  {
   if(SDL_GetModState() == entry->keymod)
     HandleMacro(entry);
   else
   {
     int macrocount = pMacroLoader->GetEntriesCount((int)keysym->sym);
     if( macrocount>1 )
     {
        for(int i = 0; i < macrocount; i++)
        {
           entry = pMacroLoader->GetMultiMacro((int)keysym->sym, i);
           if(entry && entry->keymod == SDL_GetModState())
              HandleMacro(entry);
        }
     }
   }
  }

  msg.keypressed.type = MESSAGE_KEYPRESSED;
  msg.keypressed.key = keysym->unicode & 0xFF;

  if (keysym->sym == SDLK_LEFT)
    msg.keypressed.key = SDLK_LEFT;
  else if (keysym->sym == SDLK_RIGHT)
    msg.keypressed.key = SDLK_RIGHT;

    if (pUOGUI.HandleMessage (&msg))
        {
          pUOGUI.HandleMessageQueues ();
          return;
        }

  // Check if the GUI can handle it
  Uint8 *keys;

  keys = SDL_GetKeyState (NULL);

  /* ESC key was pressed */
  if (keys[SDLK_ESCAPE] == SDL_PRESSED)
    quit = true;

  Renderer *pRenderer = Game::GetInstance()->GetRenderer ();
  if (keys[SDLK_a] == SDL_PRESSED && pRenderer)
      {
        pRenderer->FadeStatics (50, 1000);
      }

  if (keys[SDLK_s] == SDL_PRESSED && pRenderer)
      {
        pRenderer->FadeStatics (255, 1000);
      }

  Game::GetInstance()->OnKeyPress (keysym);
  /* F1 key was pressed this toggles fullscreen mode - does not work under windows currently */

#ifndef WIN32
  if (keys[SDLK_F1] == SDL_PRESSED)
    SDLscreen->ToggleFullScreen ();

  if (keys[SDLK_F12] == SDL_PRESSED)
    SDLscreen->ScreenSave ();
#endif
}

void SDLEvent::HandleMovement (void)
{


        Uint8 *keys;

        keys = SDL_GetKeyState (NULL);

        static Uint32 lasttick = 0;
        float factor = 30.0f;

        Uint32 tick = SDL_GetTicks ();

        if (lasttick)
          factor = (float) (tick - lasttick) / 150.0f;
        lasttick = tick;

        if (factor > 30.0f)
          factor = 30.0f;

/*      if(keys[SDLK_y] == SDL_PRESSED)
	pCamera.SetZ(pCamera.GetZ() + factor * 2.0f );

      if(keys[SDLK_x] == SDL_PRESSED)
	pCamera.SetZ(pCamera.GetZ() - factor * 2.0f ); */

        if (keys[SDLK_LEFT] == SDL_PRESSED)
            {
              Game::GetInstance()->Walk_Simple (WALK_LEFT);
            }
//      pCamera.Rotate(0.0, 0.0, 8.0 * factor);

        if (keys[SDLK_RIGHT] == SDL_PRESSED)
          Game::GetInstance()->Walk_Simple (WALK_RIGHT);
//      pCamera.Rotate(0.0, 0.0, -8.0 * factor);

        if (keys[SDLK_UP] == SDL_PRESSED)
          Game::GetInstance()->Walk_Simple (WALK_FORWARD);
//      pCamera.Move(factor);

        if (keys[SDLK_DOWN] == SDL_PRESSED)
          //pCamera.Move (-factor);
           {
            int dir =  pClient->player_character()->direction();
            (dir >= 4)?dir-=4:dir+=4;
                  pClient->Walk (dir);       
           }

}

void SDLEvent::HandleMouseMotion (SDL_MouseMotionEvent * event)
{
  Uint8 *keys;
  keys = SDL_GetKeyState (NULL);
  
  if (!event)
      {
        Logger::WriteLine ("Null event pointer in SDLEvent::HandleMouseMotion",
                    __FILE__, __LINE__, LEVEL_WARNING);
        return;
      }
      
   if(!pCamera.forceRotation())
    pUOGUI.SetCursorPos (event->x, event->y);

  Game::GetInstance()->UpdateDragMode (event->x, event->y);

  gui_message msg;

  msg.type = MESSAGE_MOUSEMOTION;
  msg.mousemotionevent.x = event->x - pUOGUI.GetX ();
  msg.mousemotionevent.y = event->y - pUOGUI.GetY ();
  msg.mousemotionevent.button = event->state;
  msg.mousemotionevent.relx = event->xrel;
  msg.mousemotionevent.rely = event->yrel;

  if (pUOGUI.HandleMessage (&msg))
      {
        pUOGUI.HandleMessageQueues ();
      }
  else
      {

        if ((event->state & (SDL_BUTTON (2) | SDL_BUTTON (3))))
            {
              /*if (SDL_GetModState () & KMOD_LSHIFT)
                  {
                    pCamera.Rotate (0.0f, 0.0f, -event->xrel / 3.0f);
                    pCamera.ChangeZoom (event->yrel * 0.05f);
                  }
              else if (SDL_GetModState () & (KMOD_RSHIFT))
                pCamera.Rotate (event->yrel / 3.0f, 0.0f,
                                 -event->xrel / 3.0f);*/
                                 
                
            if(event->state & SDL_BUTTON (1)){                 
                pCamera.Rotate (event->yrel / 3.0f, 0.0f,
                                 -event->xrel / 3.0f);
                pCamera.SetForceRotation(true); 
                } 
                
                float amount= pCamera.GetAngleZ() - -pClient->player_character ()->angle ();
                  
                if (keys[SDLK_UP] == SDL_PRESSED && Config::GetPerspective() == 1 ){
                  int wdirection=WALK_FORWARD;
                  if(amount > 215)
                   wdirection = WALK_LEFT;
                  else if (amount < 145)
                   wdirection = WALK_RIGHT;
                  if(wdirection == WALK_LEFT ||  wdirection == WALK_RIGHT)
                  Game::GetInstance()->Walk_Simple (wdirection); 
                  }                            
            }

        if (!(event->state & (SDL_BUTTON (2) | SDL_BUTTON (3))))
         pCamera.SetForceRotation(false);  




        if (event->state & SDL_BUTTON (1))
          if ((abs (clickdown_x - event->x) > 10)
              && (abs (clickdown_y - event->y) > 10) && !dragging)
              {
                Game::GetInstance()->HandleDrag (clickdown_x, clickdown_y);
                dragging = true;
              }
        Game::GetInstance()->HandleMouseMotion (event);

      }

}

void SDLEvent::HandleMacro(MacroEntry * entry)
{

      char integers[4][15];
     
      //std::vector<char*> ints;
      int numints = 0;
      std::vector<char*> strs;
      
      
      if(entry->parameters.size() == 4)
      {
        for(int i = 0; i < 4; i++)
        {
         
         if(entry->parameters.at(i)->type == 2)
         {
           sprintf(integers[i- strs.size()], "%d", entry->parameters.at(i)->int_value);
           numints++;
         }
        else if(entry->parameters.at(i)->type == 1)
         strs.push_back((char*)entry->parameters.at(i)->str_value.c_str());
        }
        
        if(numints == 4)
         pCSLHandler.ExecuteFunction((char*)entry->script_function.c_str(), integers[0], integers[1],integers[2],integers[3]);
        else if(numints == 3)
         pCSLHandler.ExecuteFunction((char*)entry->script_function.c_str(), integers[0], integers[1],integers[2],strs.at(0));
        else if(numints == 2)
         pCSLHandler.ExecuteFunction((char*)entry->script_function.c_str(), integers[0], integers[1],strs.at(0),strs.at(1));
        else if(numints == 1)
         pCSLHandler.ExecuteFunction((char*)entry->script_function.c_str(), integers[0], strs.at(0),strs.at(1),strs.at(2));
        else if(numints == 0)
         pCSLHandler.ExecuteFunction((char*)entry->script_function.c_str(), strs.at(0),strs.at(1),strs.at(2), strs.at(3)); 
      }
       else if(entry->parameters.size() == 3)
      {
        for(int i = 0; i < 3; i++)
        {
         if(entry->parameters.at(i)->type == 2)
         {
           sprintf(integers[i - strs.size()], "%d", entry->parameters.at(i)->int_value);
           numints++;
         }
        else if(entry->parameters.at(i)->type == 1)
         strs.push_back((char*)entry->parameters.at(i)->str_value.c_str());
        }
        
  
        if(numints == 3)
         pCSLHandler.ExecuteFunction((char*)entry->script_function.c_str(), integers[0], integers[1],integers[2]);
        else if(numints == 2)
         pCSLHandler.ExecuteFunction((char*)entry->script_function.c_str(), integers[0], integers[1],strs.at(0));
        else if(numints == 1)
         pCSLHandler.ExecuteFunction((char*)entry->script_function.c_str(), integers[0], strs.at(0),strs.at(1));
        else if(numints == 0)
         pCSLHandler.ExecuteFunction((char*)entry->script_function.c_str(), strs.at(0),strs.at(1),strs.at(2)); 
      }
      else if(entry->parameters.size() == 2)
      {
        for(int i = 0; i < 2; i++)
        {
         if(entry->parameters.at(i)->type == 2)
         { 
           sprintf(integers[i - strs.size()], "%d", entry->parameters.at(i)->int_value);
          
           numints++;
         }
        else if(entry->parameters.at(i)->type == 1)
         strs.push_back((char*)entry->parameters.at(i)->str_value.c_str());
        }
        
        if(numints == 2)
         pCSLHandler.ExecuteFunction((char*)entry->script_function.c_str(), integers[0], integers[1]);
        else if(numints == 1)
         pCSLHandler.ExecuteFunction((char*)entry->script_function.c_str(), integers[0], strs.at(0));
        else if(numints == 0)
         pCSLHandler.ExecuteFunction((char*)entry->script_function.c_str(), strs.at(0),strs.at(1)); 
      }
      else if(entry->parameters.size() == 1)
      {
      
         if(entry->parameters.at(0)->type == 2)
         {
           sprintf(integers[0], "%d", entry->parameters.at(0)->int_value);
           numints++;
         }
        else if(entry->parameters.at(0)->type == 1)
         strs.push_back((char*)entry->parameters.at(0)->str_value.c_str());
      
      
        if(numints == 1)
         pCSLHandler.ExecuteFunction((char*)entry->script_function.c_str(), integers[0]);
        else if(numints == 0)
         pCSLHandler.ExecuteFunction((char*)entry->script_function.c_str(), strs.at(0)); 
      }
        else if(entry->parameters.size() == 0)
         pCSLHandler.ExecuteFunction((char*)entry->script_function.c_str());
       
    strs.clear();
    numints = 0;

}

