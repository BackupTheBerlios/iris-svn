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
#include "Logger.h"
#include "Config.h"
#include "loaders/ArtLoader.h"
#include "renderer/Characters.h"
#include "Game.h"
#include "ogrewrapper.h"
#include "robinput.h"
#include <cassert>
#include <Ogre.h>
#include <OgreOverlay.h>
#include <OgreOverlayManager.h>
#include <OgrePanelOverlayElement.h>
#include <OgreTextAreaOverlayElement.h>


GUIHandler pUOGUI;

GUIHandler::GUIHandler()
{PROFILE
	mPanel_cursor = 0;
	mPanel_cursor_drag = 0;
	Reset();
}


GUIHandler::~GUIHandler()
{PROFILE
	DeInit();
}


void GUIHandler::Reset()
{PROFILE
	posx = 0;
	posy = 0;
	idcounter = 1;
	default_focusid = 0;
	zcounter = 1;
	focusid = 0;
	startflag = 0;
	search_index = 0;
	callback_OnDrag = NULL;
	callback_OnItemClick = NULL;

	for ( int i = 0; i < 9; i++ )
	{
		tex_cursors[i] = NULL;
	}

	drag_cursor = NULL;
	m_dragging = false;
}


void GUIHandler::DeInit()
{PROFILE
	ClearControls();
	
	// SAFE_DELETE_ARRAY( tex_cursors );

	/*
	for ( int i = 0; i < 9; i++ )
	{
		delete tex_cursors[i];
		tex_cursors[i] = NULL;
	}
	*/

	SAFE_DELETE( drag_cursor );

	refresh_times.clear();
	stack.Clear();
	//pGumpHandler.ClearTextures();

	Reset();
}


void GUIHandler::ClearControls( void )
{PROFILE
	ControlList_t::iterator iter;

	for ( iter = control_root.begin(); iter != control_root.end(); iter++ )
	{
		if ((*iter).second) (*iter).second->RemoveFromRenderQueue();
		SAFE_DELETE( (*iter).second );
	}

	control_root.clear();
	z_root.clear();
}


Control *GUIHandler::GetControl( int controlid )
{PROFILE
	ControlList_t::iterator iter;

	iter = control_root.find( (Uint32)controlid );
	if ( iter == control_root.end() )
	{
		return NULL;
	}
	else
	{
		return (*iter).second;
	}
}


void GUIHandler::AddControl( Control *control )
{PROFILE
	if ( !control )
	{
		return;
	}

	control->SetID( idcounter );
	control->SetZ( zcounter );
	control_root.insert( std::make_pair( (Uint32) idcounter, control ) );
	z_root.insert( std::make_pair( (Uint32) zcounter, control ) );
	if (control) control->AddToRenderQueue();
	idcounter++;
	zcounter++;
}


void GUIHandler::CloseWindow( int controlid )
{PROFILE
	Control *control = GetControl( controlid );
	if ( control )
	{
		control->DoOnClose();
		control_root.erase( controlid );
		z_root.erase( control->GetZ() );
		control->RemoveFromRenderQueue();
		// NOTE: Do _NOT_ try to delete control since we did not alocate memory for it.
	}
	if ( controlid == focusid )
	{
		focusid = 0;
		if ( default_focusid )
		{
			SetFocus( default_focusid );
		}
	}
}


void GUIHandler::SetFocus (int controlid)
{PROFILE
  if (focusid)
    ReleaseFocus (focusid);
  Control *control = GetControl (controlid);

  if (!control)
    return;

  control->SetFocus (true);
  focusid = controlid;

}

