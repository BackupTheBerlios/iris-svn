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
#include <string>
#include <stdlib.h>

#include "gui/HTMLGumpParser.h"
#include "gui/MultiLabel.h"
#include "loaders/HueLoader.h"
#include "Debug.h"
#include "string_utils.h"
#include "iris_endian.h"
#include <cstdlib>

cHTMLGumpParser::cHTMLGumpParser ()
{
  _defcolor = 0;
}

cHTMLGumpParser::~cHTMLGumpParser ()
{
}

bool cHTMLGumpParser::Parse (std::string html_text, cMultiLabel * label)
{
  if (html_text == "") return false;
  
  std::string htmltag;
  std::vector < std::string > words;
  std::string word;
  MultiLabelComponent component;
  std::string text = "";

  int i = 0;
  //int apices = 0;
  char apices[2];
  apices[0] = 34;
  apices[1] = 0;
  int f_apices = html_text.find(string(apices), 0);
  while(f_apices != std::string::npos)
  {
   html_text.erase(f_apices);
   if(f_apices == html_text.size())
    break;
   f_apices = html_text.find(string(apices), f_apices);
  }
   // a href=aaa
  //apices = html_text.find(
  //pDebug.Log ("STR1:");
  //pDebug.Log (html_text.c_str ());
  char *tags = (char *) html_text.c_str ();
  //pDebug.Log (tags);

  /* TODO (ArTiX#1#): Check for memory leaks into char* */
  

  char *tag = strtok (tags, "<>");

  while (tag != NULL)
      {
        words.push_back (string (tag));
//        printf ("Tag: %s\n", tag);

        tag = strtok (NULL, "<>");
      }


  int line = 0;
  int center = 0;
  int underline = 0;

  int last_color[3] = { 0, 0, 0 };

  component.text = "";
  component.href = "";
  component.alignment = 0;


  char hexbuffer[33];
  hexbuffer[32] = 0;
  sprintf (hexbuffer, "%x", _defcolor);
  std::string hexstr = string (hexbuffer);


  while (hexstr.size () < 6)
      {
        hexstr += "0";
      }

  std::string red = hexstr.substr (0, 2);
  std::string green = hexstr.substr (2, 2);
  std::string blue = hexstr.substr (4, 2);
  
  long _r = strtol (red.c_str (), NULL, 16), _g =
    strtol (green.c_str (), NULL, 16), _b = strtol (blue.c_str (), NULL, 16);
    
  component.r = (int) IRIS_SwapI32(_r);
  component.g = (int) IRIS_SwapI32(_g);
  component.b = (int) IRIS_SwapI32(_b);

  component.style = 0;
  component.font = 1;
  component.x_fix = 0;


  bool closetag = false;


  for (int i = 0; i < words.size (); i++)
      {
        if (words.at (i).empty ())
          continue;


        if (words.at (i) == "CENTER" || words.at (i) == "DIV ALIGN=CENTER"
            || words.at (i) == "center" || words.at (i) == "div align=center")
            {

              if (component.text.size ())
                  {
                    label->AddLabel (component);
                    component.text.clear ();
                  }
              component.alignment = 1;
            }
        else if (words.at (i) == "/CENTER" || words.at (i) == "/DIV"
                 || words.at (i) == "/center" || words.at (i) == "/div")
            {
              if (component.text.size ())
                  {
                    label->AddLabel (component);
                    component.text.clear ();
                  }
              component.alignment = 0;
              closetag = true;
            }
        else if (words.at (i) == "DIV ALIGN=RIGHT"
                 || words.at (i) == "div align=right")
            {
              if (component.text.size ())
                  {
                    label->AddLabel (component);
                    component.text.clear ();
                  }
              component.alignment = 2;
            }
        else if (words.at (i) == "U" || words.at (i) == "u")
            {
              if (component.text.size ())
                  {
                    label->AddLabel (component);
                    component.text.clear ();
                  }
              component.style |= 0x04;
            }
        else if (words.at (i) == "/U" || words.at (i) == "/u")
            {

              //;

              if (component.text.size ())
                  {
                    label->AddLabel (component);
                    component.text.clear ();
                  }
              if (component.style & 0x04)
                component.style ^= 0x04;

              closetag = true;
            }
        else if (words.at (i) == "I" || words.at (i) == "i")
            {
              if (component.text.size ())
                  {
                    label->AddLabel (component);
                    component.text.clear ();
                  }
              component.style |= 0x02;
            }
        else if (words.at (i) == "/I" || words.at (i) == "/i")
            {


              if (component.text.size ())
                  {
                    label->AddLabel (component);
                    component.text.clear ();
                  }
              if (component.style & 0x02)
                component.style ^= 0x02;

              closetag = true;
            }
        else if (words.at (i) == "STRONG" || words.at (i) == "strong")
            {
              if (component.text.size ())
                  {
                    label->AddLabel (component);
                    component.text.clear ();
                  }
              component.style |= 0x01;
            }
        else if (words.at (i) == "/STRONG" || words.at (i) == "/strong")
            {


              if (component.text.size ())
                  {
                    label->AddLabel (component);
                    component.text.clear ();
                  }
              if (component.style & 0x01)
                component.style ^= 0x01;

              closetag = true;
            }
        else if (words.at (i) == "B" || words.at (i) == "b")
            {
              if (component.text.size ())
                  {
                    label->AddLabel (component);
                    component.text.clear ();
                  }
              component.style |= 0x01;
            }
        else if (words.at (i) == "/B" || words.at (i) == "/b")
            {

              //label->AddLabel(component);

              if (component.text.size ())
                  {
                    label->AddLabel (component);
                    component.text.clear ();
                  }
              if (component.style & 0x01)
                component.style ^= 0x01;
              //component.text.clear();
              closetag = true;
            }
        else if ((words.at (i).find ("BASEFONT COLOR=", 0) == 0)
                 || (words.at (i).find ("basefont color=", 0) == 0))
            {
              if (component.text.size ())
                  {
                    label->AddLabel (component);
                    component.text.clear ();
                  }

              std::string rgbstring = words.at (i).substr (16, 6);
              std::string r_str = rgbstring.substr (0, 2);
              std::string g_str = rgbstring.substr (2, 2);
              std::string b_str = rgbstring.substr (4, 2);
              long r = strtol (r_str.c_str (), NULL, 16), g =
                strtol (g_str.c_str (), NULL, 16), b =
                strtol (b_str.c_str (), NULL, 16);

              last_color[0] = IRIS_SwapI32(component.r);
              last_color[1] = IRIS_SwapI32(component.g);
              last_color[2] = IRIS_SwapI32(component.b);

              component.r = (int)IRIS_SwapI32(r);
              component.g = (int)IRIS_SwapI32(g);
              component.b = (int)IRIS_SwapI32(b);

            }
        else if (words.at (i) == "/BASEFONT" || words.at (i) == "/basefont")

            {

              if (component.text.size ())
                  {
                    label->AddLabel (component);
                    component.text.clear ();
                  }
              component.r = IRIS_SwapI32(last_color[0]);
              component.g = IRIS_SwapI32(last_color[1]);
              component.b = IRIS_SwapI32(last_color[2]);

              closetag = true;
            }
        else if (words.at (i) == "BR" || words.at (i) == "br")
            {

              if (component.text.size ())
                  {
                    label->AddLabel (component);
                    component.text.clear ();
                  }
              label->AddLine ();
              //component.text.clear();
              closetag = true;
            }

        else if (words.at (i) == "DIV ALIGN=LEFT" || words.at (i) == "IMG"
                 || words.at (i) == "BODY" || words.at (i) == "/BODY"
                 || words.at (i) == "div align=left" || words.at (i) == "img"
                 || words.at (i) == "/a" || words.at (i) == "/A" 
                 || words.at (i) == "body" || words.at (i) == "/body")
          continue;
        else if (words.at (i).find ("!--") == 0)
          continue;
        else if (words.at (i).find ("a href") == 0 || words.at (i).find ("A HREF") == 0 )
          continue;  
        else
            {
              //component.text += " " + words.at(i);
              component.text += words.at (i);
              //label->AddLabel(component);
            }
      }
  //if(!closetag){
  //label->AddLabel(component);
  //label->AddLine();}

  if (component.text.size ())
      {
        label->AddLabel (component);
        component.text.clear ();
      }
  label->AddLine ();
/*
  if (tags)
   delete[] tags;
  if (tag)
   delete[] tag;
*/
  return true;
}

void cHTMLGumpParser::setDefaultColor (int defcolor)
{
  _defcolor = defcolor;
}
