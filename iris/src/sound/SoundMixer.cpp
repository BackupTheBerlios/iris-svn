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
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"
#include "Debug.h"
#include "Config.h"
#include "sound/SoundMixer.h"
#include "loaders/SoundLoader.h"
#include "sound/Music.h"
#include "net/Client.h"
#include "renderer/Characters.h"
#include "math.h"
#include <cassert>

SoundMix *pSoundMix = NULL;

SoundMix::SoundMix ()
{
}

SoundMix::~SoundMix ()
{
  SDL_QuitSubSystem (SDL_INIT_AUDIO);
}

void SoundMix::Init ()
{
  int stereo_channels = MIX_DEFAULT_CHANNELS;

  if (nConfig::stereo == 1)
     stereo_channels = 2;
  else
      stereo_channels = 1;
  // start SDL with audio support
  if (SDL_InitSubSystem (SDL_INIT_AUDIO) != 0)
      {
        pDebug.Log ("Initializing SDL Audio failed...", __FILE__, __LINE__,
                    LEVEL_ERROR);
        nConfig::music = false;
        nConfig::sound = false;
      }
  else if
    (Mix_OpenAudio
     (nConfig::frequency, MIX_DEFAULT_FORMAT, stereo_channels,
      nConfig::chunksize) < 0)
      {
        pDebug.Log ("Open SDL Audio failed...", __FILE__, __LINE__,
                    LEVEL_ERROR);
        nConfig::music = false;
        nConfig::sound = false;
      };

  std::cout << "Starting sound system...\n";
  if (nConfig::music)
      {
        std::cout << "->Enabling music support....\n";
        MusicVolume (nConfig::musicvolume);
        //Mix_VolumeMusic(nConfig::musicvolume);
      }
  else
    std::cout << "->Music support disabled....\n";

  if (nConfig::sound)
      {
        if (SoundInit (nConfig::mulpath))
            {
              // allocate mixing channels
              std::cout << "->Enabling sound support....\n";
            }
        else
          std::cout << "->Sound support disabled....\n";
        nConfig::sound = 0;
      }
}

void
  SoundMix::PlaySound (int sound, int volume, char flags, int x, int y, int z)
{
  wave = Load (sound);

  if (!wave)
    return;

  cCharacter *mychar = pClient->player_character ();
  assert (mychar);

  int my_x = mychar->x ();
  int my_y = mychar->y ();

  int xdist = my_x - x;         // Calculate distance
  int ydist = my_y - y;
  float dist = sqrt ((float)(xdist * xdist + ydist * ydist));

  if (wave)
      {
        int volume = (int) ((1.0f - (dist * 0.05f)) * nConfig::soundvolume);  // Set Volume
        if (volume <= 5)
          volume = 5;
        Mix_VolumeChunk (wave, volume);

        if (Mix_PlayChannel (-1, wave, 0) == -1)
            {
              printf ("Mix_PlayChannel: %s\n", Mix_GetError ());
              // may be critical error, or maybe just no channels were free.
              // you could allocated another channel in that case...
              Mix_FreeChunk (wave);
              wave = NULL;
            }
      }

}

void SoundMix::PlaySoundWithVolume (int sound, int volume)
{
  wave = Load (sound);

  if (!wave)
    return;

  if (wave)
      {
        Mix_VolumeChunk (wave, volume);
        if (Mix_PlayChannel (-1, wave, 0) == -1)
            {
              printf ("Mix_PlayChannel: %s\n", Mix_GetError ());
              // may be critical error, or maybe just no channels were free.
              // you could allocated another channel in that case...
              Mix_FreeChunk (wave);
              wave = NULL;
            }
      }

}

int SoundMix::Quit ()
{
  Mix_CloseAudio ();
  return true;
}