void GUIHandler::ReleaseFocus (int controlid)
{PROFILE
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

void GUIHandler::BringToFront( int controlid )
{PROFILE
	Control *control = GetControl( controlid );

	if ( control )
	{
		control->BringToFront();
		z_root.erase( control->GetZ() );
		z_root.insert( std::make_pair( (Uint32) zcounter, control ) );
		control->SetZ( zcounter );
		zcounter++;
	}
}

void GUIHandler::Draw (void)    //Uint16 hue)
{PROFILE
	/*
	ghoulsblade: deaktivated for ogre
  glDisable (GL_ALPHA_TEST);
  glDisable (GL_DEPTH_TEST);
  glDisable (GL_LIGHTING);
  glDisable (GL_CULL_FACE);

  glMatrixMode (GL_PROJECTION);
  glPushMatrix ();

  glLoadIdentity ();
  glOrtho (0, Config::GetWidth(), 0, Config::GetHeight(), -1, 1);

  glMatrixMode (GL_MODELVIEW);
  glPushMatrix ();

  glLoadIdentity ();
*/

  pCharacterList.RenderText ();

  // glTranslatef( (GLfloat)posx, (GLfloat)-posy, 0.0f ); // ghoulsblade : replaced by ogrewrapper
  cOgreWrapper::GetSingleton().GUI_SetOrigin(posx,-posy);

  ControlList_t::iterator iter;

  // 14� -> crash
  for (iter = z_root.begin (); iter != z_root.end (); iter++) {
    iter->second->GUIHandlerStep( &pGumpHandler ); // ghoulsblade : changed for ogre
  }

  /*
  // TODO : port me to ogre (PanelOverlayElement)
  if (drag_cursor)
      {
		// mPanel_cursor_drag = 0;
        glLoadIdentity ();
        glTranslatef( (GLfloat)cursorx - drag_cursor->GetRealWidth(), 
			(GLfloat)Config::GetHeight() - 1 - ( cursory - drag_cursor->GetRealHeight() ), 0.0f );
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
  */

	bool myshowcursor = false;
  if ( Config::GetCursor() == 1)
      {
        int cursor_id = 2;
        if ((Game::GetInstance()->click_mode () == CLICK_TARGET_ID)
            || (Game::GetInstance()->click_mode () == CLICK_TARGET_XYZ))
            {                   //std::cout << "Clickmode Target" << endl;
              cursor_id = 3;
            }
        if (getwarmode ())
          cursor_id = cursor_id - 2;
        if (tex_cursors[cursor_id])
            {
				if (!mPanel_cursor) {
					mPanel_cursor = static_cast<Ogre::PanelOverlayElement*>(Ogre::OverlayManager::getSingleton().createOverlayElement("Panel",cOgreWrapper::GetUniqueName()));
					cOgreWrapper::GetSingleton().mpOverlayCursor->add2D(mPanel_cursor);
					mPanel_cursor->setMetricsMode(Ogre::GMM_PIXELS);
					mPanel_cursor->show();
				}
				
				myshowcursor = true;
				if (!mPanel_cursor->isVisible()) mPanel_cursor->show();
				static int last_cursor_id = 0;
				static bool last_cursor_id_set = false;
				static int cursor_offset_x = -1;
				static int cursor_offset_y = -1;
				
				if (!last_cursor_id_set || last_cursor_id != cursor_id) {
					last_cursor_id_set = true;
					last_cursor_id = cursor_id;
					
					mPanel_cursor->setUV(0,1,1,0);
					//mPanel_cursor->setMaterialName("BaseWhiteNoLighting");
					mPanel_cursor->setMaterialName(tex_cursors[cursor_id]->GetGUIMaterial());
					float cw = tex_cursors[cursor_id]->GetWidth();
					float ch = tex_cursors[cursor_id]->GetHeight();
					if (ch < 0) ch = -ch;
					if (cw < 0) cw = -cw;
					mPanel_cursor->setDimensions(cw,ch);
					printf("cursor w=%f h=%f\n",(float)tex_cursors[cursor_id]->GetWidth(),(float)tex_cursors[cursor_id]->GetHeight());
				}
				cursorx = cInput::iMouse[0]; // todo : replace by mousemotionevent
				cursory = cInput::iMouse[1];
				
				/*
				if (cInput::bKeys[cInput::kkey_i]) --cursor_offset_y;
				if (cInput::bKeys[cInput::kkey_k]) ++cursor_offset_y;
				if (cInput::bKeys[cInput::kkey_j]) --cursor_offset_x;
				if (cInput::bKeys[cInput::kkey_l]) ++cursor_offset_x;
				
				//printf("cursor_offset %d,%d\n",cursor_offset_x,cursor_offset_y);
				*/
				
				mPanel_cursor->setPosition(cursorx+cursor_offset_x,cursory+cursor_offset_y);
				
				//mPanel_cursor = 0;
				/*
				// TODO : port me to ogre (PanelOverlayElement)
              glLoadIdentity ();
              glTranslatef( (GLfloat)cursorx, (GLfloat)Config::GetHeight() - 1 - cursory, 0.0f );
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
				*/

            }
      }
		if (!myshowcursor && mPanel_cursor && mPanel_cursor->isVisible()) mPanel_cursor->hide();
}


int GUIHandler::HandleMessage( gui_message *kMsg )
{PROFILE
	assert( kMsg );
	ControlList_t::reverse_iterator iter;
	for ( iter = z_root.rbegin(); iter != z_root.rend(); iter++ )
	{
		//if ( kMsg->type == MESSAGE_MOUSEMOTION )
		//{
			// Needs to be worked out (8 directions)
			//if ( kMsg->mousemotionevent.x < 100 )
			//{
			//	// Load Icon left
			//}
			//else
			//{
			//	if ( kMsg->mousemotionevent.x > Config::GetWidth() - 100 )
			//	{
			//		// Load Icon right
			//	}
			//	else
			//	{
			//		if ( kMsg->mousemotionevent.y > )
			//	}
			//}

		//	return true;
		//}
		if ( kMsg->type != MESSAGE_MOUSEMOTION || !m_dragging )
		{
			if ( (*iter).second->HandleMessage( kMsg ) )
			{
				return true;
			}
		}
	}

	return false;
}


void GUIHandler::HandleMessageQueues (void)
{PROFILE
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
			SDLEvent::KillApplication();
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

int GUIHandler::StartFlag (void)
{PROFILE
  int result = startflag;

  startflag = 0;
  return result;
}

void GUIHandler::SetDefaultFocus (Control * control)
{PROFILE
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
    Logger::WriteLine ("Exception in GUIHandler::SetDefaultFocus(Control *)",
                __FILE__, __LINE__, LEVEL_ERROR);
    return;
  }
}

ControlList_t *GUIHandler::GetControlList ()
{PROFILE
  return &control_root;
}


void GUIHandler::AddMessageToStack (gui_message * msg)
{PROFILE
  if (msg)
    stack.Push (*msg);
}

void GUIHandler::SetStartFlag (int flag)
{PROFILE
	startflag = flag;
}

void GUIHandler::SetQuitFlag()
{PROFILE
	SDLEvent::KillApplication();
}

void GUIHandler::Rewind (void)
{PROFILE
	search_index = 0;
}

Control *GUIHandler::GetNext (void)
{PROFILE
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
    Logger::WriteLine ("Exception in GUIHandler::GetNext(void)", __FILE__, __LINE__,
                LEVEL_ERROR);
    return NULL;
  }
}

