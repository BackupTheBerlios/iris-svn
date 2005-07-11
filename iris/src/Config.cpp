//
// File: Debug.cpp
// Created by: Alexander Oster - tensor@ultima-iris.de
// Modified by: SiENcE (10.01.2004)
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

#include <string>
#include "Debug.h"
#include "Exception.h"
#include "xml.h"
#include "renderer/SDLScreen.h"

extern SDLScreen* SDLscreen;

using namespace std;

#define IS_SECTION 1
#define IS_INTEGER 2
#define IS_STRING  3
#define IS_BOOL    4
#define IS_END     5

struct ParserData
{
  ParserData (const string & NewName, int NewType,
              void *NewDataPtr):name (NewName), type (NewType),
    dataptr (NewDataPtr)
  {
  };
  string name;
  int type;
  void *dataptr;                // datatofill -> yes void pointers are bad style.
};


namespace nConfig
{

  string version = "0.8.0";

// Screen related
  int width = 640;
  int height = 480;
  int width_2d = 640;
  int height_2d = 480;
  int bpp = 16;
  int startfullscreen = 0;
  int roof_fade_time = 2000;
  int roof_fade_alpha = 1;
  int depthbuffer = 16;
  int maxzoom = 10;
  int maxangle = 75;
  int viewdistance = 8;
  int brightness = 5;
  int animationsmoothness = 25;

// Net related
  string server = "localhost";
  string login = "account";
  string password = "";
  int serverport = 2593;
  int is_sphere = 0;
  int is_sphere55R = 0;
  int is_pol = 0;
  int is_runuo = 1;
  int is_uox3 = 0;
  int client_key = 1;

// UO related//
  int startx = 1500;
  int starty = 1000;
  int startz = 0;
  int cursor = 1;
  int perspective = 0;
  string mulpath;
  string compressed_map = "";
  int aos = 1;
  int popup = 1;
  string client_version = "4.0.10a";
  int clilocs = 1;
  string cliloc_lang = "enu";

  int speech_hue = 100;
  int aostooltips = 0;
// Sound related
  int music = 1;
  int sound = 1;
  int frequency = 22050;
  int stereo = 1;
  int chunksize = 1024;
  int musicvolume = 60;
  int soundvolume = 80;
  int mp3 = 1;
  int footsteps = 0;
  // Iris related 
  string scriptpath = "root";
  int mousemotion_timer = 500;

	// FONT related
	struct font_info {
		int id;
		std::string file;
		std::string name;
		int size;
		int hue;
	};

	std::vector<font_info> fonts;
}

using namespace nConfig;

