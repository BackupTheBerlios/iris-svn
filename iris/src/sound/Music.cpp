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


#include <stdio.h>
#include "Debug.h"
#include "Config.h"
#include "sound/Music.h"
#include "sound/MusicListLoader.h"
#ifdef WIN32
#include "smpeg/smpeg.h"
#endif

#include <iostream>
#include <cassert>
using namespace std;

Music::Music () 
{ 
       music=NULL; 
#ifdef WIN32 
       mpeg=NULL; 
#endif 
}

Music::~Music () 
{ 
  // fade music out for .1 seconds 
  while (Mix_PlayingMusic() && !Mix_FadeOutMusic (100)) 
      { 
        // wait for any fades to complete 
        SDL_Delay (100); 
      } 

  // MIDI doesn't fade nor does it turn MusicPlaying off when it completes 
  if (Mix_PlayingMusic() && !nConfig::mp3) { 
    // force music to end 
    Mix_HaltMusic(); 
  } 

  Mix_FreeMusic (music); 
  music = NULL; 
}

// Function to read music.ini, will finish when xml support is done
int Music::Config ()
{
  return true;
}

int Music::PlayMusic (std::string name, int volume)
{
  std::string file = nConfig::mulpath;

  if (!volume)
    volume = nConfig::musicvolume;

  if (nConfig::mp3)
      {
        file += "music/digital/" + name;
#ifdef WIN32
        if (mpeg)
            {
              SMPEG_stop (mpeg);
              SMPEG_delete (mpeg);
            }
        mpeg = SMPEG_new (file.c_str (), &info, 0);

        char *error = SMPEG_error (mpeg);

        pDebug.Log (error);
        assert (mpeg);
        assert (info.has_audio);

        // Tell SMPEG what the audio format is.

        SDL_AudioSpec audioFmt;
        int frequency, channels;
        Uint16 format;
        Mix_QuerySpec (&frequency, &format, &channels);
        audioFmt.format = format;
        audioFmt.freq = frequency;
        audioFmt.channels = channels;

        SMPEG_actualSpec (mpeg, &audioFmt);


        // Hook in the MPEG music mixer.
        Mix_HookMusic (SMPEG_playAudioSDL, mpeg);
        SMPEG_enableaudio (mpeg, 1);
        SMPEG_enablevideo (mpeg, 0);
        SMPEG_setvolume (mpeg, volume);
        SMPEG_loop (mpeg, true /* repeat */ );


        SMPEG_play (mpeg);
        return 1;
#endif
      }
      else
      {
        file += "music/" + name;
      }

      while (!Mix_FadeOutMusic (100) && Mix_PlayingMusic ()) {
        // wait for any fades to complete
        SDL_Delay (100);
      }
      /* Stop the music from playing */
      Mix_HaltMusic();
      /* Unload the music from memory, since we don't need it
      anymore */
      Mix_FreeMusic(music);
      music = NULL;
      pDebug.Log (file.c_str ());

      if(music == NULL) {
               music = Mix_LoadMUS (file.c_str ());
      }

      if (!music)
      {
        pDebug.Log ("PlayMusic() can not load file", __FILE__, __LINE__,
                    LEVEL_WARNING);
        // this might be a critical error...
        nConfig::music = 0;
        return false;
      }
      if (Mix_FadeInMusic (music, -1, 3000) == -1)
      {
        printf ("Mix_FadeInMusic: %s\n", Mix_GetError ());
        // well, there's no music, but most games don't break without music...
        nConfig::music = 0;
        return false;
      }
      return true;
}

int Music::PlayMusic (int id, int format, int volume)
{
  std::string file;

  std::string mp3path = pMusicListLoader->mp3path ();
  std::string midipath = pMusicListLoader->midipath ();

  if (!volume)
    volume = nConfig::musicvolume;

  std::string name = pMusicListLoader->GetMusic (id, format);
  if(name == "")
    return 0;

  cMusicListEntry *entry = pMusicListLoader->GetMusicListEntry (id);

  if (name.empty ())
      {
        pDebug.Log ("Wrong musicfile name");
        return 0;
      }

  if (format == FORMAT_MP3)
      {
        file = mp3path + name;

#ifdef WIN32
        if (mpeg)
            {
              pDebug.Log ("PLAYING");
              SMPEG_stop (mpeg);
              SMPEG_delete (mpeg);
            }
        mpeg = SMPEG_new (file.c_str (), &info, 0);

        char *error = SMPEG_error (mpeg);

        if (error != NULL)
          return 0;
        pDebug.Log (error);

        assert (mpeg);
        assert (info.has_audio);

        // Tell SMPEG what the audio format is.

        SDL_AudioSpec audioFmt;
        int frequency, channels;
        Uint16 format;
        Mix_QuerySpec (&frequency, &format, &channels);
        audioFmt.format = format;
        audioFmt.freq = frequency;
        audioFmt.channels = channels;

        SMPEG_actualSpec (mpeg, &audioFmt);

        // Hook in the MPEG music mixer.
        Mix_HookMusic (SMPEG_playAudioSDL, mpeg);
        SMPEG_enableaudio (mpeg, 1);
        SMPEG_enablevideo (mpeg, 0);
        SMPEG_setvolume (mpeg, volume);
        SMPEG_loop (mpeg, entry->loop);

        SMPEG_play (mpeg);
        return true;
#endif
      }
      else
      {
        file = midipath + name;
        //pDebug.Log(file.c_str());
      }
      while (!Mix_FadeOutMusic (100) && Mix_PlayingMusic ()) {
        // wait for any fades to complete
        SDL_Delay (100);
      }
     // MIDI doesn't fade nor does it turn MusicPlaying off when it completes 
     if (Mix_PlayingMusic() && !nConfig::mp3) {
         // force music to end
         Mix_HaltMusic(); 
         Mix_FreeMusic (music);
         music = NULL;
      } 
      pDebug.Log (file.c_str ());

      if(music == NULL) {
               music = Mix_LoadMUS (file.c_str ());
      }

      if(music == NULL) {
        pDebug.Log ("PlayMusic() can not load file", __FILE__, __LINE__,
                    LEVEL_WARNING);
        // this might be a critical error...
        nConfig::music = 0;
        return false;
      }
      if (Mix_FadeInMusic (music, -1, 3000) == -1) {
        printf ("Mix_FadeInMusic: %s\n", Mix_GetError ());
        // well, there's no music, but most games don't break without music...
        nConfig::music = 0;
        return false;
      }
      return true;
}

void Music::MusicVolume (int volume)
{
  Mix_VolumeMusic (volume);
}