void GUIHandler::SetPosition (int x, int y)
{PROFILE
  posx = x;
  posy = y;
}

int GUIHandler::GetX (void)
{PROFILE
  return posx;
}

int GUIHandler::GetY (void)
{PROFILE
  return posy;
}

Uint32 GUIHandler::RegisterGump (Texture * texture)
{PROFILE
  static Uint32 id = 65536;
  id++;
  pGumpHandler.AddTexture (id, texture);
  return id;
}

void GUIHandler::UnregisterGump (Uint32 id)
{PROFILE
  if (id >= 65536)
    pGumpHandler.DelTexture (id);
}


void GUIHandler::
OnDrag (void (*callback) (unsigned int itemid, unsigned int model))
{PROFILE
  callback_OnDrag = callback;
}

void GUIHandler::
OnItemClick (void (*callback) (unsigned int itemid, bool doubleclick))
{PROFILE
  callback_OnItemClick = callback;
}

void GUIHandler::LoadCursor (int id, int artid)
{PROFILE
	if ( ( id >= 0 ) && ( id < 9 ) )
	{
		SAFE_DELETE( tex_cursors[id] );
        /*delete tex_cursors[id];
        tex_cursors[id] = NULL;*/
		tex_cursors[id] = ArtLoader::GetInstance()->LoadArt( artid + 0x4000, true, true );
	}
}

void GUIHandler::SetCursorPos (int x, int y)
{PROFILE
  cursorx = x;
  cursory = y;
}

bool GUIHandler::FindDragContainer( int x, int y, Uint32 *containerid, int *drop_x, int *drop_y, Uint32 *charid )
{PROFILE
	ControlList_t::reverse_iterator iter;
	
	for ( iter = z_root.rbegin (); iter != z_root.rend (); iter++ )
	{
		if ( iter->second->MouseIsOver( x, y ) )
		{
			if ( containerid )
			{
				*containerid = iter->second->FindDragContainer( x, y, drop_x, drop_y, charid );
			}

          return true;
        }
	}

  return false;
}


void GUIHandler::LoadDragCursor( Uint16 model )
{PROFILE
	SAFE_DELETE( drag_cursor );

	if ( model )
	{
		drag_cursor = ArtLoader::GetInstance()->LoadArt( model + 16384, true, false, 0 );
	}
}


void GUIHandler::SetDragging (bool dragging)
{PROFILE
	m_dragging = dragging;
}

void GUIHandler::AdjustDropPosition (int &x, int &y)
{PROFILE
  if (drag_cursor)
      {
        x -= drag_cursor->GetRealWidth ();
        y -= drag_cursor->GetRealHeight ();
      }
}
