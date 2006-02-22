/*! \file SDLEvent.h
 * \brief Handles SDL Events
 *
 * This class will handle all SDL input, it is a parser for SDL input functions.
 *
 * This class control input (keyboard).
 *
 * Copyright (©) Iris Team
 */

/*
 * Created by Gustav Nylander.
 * Last change: 22-02-06 (Nuno Ramiro)
 */

/*
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
 */

#ifndef _SDLEVENT_H_
#define _SDLEVENT_H_

#include "SDL/SDL.h"
#include "Macros.h"

///\brief SDLEvent class, handles all keyboard input.
class SDLEvent
{
public:
	/// Constructor
	SDLEvent();
	/// Destructor
	~SDLEvent();

	/// Handle events on our queue list.
	void PollEvent();
	/// Waits for a key press and handles it after we have pressed any key.
	void WaitEvent();

	/// Kills the Application (closes it).
	static void KillApplication();
	/// Gets Application status (if we have forced a close this will tell us).
	static bool GetStatus();

private:
	static bool m_bQuit;
	SDL_Event m_kEvent;
	int m_iLastTick;
	int m_iLastX, m_iLastY, m_iLastButton;
	unsigned int m_uiLastClick;
	int m_iClickDownX, m_iClickDownY;
	bool m_bIsDragging;

private:
	void HandleKeyPress( SDL_keysym *kKeysym );
	void HandleMouseMotion( SDL_MouseMotionEvent *kEvent );
	void HandleMouseDown( SDL_MouseButtonEvent *kEvent );
	void HandleEvent( SDL_Event kEvent, unsigned int uiCurrenttime );
	void HandleMovement( void );
	void HandleMacro( MacroEntry *kEntry );
};

#endif //_SDLEVENT_H_
