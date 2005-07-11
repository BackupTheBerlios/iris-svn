//
// File: GUIHandler.cpp
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

#include "gui/GUIHandler.h"
#include "gui/Button.h"
#include "gui/Container.h"
#include "gui/InputField.h"
#include "Debug.h"
#include "Config.h"
#include "loaders/ArtLoader.h"
#include "renderer/Characters.h"
#include "Game.h"
#include <cassert>

using namespace std;

GUIHandler *pUOGUI = NULL;

GUIHandler::GUIHandler ()
{
  posx = 0;
  posy = 0;
  idcounter = 1;
  default_focusid = 0;
  zcounter = 1;
  focusid = 0;
  quitflag = 0;
  startflag = 0;
  search_index = 0;
  callback_OnDrag = NULL;
  callback_OnItemClick = NULL;
  for (int i = 0; i < 9; i++)
    tex_cursors[i] = NULL;
  pGumpHandler = &gumps;
  drag_cursor = NULL;
  m_dragging = false;
}

GUIHandler::~GUIHandler ()
{
  ClearControls ();
  for (int i = 0; i < 9; i++)
      {
        delete tex_cursors[i];
        tex_cursors[i] = NULL;
      }

  if (drag_cursor)
    delete drag_cursor;
  drag_cursor = NULL;

  pGumpHandler = NULL;
}

void GUIHandler::ClearControls (void)
{
  ControlList_t::iterator iter;

  for (iter = control_root.begin (); iter != control_root.end (); iter++)
    delete (*iter).second;

  control_root.clear ();
  z_root.clear ();

}


void GUIHandler::ClearControlsSafe (void)
{
}

Control *GUIHandler::GetControl (int controlid)
{
  ControlList_t::iterator iter;

  iter = control_root.find ((Uint32) controlid);
  if (iter == control_root.end ())
    return NULL;
  else
    return (*iter).second;
}

void GUIHandler::AddControl (Control * control)
{
  if (!control)
    return;
  control->SetID (idcounter);
  control->SetZ (zcounter);
  control_root.insert (make_pair ((Uint32) idcounter, control));
  z_root.insert (make_pair ((Uint32) zcounter, control));
  idcounter++;
  zcounter++;
}

void GUIHandler::CloseWindow (int controlid)
{
  Control *control = GetControl (controlid);
  if (control)
      {
        control->DoOnClose ();
        control_root.erase (controlid);
        z_root.erase (control->GetZ ());
        delete control;
      }
  if (controlid == focusid)
      {
        focusid = 0;
        if (default_focusid)
          SetFocus (default_focusid);
      }
}

void GUIHandler::SetFocus (int controlid)
{
  if (focusid)
    ReleaseFocus (focusid);
  Control *control = GetControl (controlid);

  if (!control)
    return;

  control->SetFocus (true);
  focusid = controlid;

}

void GUIHandler::ReleaseFocus (int controlid)
{
  Control *control = GetControl (controlid);

  if (control)
    control->SetFocus (false);

  if (controlid == focusid)
      {
        focusid = 0;
        if (default_focusid)
          SetFocus (default_focusid);
      }
}

void GUIHandler::BringToFront (int controlid)
{
  Control *control = GetControl (controlid);

  if (control)
      {
        z_root.erase (control->GetZ ());
        z_root.insert (make_pair ((Uint32) zcounter, control));
        control->SetZ (zcounter);
        zcounter++;
      }
}

