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


#include "iris_endian.h"
#include "loaders/FontLoader.h"
#include "Debug.h"
#include "Config.h"
#include "uotype.h"
#include "Exception.h"

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

cFontLoader::cFontLoader()
{
}

cFontLoader::~cFontLoader ()
{
    DeInit ();
}

void cFontLoader::DeInit ()
{
  std::map < unsigned short, stFont >::const_iterator it;

  for (it = fonts.begin (); it != fonts.end (); ++it)
      {
        unsigned int i;

        for (i = 0; i < 224; ++i)
            {
              delete[]it->second.chars[i].pixels;
              delete[]it->second.chars[i].redmask;
            }
      }
      fonts.clear ();
}

// Load Fonts
void cFontLoader::Init (std::string filename)
{
  std::ifstream fontfile;

  DeInit ();
  
  fontfile.open (filename.c_str (), std::ios::in | std::ios::binary);

  if (!fontfile.is_open ())
      {
        THROWEXCEPTION ("Couldn't open fonts.mul");
      }

  unsigned int j = 0;

  // Read as many fonts as we can
  while (!fontfile.eof ())
      {
        // Header
        unsigned char header;

        fontfile.get (reinterpret_cast < char &>(header));

        // The only valid header in found is an one
        if (header != 1)
          break;

        // Read in the 224 characters
        unsigned int i;
        stFont font;

        font.maxHeight = 0;

        for (i = 0; i < 224; ++i)
            {
              // Width + Height + Header
              fontfile.get (reinterpret_cast < char &>(font.chars[i].width));
              fontfile.get (reinterpret_cast < char &>(font.chars[i].height));
              fontfile.get (reinterpret_cast < char &>(font.chars[i].header));

              if (font.chars[i].height > font.maxHeight)
                font.maxHeight = font.chars[i].height;

              // Real Character Data
              font.chars[i].pixels =
                new unsigned int[font.chars[i].width * font.chars[i].height];
              font.chars[i].redmask =
                new unsigned char[font.chars[i].width * font.chars[i].height];

              unsigned short *pixels =
                new unsigned short[font.chars[i].width *
                                   font.chars[i].height];
              fontfile.read (reinterpret_cast < char *>(pixels),
                             font.chars[i].width * font.chars[i].height * 2);

              for (int j = 0; j < font.chars[i].width * font.chars[i].height;
                   ++j)
                  {
                    if (IRIS_SwapU16 (pixels[j]) == 0)
                        {
                          font.chars[i].pixels[j] = 0;
                          font.chars[i].redmask[j] = 0;
                        }
                    else
                        {
                          font.chars[i].pixels[j] =
                            color15to32 (IRIS_SwapU16 (pixels[j]));
                          //font.chars[i].pixels[j] = color15to32( pixels[j] );
                          font.chars[i].redmask[j] =
                            (IRIS_SwapU16 (pixels[j]) >> 10) & 0x1F;
                        }
                  }

              delete[]pixels;
            }

        fonts.insert (std::make_pair (j++, font));
      }

  fontfile.close ();

}

const stFont *cFontLoader::getFont (unsigned short id) const
{
  std::map < unsigned short, stFont >::const_iterator it;
  it = fonts.find (id);

  if (it == fonts.end ())
    return NULL;

  return &(it->second);
}

cFontLoader pFontLoader;

