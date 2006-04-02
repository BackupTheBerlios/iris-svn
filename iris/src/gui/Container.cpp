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


Container::Container()
{
	Control::Control();
	idcounter = 1;
	search_index = 0;
	focusid = 0;
	current_page = 0;
	shape_id = 0;
	control_type = CONTROLTYPE_CONTAINER;
}

Container::~Container ()
{
	ClearControls();
}


void Container::Draw( GumpHandler *kGumps )
{
	Control::Draw( kGumps );
	glPushMatrix();
	glTranslatef( GetX(), -GetY(), 0.0 );

	ControlList_t::iterator iter;
	for ( iter = control_root.begin(); iter != control_root.end(); iter++ )
	{
		if ( iter->second->GetPage() == current_page || iter->second->GetPage() == 0 )
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
	for ( iter = control_root.begin(); iter != control_root.end(); iter++ )
	{
		iter->second->DoOnClose();
		SAFE_DELETE( (*iter).second );
	}

	control_root.clear();
}


void Container::AddControl( Control *kControl )
{
	kControl->SetID( idcounter | (GetID() << 16) );
	kControl->SetParent( this );
	control_root.insert( std::make_pair( (Uint32)idcounter | (GetID() << 16), kControl) );
	idcounter++;
}


void Container::AddControl( Control *kControl, int iPage )
{
	kControl->SetPage( iPage );
	AddControl( kControl );
}


Control *Container::GetControl( int iControlId )
{
	ControlList_t::iterator iter;
	iter = control_root.find( (Uint32)iControlId );
	if ( iter == control_root.end() )
	{
		return NULL;
	}
	return (*iter).second;
}


ControlList_t *Container::GetControlList()
{
	return &control_root;
}


Control *Container::GetNext()
{
	ControlList_t::iterator iter;

	if ( (search_index >= 0) && ((unsigned int)search_index < control_root.size()) )
	{
		unsigned int i = search_index;
		search_index++;
		for ( iter = control_root.begin(); (iter != control_root.end()) && (i > 0); iter++ )
		{
			i--;
		}
		if ( iter != control_root.end() )
		{
			return iter->second;
		}
	}
	return NULL;
}


int Container::GetCurrentPage() const
{
	return current_page;
}


int Container::GetPlayerID() const
{
	return pl_id;
}


int Container::GetGumpID() const
{
	return gump_id;
}


void Container::SetCurrentPage( int iCurrentPage )
{
	current_page = iCurrentPage;
}


void Container::SetFocus( int iControlId )
{
	ReleaseFocus( focusid );
	Control *control = GetControl( iControlId );
	if ( !control )
	{
		return;
	}
	control->SetFocus( true );
	focusid = iControlId;
}


void Container::SetAlpha( unsigned char ucAlpha )
{
	Control::SetAlpha( ucAlpha );
	ControlList_t::iterator iter;
	for ( iter = control_root.begin(); iter != control_root.end(); iter++ )
	{
		iter->second->SetAlpha( ucAlpha );
	}
}


void Container::SetShape( int iControlId )
{
	shape_id = iControlId;
}


void Container::SetGumpID( int iId )
{
	gump_id = iId;
}


void Container::SetPlayerID( int iId )
{
	pl_id = iId;
}


void Container::Rewind()
{
	search_index = 0;
}


bool Container::CheckPixel( int iX, int iY )
{
	if ( shape_id )
	{
		Control *control = GetControl( shape_id );
		if ( control )
		{
			return control->CheckPixel( iX, iY );
		}
	}
	return true;
}


Uint32 Container::FindDragContainer( int iX, int iY, int *DropX, int *DropY, Uint32 *uiCharId )
{
	x -= GetX();
	y -= GetY();
	ControlList_t::reverse_iterator iter;
	for ( iter = control_root.rbegin(); iter != control_root.rend(); iter++ )
	{
		if ( iter->second->MouseIsOver( iX, iY ) )
		{
			return iter->second->FindDragContainer( iX, iY, DropX, DropY, uiCharId );
		}
	}
	return 0;
}


int Container::SendMessageToItems( gui_message *kMsg )
{
	ControlList_t::reverse_iterator iter;
	for ( iter = control_root.rbegin(); iter != control_root.rend(); iter++ )
	{
		if ( (iter->second->GetPage () == current_page) || (iter->second->GetPage() == 0) )
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

	if (control)
	{
		control->SetFocus( false );
	}
	if ( iControlId == focusid )
	{
		focusid = 0;
	}
}


void Container::HandleMessageQueues()
{
	MessageStack cont_stack;
	gui_message msg;

	ControlList_t::iterator iter;
	for ( iter = control_root.begin(); iter != control_root.end(); iter++ )
	{
		while ( iter->second->stack.Pop( &msg ) )
		{
			cont_stack.Push( msg );
		}
	}

	while ( cont_stack.Pop( &msg ) )
	{
		switch ( msg.type )
		{
		case MESSAGE_SETFOCUS:
			SetFocus( msg.windowaction.controlid );
			break;

		case MESSAGE_RELEASEFOCUS:
			ReleaseFocus( msg.windowaction.controlid );
			break;

		case MESSAGE_QUIT:
		case MESSAGE_STARTGAME:
		case MESSAGE_ONDRAGITEM:
		case MESSAGE_ONCLICKITEM:
		case MESSAGE_REFRESHREQUEST:
		case MESSAGE_CALLBACK:
			stack.Push( msg );
			break;
		}
	}
}