void GUIHandler::Draw (void)    //Uint16 hue)
{
  glDisable (GL_ALPHA_TEST);
  glDisable (GL_DEPTH_TEST);
  glDisable (GL_LIGHTING);
  glDisable (GL_CULL_FACE);

  glMatrixMode (GL_PROJECTION);
  glPushMatrix ();

  glLoadIdentity ();
  glOrtho (0, nConfig::width, 0, nConfig::height, -1, 1);

  glMatrixMode (GL_MODELVIEW);
  glPushMatrix ();

  glLoadIdentity ();

  if (pCharacterList)
    pCharacterList->RenderText ();

  glTranslatef (posx, -posy, 0.0f);

  ControlList_t::iterator iter;
  for (iter = z_root.begin (); iter != z_root.end (); iter++)
    iter->second->Draw (&gumps);

  if (drag_cursor)
      {
        glLoadIdentity ();
        glTranslatef (cursorx - drag_cursor->GetRealWidth (),
                      nConfig::height - 1 - (cursory -
                                             drag_cursor->GetRealHeight ()),
                      0.0f);
        glBindTexture (GL_TEXTURE_2D, drag_cursor->GetGLTex ());
        glBegin (GL_QUADS);
        glTexCoord2f (0.0f, 0.0f);
        glVertex3f (0.0f, -(float) drag_cursor->GetHeight (), 0.0f);
        glTexCoord2f (1.0f, 0.0f);
        glVertex3f ((float) drag_cursor->GetWidth (),
                    -(float) drag_cursor->GetHeight (), 0.0f);
        glTexCoord2f (1.0f, 1.0f);
        glVertex3f ((float) drag_cursor->GetWidth (), 0.0f, 0.0f);
        glTexCoord2f (0.0f, 1.0f);
        glVertex3f (0.0f, 0.0f, 0.0f);
        glEnd ();
      }

  if (nConfig::cursor == 1)
      {
        int cursor_id = 2;
        if ((pGame.click_mode () == CLICK_TARGET_ID)
            || (pGame.click_mode () == CLICK_TARGET_XYZ))
            {                   //std::cout << "Clickmode Target" << endl;
              cursor_id = 3;
            }
        if (getwarmode ())
          cursor_id = cursor_id - 2;
        if (tex_cursors[cursor_id])
            {
              glLoadIdentity ();
              glTranslatef (cursorx, nConfig::height - 1 - cursory, 0.0f);
              glBindTexture (GL_TEXTURE_2D,
                             tex_cursors[cursor_id]->GetGLTex ());
              glBegin (GL_QUADS);
              glTexCoord2f (0.0f, 0.0f);
              glVertex3f (0.0f, -(float) tex_cursors[cursor_id]->GetHeight (),
                          0.0f);
              glTexCoord2f (1.0f, 0.0f);
              glVertex3f ((float) tex_cursors[cursor_id]->GetWidth (),
                          -(float) tex_cursors[cursor_id]->GetHeight (),
                          0.0f);
              glTexCoord2f (1.0f, 1.0f);
              glVertex3f ((float) tex_cursors[cursor_id]->GetWidth (), 0.0f,
                          0.0f);
              glTexCoord2f (0.0f, 1.0f);
              glVertex3f (0.0f, 0.0f, 0.0f);
              glEnd ();

            }
      }

  // Restore 3d Mode
  glMatrixMode (GL_PROJECTION);
  glPopMatrix ();

  glMatrixMode (GL_MODELVIEW);
  glPopMatrix ();

  glEnable (GL_LIGHTING);
  glEnable (GL_DEPTH_TEST);
  glEnable (GL_ALPHA_TEST);
  glEnable (GL_CULL_FACE);
}

int GUIHandler::HandleMessage (gui_message * msg)
{
  assert (msg);
  ControlList_t::reverse_iterator iter;
  for (iter = z_root.rbegin (); iter != z_root.rend (); iter++)
    if (!m_dragging || (msg->type != MESSAGE_MOUSEMOTION))
      if ((*iter).second->HandleMessage (msg))
        return true;

  return false;
}

void GUIHandler::HandleMessageQueues (void)
{
  Control *control;
  Container *container;

  gui_message msg;

  std::list < Uint32 >::iterator refresh_iter;
  Uint32 currenttime = SDL_GetTicks ();
  bool do_refresh = false;
  for (refresh_iter = refresh_times.begin ();
       refresh_iter != refresh_times.end ();)
      {
        if (*refresh_iter < currenttime)
            {
              std::list < Uint32 >::iterator nextiter = refresh_iter;
              nextiter++;
              refresh_times.erase (refresh_iter);
              refresh_iter = nextiter;
              do_refresh = true;
            }
        else
          refresh_iter++;
      }

  if (do_refresh)
      {
        msg.type = MESSAGE_DOREFRESH;
        msg.refresh.time = currenttime;
        stack.Push (msg);
      }

  ControlList_t::iterator iter;
  for (iter = control_root.begin (); iter != control_root.end (); iter++)
    while (iter->second->stack.Pop (&msg))
      stack.Push (msg);

  while (stack.Pop (&msg))
    switch (msg.type)
        {
        case MESSAGE_CLOSEGUMP:
          CloseWindow (msg.windowaction.controlid);
          break;
        case MESSAGE_SETFOCUS:
          SetFocus (msg.windowaction.controlid);
          break;
        case MESSAGE_RELEASEFOCUS:
          ReleaseFocus (msg.windowaction.controlid);
          break;
        case MESSAGE_BRINGTOFRONT:
          BringToFront (msg.windowaction.controlid);
          break;
        case MESSAGE_QUIT:
          quitflag = -1;
          break;
        case MESSAGE_STARTGAME:
          startflag = -1;
          break;
        case MESSAGE_CALLBACK:
          if (msg.callback.containerid)
              {
                control = GetControl (msg.callback.containerid);
                if (control)
                    {
                      if (control->getType () == CONTROLTYPE_CONTAINER)
                          {
                            container = (Container *) control;
                            control = container->GetControl (msg.callback.id);
                          }
                      else
                        control = NULL;
                    }
              }
          else
            control = GetControl (msg.callback.id);
          if (control)
            switch (msg.callback.callback_type)
                {
                case CALLBACK_ONCLICK:
                  if (control->getType () == CONTROLTYPE_BUTTON)
                    ((Button *) control)->DoOnClick ();
                  break;
                case CALLBACK_ONKEYPRESS:
                  if (control->getType () == CONTROLTYPE_INPUTFIELD)
                    ((InputField *) control)->DoOnKeyPress (msg.callback.key);
                  break;
                case CALLBACK_ONCLOSE:
                  control->DoOnClose ();
                  break;
                case CALLBACK_ONMOUSEUP:
                  control->DoOnMouseUp ();
                  break;
                case CALLBACK_ONMOUSEDOWN:
                  control->DoOnMouseDown ();
                  break;
                }
          break;
        case MESSAGE_ONDRAGITEM:
          if (callback_OnDrag)
            callback_OnDrag (msg.dragitem.itemid, msg.dragitem.model);
          drag_id = msg.dragitem.itemid;
          drag_model = msg.dragitem.model;

          break;
        case MESSAGE_ONCLICKITEM:
          if (callback_OnItemClick)
            callback_OnItemClick (msg.onclickitem.itemid,
                                  msg.onclickitem.doubleclick);
          break;
        case MESSAGE_REFRESHREQUEST:
          refresh_times.push_back (msg.refresh.time);
          break;
        default:
          HandleMessage (&msg);
        }

}

