//
// File: Button.cpp
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

#include "gui/Button.h"
#include "gui/Container.h"
#include "Logger.h"
#include "Config.h"
#include <cassert>
#include "ogrewrapper.h"
#include <Ogre.h>
#include <OgreOverlay.h>
#include <OgreOverlayManager.h>
#include <OgrePanelOverlayElement.h>
#include <OgreTextAreaOverlayElement.h>




using namespace std;

Button::Button ()
{PROFILE
  Image::Image ();
  destPage = 0;
  isPageSelector = false;
  for (int i = 0; i < 3; i++)
    button_gump[i] = 0;
  pressed = 0;
  SetFlag (GUMPFLAG_MOVABLE, false);
  onclick_message.type = MESSAGE_NONE;
  callback_OnClick = NULL;
  control_type = CONTROLTYPE_BUTTON;
}

Button::Button (int x, int y)
{PROFILE
  Image::Image (x, y, 0);
  destPage = 0;
  isPageSelector = false;
  for (int i = 0; i < 3; i++)
    button_gump[i] = 0;
  SetPosition (x, y);
  pressed = false;
  mouseover = false;
  OnClick (NULL);
  SetFlag (GUMPFLAG_MOVABLE, false);
  control_type = CONTROLTYPE_BUTTON;

}

Button::~Button ()
{PROFILE
}

void Button::SetButton (int type, int gump)
{PROFILE
  if ((type >= 0) && (type < 3))
    this->button_gump[type] = gump;
}

int Button::GetButton (int type)
{PROFILE
  if ((type >= 0) && (type < 3))
    return this->button_gump[type];
  return 0;
}

void Button::SetClickMessage (gui_message * msg)
{PROFILE
  onclick_message = *msg;
}

void Button::OnClick (int (*callback) (Control * sender))
{PROFILE
  callback_OnClick = callback;
}

bool Button::IsPageSelector (void)
{PROFILE
  return isPageSelector;
}

void Button::SetPageSelector (bool sel)
{PROFILE
  isPageSelector = sel;
}

void Button::SetDestinationPage (int dp)
{PROFILE
  destPage = dp;
}

int Button::GetDestinationPage (void)
{PROFILE
  return destPage;
}




void Button::CreateGFX		(GumpHandler *gumps) {  PROFILE
	printf("Button::CreateGFX\n"); 
	if (mlRect.size() > 0) return;
	mlRect.push_back(Control::CreateRect());
}

void Button::DestroyGFX		() { PROFILE Control::DestroyGFX(); }

void Button::DrawStep 		(GumpHandler * gumps) { PROFILE
	if (mbNeedGfxUpdate) {
		Texture *texture;
		
		if (!pressed) {
			if (mouseover)
					texture = LoadGump (button_gump[1], gumps, false);
			else	texture = LoadGump (button_gump[0], gumps, false);
		} else {
			texture = LoadGump (button_gump[2], gumps, false);
		}
		
		if (texture) {
			if ((!GetWidth ()) || (!GetHeight ()))
				SetSize (texture->GetRealWidth (), texture->GetRealHeight ());
			UpdateRect(mlRect[0],GetX(),GetY(),GetWidth(),GetHeight(),texture,GetAlpha());
		} else { 
			mlRect[0]->hide();
		}
	}
}



void Button::Draw (GumpHandler * gumps)
{PROFILE
  Control::Draw (gumps);
  Texture *texture = NULL;

  if (!pressed)
      {
        if (mouseover)
          texture = LoadGump (button_gump[1], gumps, false);
        else
          texture = LoadGump (button_gump[0], gumps, false);
      }
  else
    texture = LoadGump (button_gump[2], gumps, false);

  if (texture)
      {
        SetSize (texture->GetRealWidth (), texture->GetRealHeight ());
        DrawRect (GetX (), GetY (), GetWidth (), GetHeight (), texture,
                  GetAlpha ());
      }
}

void Button::SetPressed (int pressed)
{PROFILE
  this->pressed = pressed;
}

int Button::GetPressed (void)
{PROFILE
  return pressed;
}

int Button::HandleMessage (gui_message * msg)
{PROFILE
  gui_message new_message;
  switch (msg->type)
      {
      case MESSAGE_MOUSEDOWN:
        if (MouseIsOver (msg->mouseevent.x, msg->mouseevent.y))
            {
				mbNeedGfxUpdate = true;
              pressed = true;
              if ((!GetFocus ()))
                  {
                    new_message.type = MESSAGE_SETFOCUS;
                    new_message.windowaction.controlid = GetID ();
                    stack.Push (new_message);
                  }
              return true;
            }
        else if (GetFocus ())
            {
              new_message.type = MESSAGE_RELEASEFOCUS;
              new_message.windowaction.controlid = GetID ();
              stack.Push (new_message);
            }
        break;
      case MESSAGE_MOUSEMOTION:
        if (MouseIsOver (msg->mousemotionevent.x, msg->mousemotionevent.y))
            {
				mbNeedGfxUpdate = true;
              if (msg->mousemotionevent.button)
                pressed = true;
              mouseover = true;
              return true;
            }
        else
            {
				mbNeedGfxUpdate = true;
              pressed = false;
              mouseover = false;
            }
        break;
      case MESSAGE_MOUSEUP:
        if (MouseIsOver (msg->mouseevent.x, msg->mouseevent.y) && pressed)
            {
              if (onclick_message.type != MESSAGE_NONE)
                stack.Push (onclick_message);

              stack.Push (CreateCallbackMessage (CALLBACK_ONCLICK));

				mbNeedGfxUpdate = true;
              pressed = false;
              mouseover = false;
              return true;
            }
		mbNeedGfxUpdate = true;
        pressed = false;
        mouseover = false;
        break;
      default:
        Control::HandleMessage (msg);
      }
  return false;
}


bool Button::CheckPixel (int x, int y)
{PROFILE
  Texture *texture = NULL;

  if (!pressed)
      {
        if (mouseover)
          texture = LoadGump (button_gump[1], &pGumpHandler, false);
        else
          texture = LoadGump (button_gump[0], &pGumpHandler, false);
      }
  else
    texture = LoadGump (button_gump[2], &pGumpHandler, false);

  if (texture)
      {
        return texture->CheckPixel (x, y);
      }
  return true;
}

void Button::DoOnClick (void)
{PROFILE
  if (callback_OnClick)
    callback_OnClick (this);
}
