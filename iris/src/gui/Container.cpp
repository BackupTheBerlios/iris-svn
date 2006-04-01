//
// File: Container.cpp
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

#include "gui/Container.h"


Container::Container() : iIdCounter( 1 ), iSearchIndex( 0 ), iFocusId( 0 ), iCurrentPage( 0 ), iShapeId( 0 )
{
	Control::Control();
	control_type = CONTROLTYPE_CONTAINER;
}


Container::~Container()
{
	ClearControls();
}


void Container::Draw( GumpHandler *kGumps )
{
	Control::Draw( kGumps );
	glPushMatrix();
	glTranslatef( (GLfloat)GetX(), (GLfloat)-GetY(), 0.0 );

	ControlList_t::iterator iter;
	for ( iter = kControlList.begin(); iter != kControlList.end(); iter++ )
	{
		if ( iter->second->GetPage() == iCurrentPage || iter->second->GetPage() == 0 )
		{
			iter->second->Draw( kGumps );
		}
	}

	glPopMatrix();
}


int Container::HandleMessage( gui_message *kMsg )
{
	gui_message new_message;

	if ( !kMsg )
	{
		Logger::WriteLine( "NULL msg in Container::HandleMessage(gui_message *)",
			__FILE__, __LINE__, LEVEL_ERROR );
		return false;
	}

	switch ( kMsg->type )
	{
	case MESSAGE_MOUSEDOWN:
	case MESSAGE_MOUSEUP:
		new_message = *kMsg;
		new_message.mouseevent.x -= GetX();
		new_message.mouseevent.y -= GetY();

		if ( SendMessageToItems( &new_message ) )
		{
			HandleMessageQueues();
			return true;
		}
		break;

	case MESSAGE_MOUSEMOTION:
		new_message = *kMsg;
		new_message.mousemotionevent.x -= GetX();
		new_message.mousemotionevent.y -= GetY();

		if ( SendMessageToItems( &new_message ) )
		{
			HandleMessageQueues();
			return true;
		}
		break;

	case MESSAGE_KEYPRESSED:
	case MESSAGE_REBUILDITEMCONTAINER:
	case MESSAGE_ADDCONTAINERITEM:
	case MESSAGE_UPDATEPAPERDOLL:
	case MESSAGE_DOREFRESH:
		SendMessageToItems( kMsg );
	}

	int result = Control::HandleMessage( kMsg );

	HandleMessageQueues();

	return result;
}


void Container::ClearControls()
{
	ControlList_t::iterator iter;
	for ( iter = kControlList.begin(); iter != kControlList.end(); iter++ )
	{
		iter->second->DoOnClose();
		SAFE_DELETE( (*iter).second );
	}

	kControlList.clear();
}


void Container::AddControl( Control *kControl )
{
	kControl->SetID( iIdCounter | ( GetID() << 16 ) );
	kControl->SetParent( this );
	kControlList.insert( std::make_pair( (Uint32)iIdCounter | ( GetID() << 16 ), kControl ) );
	iIdCounter++;
}

void Container::AddControl( Control *kControl, int iPage )
{
	kControl->SetPage( iPage );
	AddControl( kControl );
}


Control *Container::GetControl( int iControlId )
{
	ControlList_t::iterator iter;

	iter = kControlList.find( (Uint32)iControlId );
	if ( iter == kControlList.end() )
	{
		return NULL;
	}
	
	return (*iter).second;
}


Control *Container::GetNext()
{
	ControlList_t::iterator iter;
	if ( (iSearchIndex >= 0) && ((unsigned int)iSearchIndex < kControlList.size()) )
	{
		unsigned int i = iSearchIndex;
		iSearchIndex++;
		for ( iter = kControlList.begin(); (iter != kControlList.end()) && (i > 0); iter++ )
		{
			i--;
		}
		if ( iter != kControlList.end() )
		{
			return iter->second;
		}
	}

	return NULL;
}


ControlList_t *Container::GetControlList()
{
	return &kControlList;
}


int Container::GetCurrentPage() const
{
	return iCurrentPage;
}


int Container::GetPlayerID() const
{
	return iPlayerId;
}


int Container::GetGumpID() const
{
	return iGumpId;
}


void Container::SetCurrentPage( int iCurrentPage )
{
	iCurrentPage = iCurrentPage;
}


void Container::SetFocus( int iControlId )
{
	ReleaseFocus( iFocusId );
	Control *control = GetControl( iControlId );

	if ( !control )
	{
		return;
	}
	control->SetFocus( true );
	iFocusId = iControlId;
}


void Container::SetGumpID( int iId )
{
	iGumpId = iId;
}


void Container::SetPlayerID( int iId )
{
	iPlayerId = iId;
}


void Container::SetShape( int iControlId )
{
	iShapeId = iControlId;
}


void Container::SetAlpha( unsigned char ucAlpha )
{
	Control::SetAlpha( ucAlpha );
	ControlList_t::iterator iter;
	for ( iter = kControlList.begin(); iter != kControlList.end(); iter++ )
	{
		iter->second->SetAlpha( ucAlpha );
	}
}


void Container::Rewind()
{
	iSearchIndex = 0;
}


bool Container::CheckPixel( int iX, int iY )
{
	if ( iShapeId )
	{
		Control *control = GetControl( iShapeId );
		if ( control )
		{
			return control->CheckPixel( iX, iY );
		}
	}
	return true;
}


Uint32 Container::FindDragContainer( int iX, int iY, int *iDropX, int *iDropY, Uint32 *uiCharId )
{
	iX -= GetX();
	iY -= GetY();
	
	ControlList_t::reverse_iterator iter;
	for ( iter = kControlList.rbegin(); iter != kControlList.rend(); iter++ )
	{
		if ( iter->second->MouseIsOver( iX, iY ) )
		{
			return iter->second->FindDragContainer( iX, iY, iDropX, iDropY, uiCharId );
		}
	}

	return 0;
}


int Container::SendMessageToItems( gui_message *kMsg )
{
	ControlList_t::reverse_iterator iter;
	for ( iter = kControlList.rbegin(); iter != kControlList.rend(); iter++ )
	{
		if ( ( iter->second->GetPage() == iCurrentPage ) 
			|| ( iter->second->GetPage() == 0 ) )
		{
			if ( iter->second->HandleMessage( kMsg ) )
			{
				return true;
			}
		}
	}

	return false;
}


void Container::ReleaseFocus( int iControlId )
{
	Control *control = GetControl( iControlId );

	if ( control )
	{
		control->SetFocus( false );
	}
	if ( iControlId == iFocusId )
	{
		iFocusId = 0;
	}
}


void Container::HandleMessageQueues()
{
	MessageStack kMessageStack;
	gui_message kMsg;

	ControlList_t::iterator iter;
	for ( iter = kControlList.begin(); iter != kControlList.end(); iter++ )
	{
		while ( iter->second->stack.Pop( &kMsg ) )
		{
			kMessageStack.Push( kMsg );
		}
	}

	while ( kMessageStack.Pop( &kMsg ) )
	{
		switch ( kMsg.type )
		{
		case MESSAGE_SETFOCUS:
			SetFocus( kMsg.windowaction.controlid );
			break;

		case MESSAGE_RELEASEFOCUS:
			ReleaseFocus( kMsg.windowaction.controlid );
			break;

		case MESSAGE_QUIT:
		case MESSAGE_STARTGAME:
		case MESSAGE_ONDRAGITEM:
		case MESSAGE_ONCLICKITEM:
		case MESSAGE_REFRESHREQUEST:
		case MESSAGE_CALLBACK:
			stack.Push( kMsg );
			break;
		}
	}
}