int GUIHandler::QuitFlag (void)
{
  int result = quitflag;

  quitflag = 0;
  return result;
}

int GUIHandler::StartFlag (void)
{
  int result = startflag;

  startflag = 0;
  return result;
}

void GUIHandler::SetDefaultFocus (Control * control)
{
  try
  {
    if (control)
      default_focusid = control->GetID ();
    else
      default_focusid = 0;

    if (default_focusid)
      SetFocus (default_focusid);
  }
  catch (...)
  {
    pDebug.Log ("Exception in GUIHandler::SetDefaultFocus(Control *)",
                __FILE__, __LINE__, LEVEL_ERROR);
    return;
  }
}

ControlList_t *GUIHandler::GetControlList ()
{
  return &control_root;
}


void GUIHandler::AddMessageToStack (gui_message * msg)
{
  if (msg)
    stack.Push (*msg);
}

void GUIHandler::SetStartFlag (int flag)
{
  startflag = flag;
}

void GUIHandler::SetQuitFlag (int flag)
{
  quitflag = flag;
}

void GUIHandler::Rewind (void)
{
  search_index = 0;
}

Control *GUIHandler::GetNext (void)
{
  ControlList_t::iterator iter;

  try
  {
    if ((search_index >= 0)
        && ((unsigned int) search_index < control_root.size ()))
        {
          unsigned int i = search_index;
          search_index++;
          for (iter = control_root.begin ();
               (iter != control_root.end ()) && (i > 0); iter++)
            i--;
          if (iter != control_root.end ())
            return iter->second;
        }
    return NULL;
  }
  catch (...)
  {
    pDebug.Log ("Exception in GUIHandler::GetNext(void)", __FILE__, __LINE__,
                LEVEL_ERROR);
    return NULL;
  }
}

void GUIHandler::SetPosition (int x, int y)
{
  posx = x;
  posy = y;
}

int GUIHandler::GetX (void)
{
  return posx;
}

int GUIHandler::GetY (void)
{
  return posy;
}

Uint32 GUIHandler::RegisterGump (Texture * texture)
{
  static Uint32 id = 65536;
  id++;
  gumps.AddTexture (id, texture);
  return id;
}

void GUIHandler::UnregisterGump (Uint32 id)
{
  if (id >= 65536)
    gumps.DelTexture (id);
}


void GUIHandler::
OnDrag (void (*callback) (unsigned int itemid, unsigned int model))
{
  callback_OnDrag = callback;
}

void GUIHandler::
OnItemClick (void (*callback) (unsigned int itemid, bool doubleclick))
{
  callback_OnItemClick = callback;
}

void GUIHandler::LoadCursor (int id, int artid)
{
  if ((id >= 0) && (id < 9))
      {
        delete tex_cursors[id];
        tex_cursors[id] = NULL;
        tex_cursors[id] = pArtLoader.LoadArt (artid + 0x4000, true, true);
      }
}

void GUIHandler::SetCursorPos (int x, int y)
{
  cursorx = x;
  cursory = y;
}

bool GUIHandler::FindDragContainer (int x, int y, Uint32 * containerid,
                                    int *drop_x, int *drop_y, Uint32 * charid)
{
  ControlList_t::reverse_iterator iter;
  for (iter = z_root.rbegin (); iter != z_root.rend (); iter++)
    if (iter->second->MouseIsOver (x, y))
        {
          if (containerid)
            *containerid =
              iter->second->FindDragContainer (x, y, drop_x, drop_y, charid);

          return true;
        }

  return false;
}


void GUIHandler::LoadDragCursor (Uint16 model)
{
  if (drag_cursor)
    delete drag_cursor;
  drag_cursor = NULL;

  if (model)
      {
        drag_cursor = pArtLoader.LoadArt (model + 16384, true, false, 0);
      }
}


void GUIHandler::SetDragging (bool dragging)
{
  m_dragging = dragging;
}

void GUIHandler::AdjustDropPosition (int &x, int &y)
{
  if (drag_cursor)
      {
        x -= drag_cursor->GetRealWidth ();
        y -= drag_cursor->GetRealHeight ();
      }
}