// important always write childs of section behind that Section array element//
ParserData ParserInfo[] = {
  // Section Gfx
  ParserData ("GFX", IS_SECTION, NULL),
  ParserData ("WIDTH", IS_INTEGER, &width),
  ParserData ("HEIGHT", IS_INTEGER, &height),
  ParserData ("BPP", IS_INTEGER, &bpp),
  ParserData ("CURSOR", IS_INTEGER, &cursor),
  ParserData ("PERSPECTIVE", IS_INTEGER, &perspective),
  ParserData ("FULLSCREEN", IS_INTEGER, &startfullscreen),
  ParserData ("ROOF_FADE_TIME", IS_INTEGER, &roof_fade_time),
  ParserData ("ROOF_FADE_ALPHA", IS_INTEGER, &roof_fade_alpha),
  ParserData ("ZBUFFER_SIZE", IS_INTEGER, &depthbuffer),
  ParserData ("MAXZOOM", IS_INTEGER, &maxzoom),
  ParserData ("MAXANGLE", IS_INTEGER, &maxangle),
  ParserData ("VIEWDISTANCE", IS_INTEGER, &viewdistance),
  ParserData ("BRIGHTNESS", IS_INTEGER, &brightness),
  ParserData ("ANIMATION_SMOOTHNESS", IS_INTEGER, &animationsmoothness),
  ParserData ("UO", IS_SECTION, NULL),
  ParserData ("STARTX", IS_INTEGER, &startx),
  ParserData ("STARTY", IS_INTEGER, &starty),
  ParserData ("STARTZ", IS_INTEGER, &startz),
  ParserData ("MULPATH", IS_STRING, &mulpath),
//  ParserData ("COMPRESSED_MAP", IS_STRING, &compressed_map),
  ParserData ("POPUP_MENU", IS_INTEGER, &popup),
  ParserData ("AOS", IS_INTEGER, &aos),
  ParserData ("CLIENT_IDENTIFICATION", IS_STRING, &client_version),
  ParserData ("USE_CLILOCS", IS_INTEGER, &clilocs),
  ParserData ("CLILOC_LANGUAGE", IS_STRING, &cliloc_lang),
  ParserData ("SPEECH_HUE", IS_INTEGER, &speech_hue),

  ParserData ("AOSTOOLTIPS", IS_INTEGER, &aostooltips),
  ParserData ("NET", IS_SECTION, NULL),
  ParserData ("PORT", IS_INTEGER, &serverport),
  ParserData ("SERVER", IS_STRING, &server),
  ParserData ("LOGIN", IS_STRING, &login),
  ParserData ("PASSWORD", IS_STRING, &password),
  ParserData ("IS_SPHERE", IS_INTEGER, &is_sphere),
  ParserData ("IS_SPHERE55R", IS_INTEGER, &is_sphere55R),
  ParserData ("IS_POL", IS_INTEGER, &is_pol),
  ParserData ("IS_RUNUO", IS_INTEGER, &is_runuo),
  ParserData ("IS_UOX3", IS_INTEGER, &is_uox3),
  ParserData ("CLIENT_KEY", IS_INTEGER, &client_key),
  ParserData ("SOUND", IS_SECTION, NULL),
  ParserData ("MUSIC", IS_INTEGER, &music),
  ParserData ("SOUND", IS_INTEGER, &sound),
  ParserData ("FREQUENCY", IS_INTEGER, &frequency),
  ParserData ("STEREO", IS_INTEGER, &stereo),
  ParserData ("CHUNKSIZE", IS_INTEGER, &chunksize),
  ParserData ("MUSICVOLUME", IS_INTEGER, &musicvolume),
  ParserData ("SOUNDVOLUME", IS_INTEGER, &soundvolume),
  ParserData ("MP3", IS_INTEGER, &mp3),
  ParserData ("FOOTSTEPS", IS_INTEGER, &footsteps),

  ParserData ("IRIS", IS_SECTION, NULL),
  ParserData ("SCRIPTS_PATH", IS_STRING, &scriptpath),
  ParserData ("MOUSEOVER_TIMER", IS_INTEGER, &mousemotion_timer),
  ParserData ("", IS_END, NULL)
};
namespace nConfig
{

// Functions
  bool Init ()
  {
    XML::Parser parser;
    XML::Node * config, *document;

    try
    {
      parser.loadData ("./xml/config.xml");
      document = parser.parseDocument ();

      config = document->findNode ("CONFIG");

      if (!config)
        THROWEXCEPTION ("Couldn't find configuration node.");
    }
    catch (...)
    {
      return false;
    }
    XML::Node * section = NULL, *value = NULL;
    for (int i = 0; ParserInfo[i].type != IS_END; i++)
        {
          const ParserData & Data = ParserInfo[i];
          if (Data.type == IS_SECTION)
              {
                section = config->findNode (Data.name);
              }
          else
              {
                value = section != NULL ? section->findNode (Data.name) : config->findNode (Data.name);     // If no section is loaded get from <config>
                if (value == NULL);
                else
                    {
                      if (Data.type == IS_BOOL)
                          {
                            *reinterpret_cast < bool * >(Data.dataptr) =
                              value->asBool ();
                          }
                      else if (Data.type == IS_INTEGER)
                          {
                            *reinterpret_cast < int *>(Data.dataptr) =
                              value->asInteger ();
                          }
                      else if (Data.type == IS_STRING)
                          {
                            *reinterpret_cast < string * >(Data.dataptr) =
                              value->asString ();
                          }
                    }
              }
        }

		XML::Node* font_set = config->findNode("FONTSET");

		if (font_set) {
			int idx=0;
			XML::Node* font_node = NULL;			
			font_info font;
			while ((font_node = font_set->findNode("FONT", idx++))) {
				if (!font_node->lookupAttribute("ID", font.id)) continue;
				if (!font_node->lookupAttribute("FILE", font.file)) continue;
				if (!font_node->lookupAttribute("NAME", font.name)) continue;
				if (!font_node->lookupAttribute("SIZE", font.size)) continue;
				if (!font_node->lookupAttribute("HUE",  font.hue)) continue;

				fonts.push_back(font);
			}
		}

    delete document;            // I don't like that I'll alter darkstorm's class xml class so this is not necessary
    if (depthbuffer <= 0)
      depthbuffer = 16;
    if (viewdistance < 3)
      viewdistance = 3;
    if (maxzoom <= 0)
      maxzoom = 0;
    if (maxangle < 10)
      maxangle = 10;
    if (maxangle > 80)
      maxangle = 80;

    if (animationsmoothness < 10)
      animationsmoothness = 10;
    if (animationsmoothness > 100)
      animationsmoothness = 100;

	//fix Mulpath if no / at the end
	if (nConfig::mulpath.size ())
    {
		char last_char = nConfig::mulpath[nConfig::mulpath.length () - 1];
		if ((last_char != '/') && (last_char != '\\'))
		  nConfig::mulpath +="/";
	}

    return true;
  }

  bool RegisterFonts() {
	if (!SDLscreen) return false;
		for (unsigned int i=0; i < fonts.size(); i++) {
			font_info& font = fonts[i];
			SDLscreen->RegisterFont(font.id, font.file, font.size, font.hue);
		}
		return true;
  }
}
