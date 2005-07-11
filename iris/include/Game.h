//
// File: Game.h
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

#ifndef _GAME_H_
#define _GAME_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "SDL/SDL.h"
#include "renderer/Renderer.h"
#include "net/Client.h"
#include "renderer/DynamicObjects.h"

#define CLICK_NORMAL     0
#define CLICK_TARGET_ID  1
#define CLICK_TARGET_XYZ 2

class Game
{
private:
 Renderer * renderer;
 bool m_paused;
 int cursor3d [3];
 int cursorx, cursory;
 
 Uint32 cursor_character;
 Uint32 cursor_object;
 
 bool button_left;
 bool button_right;
 
 Uint32 drag_id;
 Uint16 drag_model;
 bool drag_in_world;
     
 Uint32 pointed_obj;
 
 void GrabMousePosition(int x, int y, int max_z = 1000);
 void MoveToMouse();
 void GrabDynamic(int x, int y, cDynamicObject ** r_object, cCharacter ** r_character);
 int m_click_mode;
 Uint32 m_cursorid;
 
 void SetDragInWorld(bool value);
 

public:
    Game ();
   ~Game ();

   void Init(void);  /** Initialize a new game */
   void DeInit(void); /** Deinitialization */
   void ReInitRenderer(void); /** Uses DeinitRenderer / InitRenderer to restart the renderer */
   
   void InitRenderer(std::string mulpath); /** Initialize Renderer */
   void DeInitRenderer(void); /** Deinitialize */
   
   void RenderScene(void); /** Renders the whole scene */
   void Handle(void); /** Handles the scene */
   
   void Connect (void (*error_callback) (unsigned int error));
   void Disconnect (void);

   Renderer * GetRenderer(void);
   
   void SetPosition(int x, int y, int z);
   
   void OnKeyPress(SDL_keysym * key);
   void HandleMouseMotion(SDL_MouseMotionEvent * event);
   void HandleClick(int x, int y, unsigned int buttonstate, bool double_click);
   void HandleMouseDown(int x, int y, int button);
   void HandleMouseUp(int x, int y, int button);
   void HandleDrag(int x, int y);

   bool paused () { return m_paused; }
   void SetPause(bool pause) { m_paused = pause; }
   void Drag (Uint32 id, Uint16 model);
   void ItemClick2D (Uint32 id, bool double_click);
   void DragCancel ();

   void Walk (Uint8 direction);
   void Walk_Simple (Uint8 action);
   
   int click_mode () { return m_click_mode; }
   void click_mode (int value) { m_click_mode = value; }
   
   Uint32 cursorid () { return m_cursorid; }
   void cursorid (Uint32 value) { m_cursorid = value; }

   void OnStatusDrag(void (*callback) (Uint32 charid, int mousex, int mousey));
   void OnDynamicDrag(void (*callback)(Uint32 id, Uint16 model, int count, int x, int y, Uint32 container));
   void OnAOSTooltip(void (*callback) (Uint32 id, int count, int x, int y));
   
   void UpdateDragMode (int mousex, int mousey);
   bool CheckDragDrop (int mousex, int mousey);
   
   void AddDynamic (cDynamicObject * object);
   void DelDynamic (cDynamicObject * object);
   void AddCharacter (cCharacter * character);
   void DelCharacter (cCharacter * character);

   Uint32 GetPointedObj(){return pointed_obj;}
   void SendPickup(int id, int model, int count);
   void DrawAOSTooltip(int id, int count, int x, int y);
   
protected:
   void (*callback_OnStatusDrag) (Uint32 charid, int mousex, int mousey);
   void (*callback_OnDynamicDrag) (Uint32 id, Uint16 model, int count, int x, int y, Uint32 container);
   void (*callback_OnAOSTooltip) (Uint32 id, int count, int x, int y);
};

extern	Game	pGame;

#endif //_GAME_H_
