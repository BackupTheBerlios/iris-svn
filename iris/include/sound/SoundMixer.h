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

#ifndef  __SOUNDMIX_H__
#define __SOUNDMIX_H__

#ifdef WIN32
#include <windows.h>
#endif


#include <string>
#include "SDL/SDL.h"
#include "SDL/SDL_mixer.h"
#include "Music.h"
#include "loaders/SoundLoader.h"

class SoundMix : public Music, public SoundLoader
{
private:
	Mix_Chunk *wave;
public:
	SoundMix();
	~SoundMix();
	void Init();
	int Quit();
	void PlaySound(int sound, int volume, char flags, int x, int y, int z);
	void PlaySoundWithVolume(int sound, int volume);
};

extern SoundMix * pSoundMix;

#endif
