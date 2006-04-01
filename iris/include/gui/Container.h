//
// File: Container.h
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

#ifndef _CONTAINER_H_
#define _CONTAINER_H_

#include "Control.h"
#include "Logger.h"

// #include "../Fluid/mmgr.h"


class Container : public Control
{
public:
	Container();
	~Container();

	virtual void Draw( GumpHandler *kGumps );
	virtual int HandleMessage( gui_message *kMsg );

	void ClearControls();
	
	void AddControl( Control *kControl );
	void AddControl( Control *kControl, int iPage );
	
	Control *GetControl( int iControlId );
	Control *GetNext();
	ControlList_t *GetControlList();
	int GetCurrentPage() const;
	int GetPlayerID() const;
	int GetGumpID() const;

	void SetCurrentPage( int iCurrentPage );
	void SetFocus( int iControlId );
	void SetGumpID( int iId );
	void SetPlayerID( int iId );
	void SetShape( int iControlId );
	virtual void SetAlpha( unsigned char ucAlpha );

	void Rewind();

	virtual bool CheckPixel( int iX, int iY );

	virtual Uint32 FindDragContainer( int iX, int iY, int *iDropX, int *iDropY, Uint32 *uiCharId );
	//Control* RemoveControl(int controlid);
	//Control* RemoveControl(Control *control);

private:
	ControlList_t kControlList;
	int iIdCounter;
	int iSearchIndex;
	int iFocusId;
	int iCurrentPage;
	int iShapeId;
	int iGumpId;
	int iPlayerId;

private:
	int SendMessageToItems( gui_message *kMsg );
	void ReleaseFocus( int iControlId );
	void HandleMessageQueues();

};

#endif	// _CONTAINER_H_
