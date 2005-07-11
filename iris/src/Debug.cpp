//
// File: Debug.cpp
// Created by: Alexander Oster - tensor@ultima-iris.de
// Modified by: DarkStorm (4.3.2003)
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
#include <string>
#include "Debug.h"

#define _DEBUG

Debug pDebug;

using namespace std;

Debug::Debug ()
{
  loglevel = LEVEL_WARNING;
}

Debug::~Debug ()
{
// TODO: Place Destructor Here
}

// Only print to stderr in Debug mode, otherwise redirect to file (debug.txt)
#ifdef WIN32
bool logToFile = true;
#else
bool logToFile = false;
#endif

void Debug::Log (char *message, char *filename, int line, int level)
{
  // Ignore messages below our loglevel
  if (level < loglevel)
    return;

  std::string errorMsg;

  switch (level)
      {
      case LEVEL_WARNING:
        errorMsg = "Warning ";
        break;
      case LEVEL_ERROR:
        errorMsg = "Error ";
        break;
      case LEVEL_CRITICAL:
        errorMsg = "CRITICAL Error ";
        break;
      }

  char lineStr[32];

  sprintf (lineStr, "%d", line);

  errorMsg +=
    "(" + string (filename) + "," + string (lineStr) + "): " +
    string (message) + "\n";

  // Dump the message out to a file
  if (logToFile)
      {
        FILE *fp = fopen ("debug.txt", "at");

        if (fp)
            {
              fprintf (fp, errorMsg.c_str ());
              fclose (fp);
            }
      }
  else
      {
        fprintf (stderr, errorMsg.c_str ());
      }
}

void Debug::Log (char *message)
{

  // Dump the message out to a file
  if (logToFile)
      {
        FILE *fp = fopen ("debug.txt", "at");

        if (fp)
            {
              fprintf (fp, "%s\n", message);
              fclose (fp);
            }
      }
  else
      {
        fprintf (stderr, "%s\n", message);
      }
}

int Debug::GetLoglevel (void)
{
  return loglevel;
}

void Debug::SetLoglevel (int loglevel)
{
  this->loglevel = loglevel;
}
