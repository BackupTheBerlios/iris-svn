//
// File: StitchinLoader.cpp
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


#include "loaders/StitchinLoader.h"
#include "Debug.h"
#include "Exception.h"
#include "uotype.h"
#include <string.h>
#include <iostream>
#include <string_utils.h>

using namespace std;

cStitchinLoader *pStitchinLoader = NULL;
/*
cModelEntry::cModelEntry (int id, int type, int anim, unsigned int defhue, std::string name)
{
    m_id = id;
    m_type = type;
    m_anim = anim;
    m_defhue = defhue;
    m_name = name;
}
*/
cModelEntry::cModelEntry (int id, int *clist, std::vector < int >rlist,
                          std::map < int, int >replist)
{
  m_id = id;
  coverlist = new int[13];
  for (int i = 0; i < 13; i++)
    coverlist[i] = clist[i];
  removelist = rlist;
  replacelist = replist;
}



cStitchinLoader::cStitchinLoader (std::string modelfilename,
                                  std::string stitchindeffilename)
{
  int actual_id;
  //std::vector<std::string> c_list;
  int bodyparts[13] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
//     int * bodyparts = new int[13];
  std::vector < int >r_list;
  std::map < int, int >rep_list;



  ifstream modelfile (stitchindeffilename.c_str (), ios::in);
  if (!modelfile.is_open ())
    THROWEXCEPTION (string ("Could not load stitchin.def: ") +
                    stitchindeffilename);

  std::vector < std::string > splitline;
  // I know, this is very very ugly!! :)
  while (!modelfile.eof ())
      {

        char line[257];
        modelfile.getline (line, 256);
        line[256] = 0;

        char *p = line;         // DOS/Unix conversion (leave out #10-characters)
        while (*p)
            {
              if (*p == 10)
                *p = 0;
              if (*p == 13)
                *p = 0;
              p++;
            }

        int i, len;

        stringSplit (splitline, string (line), " ");
        std::string firstword = splitline.at (0);

        if (firstword.empty ())
          continue;
        if ((firstword == "#") && (splitline.at (1) != "enddef"))
            {
              std::string idstr = splitline.at (1);
              actual_id = atoi (idstr.c_str ());


            }


        else if ((firstword == "covers"))
            {

              for (int bp = 0; bp < 13; bp++)
                bodyparts[bp] = 0;
              for (int i = 1; i < splitline.size (); i++)
                  {
                    if (splitline.at (i) == "EARS")
                      bodyparts[0] = 1;
                    else if (splitline.at (i) == "FACE")
                      bodyparts[1] = 1;
                    else if (splitline.at (i) == "FEET")
                      bodyparts[2] = 1;
                    else if (splitline.at (i) == "HANDS")
                      bodyparts[3] = 1;
                    else if (splitline.at (i) == "HEAD")
                      bodyparts[4] = 1;
                    else if ((splitline.at (i) == "LOWER_LEGS_TOP")
                             || (splitline.at (i) == "LOWER_LEGS_BOTTOM"))
                        {
                          if ((actual_id != 430) && (actual_id != 461)
                              && (actual_id != 477))
                            bodyparts[5] = 1;
                        }
                    else if (splitline.at (i) == "NECK")
                      bodyparts[6] = 1;
                    else if (splitline.at (i) == "TORSO")
                        {
                          bodyparts[7] = 1;
                          bodyparts[8] = 1;
                        }
                    else if ((splitline.at (i) == "UPPER_ARMS_TOP")
                             || (splitline.at (i) == "UPPER_ARMS_BOTTOM"))
                        {
                          if ((actual_id != 447) && (actual_id != 687))
                            bodyparts[9] = 1;
                        }
                    else if ((splitline.at (i) == "UPPER_LEGS_TOP")
                             || (splitline.at (i) == "UPPER_LEGS_BOTTOM"))
                      bodyparts[10] = 1;

                    else if ((splitline.at (i) == "LOWER_ARMS_TOP")
                             || (splitline.at (i) == "LOWER_ARMS_BOTTOM"))
                      bodyparts[11] = 1;
                    else if (splitline.at (i) == "PELVIS")
                      if ((actual_id != 449) && (actual_id != 971))
                        bodyparts[12] = 1;


                  }

            }

        else if ((firstword == "remove"))
            {

              std::string r_id;

              for (int r = 1; r < splitline.size (); r++)
                  {
                    r_id = splitline.at (r);
                    r_list.push_back (atoi (r_id.c_str ()));
                  }

            }

        else if ((firstword == "replace"))
            {
              std::string rep1 = splitline.at (1);
              std::string rep2 = splitline.at (3);
              rep_list.
                insert (make_pair
                        (atoi (rep1.c_str ()), atoi (rep2.c_str ())));

            }
        else if ((firstword == "#") && (splitline.at (1) == "enddef"))
            {

              models.
                insert (make_pair
                        (actual_id,
                         new cModelEntry (actual_id, bodyparts, r_list,
                                          rep_list)));
              actual_id = 0;
              for (int bp = 0; bp < 13; bp++)
                bodyparts[bp] = 0;
              r_list.clear ();
              rep_list.clear ();

            }
        else
          continue;
        /*
           for (len = 0; *p; p++) 
           if (*p > 32) len++;

           if (len && (*line != '#')) {

           string entries[9];
           for (i = 0; i < 9; i++) entries[i] = ""; 

           i = 0; 
           char * oldp = line;
           for (p = line; (i < 9) && *p; p++)
           if (*p < 32) {
           *p = 0;
           entries[i] = string(oldp);
           oldp = p + 1;
           i++;
           }

           if (!*p && i < 9)
           entries[8] = string(oldp);

           int bodyid = atoi (entries[0].c_str());
           int animid = atoi (entries[1].c_str());
         */
        //models.insert (make_pair (bodyid, new cModelEntry (bodyid, 0, animid, 0, entries[8])));
        //unsigned int defhue = atoi(tempstr.c_str());

        //printf("%i %i %s\n", bodyid, animid, entries[8].c_str());
        //}

      }
}


cStitchinLoader::~cStitchinLoader ()
{
  std::map < int, cModelEntry * >::iterator iter;
  for (iter = models.begin (); iter != models.end (); iter++)
    delete iter->second;
  models.clear ();
}

//
cModelEntry *cStitchinLoader::GetModel (int id)
{

  std::map < int, cModelEntry * >::iterator iter;
  iter = models.find (id);
  if (iter != models.end ())
      {
        return iter->second;
      }
  else
    return NULL;

}

// Temporary function to resolve some ugly graphics
/*bool isException(int modid)
{
 switch(modid){
  case 449:
  case 971:
  case 447: return true; break;
  default: return false;
}  
}*/
