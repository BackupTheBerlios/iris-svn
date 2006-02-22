//
// File: SpeechLoader.cpp
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
#include "Logger.h"
#include "Config.h"
#include "loaders/SpeechLoader.h"
#include "string_utils.h"
#include <string>
#include <SDL/SDL_endian.h>

cSpeechLoader pSpeechLoader;

cSpeechLoader::cSpeechLoader ()
{
}

cSpeechLoader::~cSpeechLoader ()
{
    DeInit ();
}

void cSpeechLoader::Init (std::string path)
{
	std::ifstream Speechfile;

	std::string filename = path + "speech.mul";
	std::cout << "\t| -> Speech file: " << filename << std::endl;


  Speechfile.open (filename.c_str (), std::ios::in | std::ios::binary);

  if (!Speechfile.is_open ())
      {
        Logger::WriteLine ("\t| -> Warning: Couldn't open Speech file");
        Speechfile.close ();
        //nConfig::Speech_mul = 0;
        return;
      }

  Speechfile.seekg (0, std::ios::end);
  int filelen = Speechfile.tellg ();
  Speechfile.seekg (0, std::ios::beg);

  Uint16 index;
  Uint16 keywordlen;
  char * c_keyword;
  std::string s_keyword = "";
  
 bool first_lan= true;

  while (Speechfile.tellg () < filelen)
  {
       
        Speechfile.read ((char *) &index, 2);
        index = SDL_Swap16 (index);
        if(index == 0 && first_lan)
         first_lan = false;
        else{
           m_languages.push_back(m_keywords); 
           m_keywords.clear(); 
          }
        Speechfile.read ((char *) &keywordlen, 2);
        keywordlen=SDL_Swap16 (keywordlen);
        c_keyword = new char[keywordlen + 1];
        Speechfile.read ((char *) c_keyword, keywordlen);
        c_keyword[keywordlen] = 0;
        s_keyword = std::string (c_keyword);
        delete c_keyword;
        //std::cout << "ID: " << index << " WORD: " << s_keyword << "   len: " << keywordlen << std::endl;
        //Logger::WriteLine(s_keyword.c_str());
        m_keywords.insert (make_pair ( s_keyword, index));
  }
  Speechfile.close ();
}

void cSpeechLoader::DeInit ()
{
 m_keywords.clear();
 m_languages.clear();
}

Uint16 cSpeechLoader::GetID (std::string keyword)
{
 for(int i=0;i<m_languages.size();i++){
  speech_language act_lang = m_languages.at(i);       
  std::map < std::string, Uint16 >::iterator iter;
  iter = act_lang.find (keyword);
  if (iter != act_lang.end ())
    return iter->second;
 }
  return 0xFFFF;
  
}

std::vector<Uint16> cSpeechLoader::GetIDs (std::string keyword)
{
 std::vector<Uint16> result;
 for(int i=0;i<m_languages.size();i++){
  speech_language act_lang = m_languages.at(i);       
  std::map < std::string, Uint16 >::iterator iter;
  iter = act_lang.find (keyword);
  if (iter != act_lang.end ())
    result.push_back(iter->second);
 }
 if(result.size()>0)
  return result;
  
 //return 0xFFFF;
}

