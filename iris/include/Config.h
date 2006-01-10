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

// MAX string length
#include "include.h"
#include <string>

#define _stricmp strcasecmp
#define _strdup strdup

namespace	nConfig
{
        extern std::string version;
	// GFX
	extern int width, height, bpp, startfullscreen;
	extern int cursor, perspective, depthbuffer, maxzoom, maxangle;
        extern int viewdistance, brightness, animationsmoothness;
        extern int width_2d, height_2d;
       
       extern int firstperson;
       extern int hideself;
       extern int roof_fade;
	// UO	
	extern int startx, starty, startz;
        extern int aos;
	extern std::string mulpath;
        extern std::string compressed_map;
        extern int popup;
        
        extern std::string client_version;
        extern int clilocs;
        extern std::string cliloc_lang;
        extern int speech_hue;
	extern int aostooltips;

	// NET
	extern std::string server, login, password;
	extern int serverport;
	extern int roof_fade_time, roof_fade_alpha;
	extern int is_sphere;
	extern int is_sphere55R;
	extern int is_pol;
	extern int is_runuo;
	extern int is_uox3;
        extern int client_key;
        
        // SOUND
        extern int music, sound, frequency, stereo, chunksize, musicvolume, soundvolume, mp3, footsteps;

        // IRIS
        extern std::string scriptpath;
        extern int mousemotion_timer;
	
	// Function
	bool	Init();
	bool RegisterFonts();
};
