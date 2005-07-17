//
// File: RadioButton.cpp
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

#include "gui/RadioButton.h"
#include "gui/Checkbox.h"
#include "gui/Container.h"
#include "Debug.h"
#include "Config.h"
#include "gui/GUIHandler.h"
using namespace std;

RadioButton::RadioButton (int checkedpic, int uncheckedpic):Checkbox (checkedpic,
          uncheckedpic)
{
  __group = 0;
}


RadioButton::RadioButton (int x, int y, int checkedpic, int uncheckedpic,
                          bool checked):Checkbox (x, y, checkedpic,
                                                  uncheckedpic, checked)
{
  __group = 0;
}


RadioButton::~RadioButton ()
{
}




void RadioButton::SetGroup (int groupid)
{
  __group = groupid;

}


int RadioButton::GetGroup (void)
{
  return __group;
}


int RadioButton::HandleMessage (gui_message * msg)
{
  if (!msg)
      {
        pDebug.Log ("NULL msg in RadioButton::HandleMessage(gui_message *)",
                    __FILE__, __LINE__, LEVEL_ERROR);
        return false;
      }

  switch (msg->type)
      {
      case MESSAGE_MOUSEDOWN:
        if (MouseIsOver (msg->mouseevent.x, msg->mouseevent.y))
            {
              if (!IsChecked ())
                  {
                    SetChecked (true);

                    //now uncheck all other radios in group:
                    ControlList_t *list = NULL;
                    ControlList_t::iterator iter;

                    if (parent == NULL)
                        {
                          list = pUOGUI.GetControlList ();

                        }
                    else
                        {
                          list = parent->GetControlList ();
                        }

                    for (iter = list->begin (); iter != list->end (); iter++)
                        {
                          if (dynamic_cast < RadioButton * >(iter->second) &&
                              iter->second->GetID () != GetID ())
                              {
                                RadioButton *r = (RadioButton *) iter->second;
                                if (r->GetGroup () == GetGroup ())
                                  r->SetChecked (false);
                              }
                        }
                    return true;
                  }
            }
        break;
      default:
        return Control::HandleMessage (msg);
      }
  return false;
}
