/*! \file Main.cpp
 * \brief Main - Program entry point
 * 
 * This is where it all starts, by initializing the engine and making everything work.
 *
 * Copyright (�) Iris Team
 */

 /*
  * Created by Alexander Oster.
  * Last change: 19-02-06 (Nuno Ramiro)
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

#include "Common.h"
#include "SDLEvent.h"
#include "renderer/SDLScreen.h"
#include "Logger.h"
#include "Exception.h"
#include "Game.h"
#include "net/Client.h"
#include "Config.h"
#include "gui/GUIHandler.h"

#include "csl/CSLHandler.h"
#include "sound/SoundMixer.h"
#include "sound/MusicListLoader.h"

SDLScreen *SDLscreen;

/// Program Entry point
int main( int argc, char **args )
{
	// Initializes Logger
	Logger::Init( Config::GetVersion() );

	// Initialize Game
	Game *pGame = Game::GetInstance();

	try
	{
		// Try to load config file
		if ( !Config::Init() )
		{
			Logger::Write( "\t| ->Unable to load configuration file - Using defaults ",
				__FILE__, __LINE__, LEVEL_WARNING );
        }

		// Initialize SDL
		SDL_Init( 0 );

		// Initializes SDLNet
		if ( SDLNet_Init() == -1 )
		{
			Logger::Write( "| ->Could not initialize SDLNet; throwing error...\n" );
		}
 
		/*
		 * Initialize video and event handling
		 * FIXME: do this after all heavy loading
		 */
		SDLscreen = new SDLScreen();
		SDLEvent *SDLevent = new SDLEvent();

		Config::RegisterFonts();

		// Initialize SDL_music
		if ( Config::GetMusic() || Config::GetSound() )
        {
			pSoundMix = new SoundMix();
			pSoundMix->Init();

			pMusicListLoader = new cMusicListLoader();

			if ( Config::GetMusic() )
			{
				pSoundMix->PlayMusic( 8, Config::GetMP3(), Config::GetMusicVolume() );
			}
		}
		else
		{
			pSoundMix = NULL;
		}

		// Initialize Scripting
		pCSLHandler.Init();
		pCSLHandler.InitAPI();

		std::string sScriptPath = Config::GetScriptPath();

		if ( sScriptPath == "root" )
		{
			sScriptPath = "script/iris.csl";
		}
		else
		{
			sScriptPath = "script/" + sScriptPath + "/iris.csl";
		}

		Logger::Write( "DBG | Script path: " );
		Logger::WriteLine( sScriptPath.c_str() );
		pCSLHandler.Load( (char *)sScriptPath.c_str() );

		// Init game engine
		if ( !pGame->Init() )
		{
			Logger::WriteLine( "\t| -> Error while trying to initialize Game." );
		}

		// Start main script
		pCSLHandler.ExecuteFunction( "main" );

		// Main loop
		while ( !SDLevent->quit && !pUOGUI.QuitFlag() )
		{
			// Handle events in the queue
			SDLevent->PollEvent();
			pGame->Handle();
			pGame->RenderScene();

			if ( pClient )
			{
				pClient->Poll();
			}
		}

		// Clean up
		pGame->Disconnect();
		pGame->DeInit();
		pUOGUI.DeInit();

		SAFE_DELETE( pSoundMix );	
		SAFE_DELETE( SDLevent );
		SAFE_DELETE( SDLscreen );

		SDLNet_Quit();
		SDL_Quit();

		SAFE_DELETE( pMusicListLoader );
	}
	catch ( cException my_exception )
	{
		Logger::WriteLine( my_exception.debug_message() );
		SDLNet_Quit();
		SDL_Quit();
	}
	catch ( ... )
	{
		Logger::WriteLine( "Unhandled exception" );
	}

	SAFE_DELETE( pGame );
	Config::Close();
	Logger::Close();

	return 0;
}
