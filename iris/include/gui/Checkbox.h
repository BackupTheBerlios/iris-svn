//
// File: Checkbox.h
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

#ifndef _CHECKBOX_H_
#define _CHECKBOX_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "SDL/SDL.h"
#include "Control.h"

////#include "../Fluid/mmgr.h"

#define CHECKBOX_CHECKED   1
#define CHECKBOX_UNCHECKED 0

//CHECKBOX X Y SELECTEDPIC UNSELECTEDPIC STARTSTATE IDNR

class Checkbox : public Control
{
 public:
  Checkbox(int checkedpic, int uncheckedpic);
  Checkbox(int x, int y, int checkedpic, int uncheckedpic, bool startstate = false);
  virtual ~Checkbox ();

  void SetGump(int type, int gump);
  void SetChecked(bool _checked);
  bool IsChecked(void);

  virtual int HandleMessage(gui_message * msg);
  virtual void Draw(GumpHandler * gumps);
  
   
 private:
  int __checked_gump;
  int __unchecked_gump;
  bool __checked;
};


#endif
