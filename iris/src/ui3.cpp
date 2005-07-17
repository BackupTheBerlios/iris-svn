//
// File: ui3.cpp
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


#include <iostream>
#include "SDLEvent.h"
#include "renderer/SDLScreen.h"
#include "Debug.h"
#include "Exception.h"
#include "Game.h"
#include "Config.h"
#include "gui/GUIHandler.h"
#include "gui/Image.h"
#include "net/Client.h"
#include "uotype.h"

#include "csl/CSLHandler.h"
#include "sound/SoundMixer.h"
#include "sound/MusicListLoader.h"

using namespace std;

SDLEvent *SDLevent;
SDLScreen *SDLscreen;

#undef main

int main (int argc, char **args)
{

  pDebug.Log (string ("Iris version ") + nConfig::version +
              string (" Copyright (C) 2003-2005 Alexander Oster"));
  pDebug.Log ("Iris comes with ABSOLUTELY NO WARRANTY");
  pDebug.
    Log ("This is free software, and you are welcome to redistribute it");
  pDebug.
    Log
    ("under certain conditions; for details, take a look into License.txt");

  try
  {
    /* try to load config file */
    if (!nConfig::Init ())
        {
          pDebug.Log ("Unable to load configuration file - Using defaults ",
                      __FILE__, __LINE__, LEVEL_WARNING);
        }

	/* initialize SDL */
    SDL_Init (0);

    if(SDLNet_Init() == -1)
    {
       pDebug.Log ("could not initialize SDLNet; throwing error...\n");
     }
 
    /* initialize video and event handling
       FIXME: do this after all heavy loading*/
    SDLscreen = new SDLScreen ();
    SDLevent = new SDLEvent;

    nConfig::RegisterFonts();

    /* initialize SDL_music */
    if (nConfig::music || nConfig::sound)
        {
          pSoundMix = new SoundMix;
          pSoundMix->Init ();

          pMusicListLoader = new cMusicListLoader ();
          if (nConfig::music)
              {
                pSoundMix->PlayMusic (8, nConfig::mp3, nConfig::musicvolume);     //
              }
        }
    else
      pSoundMix = NULL;


    /* initialize Scripting */
    pCSLHandler.Init ();
    pCSLHandler.InitAPI ();

    std::string script_path = nConfig::scriptpath;
    if (script_path == "root")
        {
          script_path = "script/iris.csl";
        }
    else
      script_path = "script/" + script_path + "/iris.csl";

    pDebug.Log (script_path.c_str ());
    pCSLHandler.Load ((char *) script_path.c_str ());

    /* init game engine */
    pGame.Init ();

    /* start main script */
    pCSLHandler.ExecuteFunction ("main");

    /* main loop */
    while (!SDLevent->quit && !pUOGUI.QuitFlag ())
        {
          // Handle events in the queue
          SDLevent->PollEvent ();
          pGame.Handle ();
          pGame.RenderScene ();
          if (pClient)
            pClient->Poll ();
        }


    /* clean up */
    pGame.Disconnect ();
    pGame.DeInit ();
    pUOGUI.DeInit ();

    if (pSoundMix)
      delete pSoundMix;
    pSoundMix = NULL;

    delete SDLevent;
    delete SDLscreen;
    SDLNet_Quit ();
    SDL_Quit ();
    delete pMusicListLoader;
    pMusicListLoader = NULL;
  }
  catch (cException my_exception)
  {
    pDebug.Log (my_exception.debug_message ());
    SDLNet_Quit ();
    SDL_Quit ();
   }
  catch (...)
  {
    pDebug.Log ("Unhandled exception");
  }

  return 0;
}

