//
// File: ClilocLoader.cpp
// Created by: ArT-iX ArT-iX@libero.it
// 12.06.2005 modified by SiENcE
//            -added support for all Cliloc Strings for non-english Languages 

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
#include <fstream>
#include <vector>
#include "Debug.h"
#include "Config.h"
#include "loaders/ClilocLoader.h"
#include "string_utils.h"
#include <string>

cClilocLoader pClilocLoader;

cClilocLoader::cClilocLoader ()
{
}

cClilocLoader::~cClilocLoader ()
{
    DeInit ();
}

void cClilocLoader::Init (std::string path)
{
  std::ifstream clilocfile;

  std::string default_language = "enu";
  std::string filename = path + "cliloc." + default_language;

  std::string language = nConfig::cliloc_lang;
  std::string filename_custom = path + "cliloc." + language;

  clilocfile.open (filename.c_str (), std::ios::in | std::ios::binary);

  if (!clilocfile.is_open ())
      {
        pDebug.Log ("Warning: Couldn't open cliloc file");
        clilocfile.close ();
        nConfig::clilocs = 0;
        return;
      }

  clilocfile.seekg (0, std::ios::end);
  int filelen = clilocfile.tellg ();
  clilocfile.seekg (0, std::ios::beg);

  int index;
  Uint16 msglen;
  char *msg;
  std::string message = "";
  char unknown[6];

  clilocfile.read (unknown, 6);
  while (clilocfile.tellg () < filelen)
  {
        clilocfile.read ((char *) &index, 4);
        clilocfile.read (unknown, 1);
        clilocfile.read ((char *) &msglen, 2);
        msg = new char[msglen + 1];
        clilocfile.read ((char *) msg, msglen);
        msg[msglen] = 0;
        message = std::string (msg);
        delete msg;
        cliloc_messages.insert (make_pair (index, message));
  }
  clilocfile.close ();
  
  if ( language!=default_language )
  {
    clilocfile.open (filename_custom.c_str (), std::ios::in | std::ios::binary);
    if (!clilocfile.is_open ())
    {
        pDebug.Log ("Warning: Couldn't open cliloc file");
        clilocfile.close ();
        nConfig::clilocs = 0;
        return;
      }
    clilocfile.seekg (0, std::ios::end);
    filelen = clilocfile.tellg ();
    clilocfile.seekg (0, std::ios::beg);
    clilocfile.read (unknown, 6);
    
    while (clilocfile.tellg () < filelen)
    {
        clilocfile.read ((char *) &index, 4);
        clilocfile.read (unknown, 1);
        clilocfile.read ((char *) &msglen, 2);

        msg = new char[msglen + 1];
        clilocfile.read ((char *) msg, msglen);
        msg[msglen] = 0;
        message = std::string (msg);
        delete msg;
        cliloc_messages[index] = message;
    }
    clilocfile.close ();
  }

}

void cClilocLoader::DeInit ()
{
 cliloc_messages.clear();
}

std::string cClilocLoader::GetMessage (int id)
{

  std::map < int, std::string >::iterator iter;
  iter = cliloc_messages.find (id);
  if (iter != cliloc_messages.end ())
    return iter->second;

  return string ("");
}

std::string cClilocLoader::GetMessageWithArguments (int id, int args_num,
                                                    vector < std::string >
                                                    &args)
{

  vector < std::string > splitted_str;
  std::string word;

  std::string ret_msg = "";
  std::string message = GetMessage (id);
  char *tags = new char[message.size () + 1];
  memset(tags,0,message.size ()+1);
  strcpy (tags, message.c_str ());


  char *tag = strtok (tags, "~~");

  while (tag != NULL)
  {
        char *copy_str = new char[strlen (tag) + 1];
        copy_str[strlen (tag)] = 0;
        strcpy (copy_str, tag);
        splitted_str.push_back (string (copy_str));
        tag = strtok (NULL, "~~");
        delete[] copy_str;
  }

  int args_index = 0;
  for (unsigned int i = 0; i < splitted_str.size (); i++)
      {

        word = splitted_str.at (i);

        if (word.empty ())
          continue;

        char *newstr = (char *) word.c_str ();

        if (((newstr[0] >= 0x30) && (newstr[0] <= 0x39)) && newstr[1] == 95)
            {
              word = args.at (args_index);
              args_index++;
              if (word[0] == 35)
                  {
                    std::string sub = word.substr (1);
                    int msgid = atoi (sub.c_str ());
                    word = GetMessage (msgid);
                  }
            }
        ret_msg += word;
        // HARKON: why delete?
        //delete[] newstr;
      }


  delete[] tags;
  delete[] tag;
  
  return ret_msg;
}
