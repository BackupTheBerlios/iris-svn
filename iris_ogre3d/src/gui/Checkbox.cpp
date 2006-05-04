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




Checkbox::Checkbox( int iUnCheckedPic, int iCheckedPic ) : m_bChecked( false )
{PROFILE
	Control::Control();
	SetGump( CHECKBOX_CHECKED, iCheckedPic );
	SetGump( CHECKBOX_UNCHECKED, iUnCheckedPic );
	SetFlag( GUMPFLAG_MOVABLE, false );
}


Checkbox::Checkbox( int iX, int iY, int iUnCheckedPic, int iCheckedPic, bool bChecked ) : m_bChecked( bChecked )
{PROFILE
	SetGump( CHECKBOX_CHECKED, iCheckedPic );
	SetGump( CHECKBOX_UNCHECKED, iUnCheckedPic );
	SetPosition( iX, iY );
	SetFlag( GUMPFLAG_MOVABLE, false );
}


Checkbox::~Checkbox()
{PROFILE
}


void Checkbox::SetGump( int iType, int iGump )
{PROFILE
	mbNeedGfxUpdate = true;
	if ( iType == CHECKBOX_CHECKED )
	{
		m_iCheckedGump = iGump;
	}
	else if ( iType == CHECKBOX_UNCHECKED )
	{
		m_iUncheckedGump = iGump;
	}
	else
	{
		char errorStr[512];
		sprintf( errorStr, "Illegal Checkbox state: %d", iType );
        Logger::WriteLine( errorStr, __FILE__, __LINE__, LEVEL_ERROR );
	}
}


void Checkbox::SetChecked( bool bChecked )
{
	mbNeedGfxUpdate = true;
	m_bChecked = bChecked;
}


bool Checkbox::IsChecked()
{
	return m_bChecked;
}


void Checkbox::CreateGFX	(GumpHandler *gumps) {PROFILE printf("Checkbox::CreateGFX\n"); 
	if (mlRect.size() > 0) return;
	mlRect.push_back(Control::CreateRect());
}

void Checkbox::DrawStep		(GumpHandler *gumps) {PROFILE
	Control::DrawStep( gumps );
	if (!mbNeedGfxUpdate)  return;
	Texture *texture = NULL;
	if ( m_bChecked )
	{
		texture = LoadGump( m_iCheckedGump, gumps, false );
	}
	else
	{
		texture = LoadGump( m_iUncheckedGump, gumps, false );
	}

	int iWidth = GetWidth();
	int iHeight = GetHeight();
	if ( !iWidth || !iHeight )
	{
		SetSize( texture->GetRealWidth(), texture->GetRealHeight() );
	}

	UpdateRect(mlRect[0], GetX(), GetY(), iWidth, iHeight, texture );	
	
}
void Checkbox::DestroyGFX	() {PROFILE  Control::DestroyGFX();}
void Checkbox::Draw( GumpHandler *gumps )
{
}


int Checkbox::HandleMessage( gui_message *kMsg )
{PROFILE
	if ( !kMsg )
	{
		Logger::WriteLine( "NULL msg in Checkbox::HandleMessage(gui_message *)", __FILE__, __LINE__, LEVEL_ERROR );

		return false;
	}

	switch ( kMsg->type )
	{
	case MESSAGE_MOUSEDOWN:
		if ( MouseIsOver( kMsg->mouseevent.x, kMsg->mouseevent.y ) )
		{
			mbNeedGfxUpdate = true;
			m_bChecked = !m_bChecked;

			return true;
		}
		break;

	default:
		Control::HandleMessage( kMsg );
	}

	return false;
}
