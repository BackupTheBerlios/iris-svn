//
// File: Checkbox.cpp
// Created by: Netzzwerg
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

#include "gui/Checkbox.h"
#include "Logger.h"
#include "Config.h"

using namespace std;


Checkbox::Checkbox (int checkedpic, int uncheckedpic)
{
  Control::Control ();
  SetGump (CHECKBOX_CHECKED, checkedpic);
  SetGump (CHECKBOX_UNCHECKED, uncheckedpic);
  __checked = false;
  SetFlag (GUMPFLAG_MOVABLE, false);
}


Checkbox::Checkbox (int x, int y, int checkedpic, int uncheckedpic,
                    bool checked)
{
  SetGump (CHECKBOX_CHECKED, checkedpic);
  SetGump (CHECKBOX_UNCHECKED, uncheckedpic);
  SetPosition (x, y);
  SetFlag (GUMPFLAG_MOVABLE, false);
  __checked = checked;
}


Checkbox::~Checkbox ()
{
}


void Checkbox::SetGump (int type, int gump)
{
  if (type == CHECKBOX_CHECKED)
      {
        this->__checked_gump = gump;
      }
  else if (type == CHECKBOX_UNCHECKED)
      {
        this->__unchecked_gump = gump;
      }
  else
      {
        char errorStr[512];
        sprintf (errorStr, "Illegal Checkbox state: %d", type);
        Logger::WriteLine (errorStr, __FILE__, __LINE__, LEVEL_ERROR);
      }
}


void Checkbox::SetChecked (bool checked)
{
  __checked = checked;

}

bool Checkbox::IsChecked (void)
{
  return __checked;
}


void Checkbox::Draw (GumpHandler * gumps)
{
  Control::Draw (gumps);
  Texture *texture = NULL;
  if (__checked)
      {
        texture = LoadGump (__checked_gump, gumps, false);
      }
  else
      {
        texture = LoadGump (__unchecked_gump, gumps, false);
      }
  if (!GetWidth () || !GetHeight ())
    SetSize (texture->GetRealWidth (), texture->GetRealHeight ());
  DrawRect (GetX (), GetY (), GetWidth (), GetHeight (), texture);

}


int Checkbox::HandleMessage (gui_message * msg)
{
  if (!msg)
      {
        Logger::WriteLine ("NULL msg in Checkbox::HandleMessage(gui_message *)",
                    __FILE__, __LINE__, LEVEL_ERROR);
        return false;
      }

  switch (msg->type)
      {
      case MESSAGE_MOUSEDOWN:
        if (MouseIsOver (msg->mouseevent.x, msg->mouseevent.y))
            {
              __checked = !__checked;
              return true;
            }
        break;
      default:
        Control::HandleMessage (msg);
      }
  return false;
}
