//
// File: MapRenderer.cpp
// Created by: Alexander Oster - tensor@ultima-iris.de
//

#include <iostream>
#include "SDLEvent.h"
#include "renderer/SDLScreen.h"
#include "Debug.h"
#include "Game.h"
#include "Config.h"
#include "renderer/TextureBuffer.h"
#include "loaders/Map.h"

using namespace std;



SDLEvent *SDLevent;
SDLScreen *SDLscreen;

 

int WinMain(      
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow
)
{

  if(!nConfig::Init()) {
    pDebug.Log("Unable to load configuration file - Using defaults ", __FILE__, __LINE__,
	       LEVEL_WARNING);
  }

  
  SDLscreen = new SDLScreen();
  SDLevent = new SDLEvent;

  int detail = nConfig::detail;
  if (detail < 0) detail = 0;
  if (detail > 9) detail = 9;
  int divisor = 1 << detail;
  pGame.Init();
  
  if (nConfig::patches.size()) 
      pMapLoader->Load((char*) nConfig::patches.c_str());
  
  pGame.GetRenderer()->RecalcHeight();
  
  int t_width = ((int) ((nConfig::width + divisor * 4 - 1) / (divisor * 4))) * divisor * 4;
  int t_height = ((int) ((nConfig::height + divisor * 4 - 1) / (divisor * 4))) * divisor * 4;
  
  nConfig::width = 1024;
  nConfig::height = 1024;
  
  SDLscreen->Init(1024, 1024, 32);

  Uint8 bmpheader[54] = {
   0x42,0x4D,0xCE,0x3A,0x00,0x00,0x00,0x00,0x00,0x00,
   0x36,0x00,0x00,0x00,0x28,0x00,0x00,0x00,0x64,0x00,
   0x00,0x00,0x32,0x00,0x00,0x00,0x01,0x00,0x18,0x00,
   0x00,0x00,0x00,0x00,0x98,0x3A,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
   0x00,0x00,0x00,0x00};
   
  Uint16 * width = (Uint16 *) (bmpheader + 18);
  Uint16 * height = (Uint16 *) (bmpheader + 22);
  
  *width = t_width / divisor;
  *height = t_height / divisor;
  
  Uint32 real_mem = (Uint32) (t_width / divisor) * (Uint32) (t_height / divisor);
  
  int blockcount = real_mem / (1024 * 1024);
  
  Uint8 * data = (Uint8 *) malloc (1024 * 1024 * 3);
  memset(data, 0, 1024 * 1024 * 3);
  
  printf("Creating Bitmap (%i/%i): %i bytes (Zoom 1/%i)....\n", (*width), (*height), real_mem * 3, divisor);
  
  ofstream * file = new ofstream(nConfig::output.c_str(), ios::out | ios::binary);
  file->write((char*) bmpheader, 54);
  for (int i = 0; i < blockcount; i++)
    file->write((char*) data, 1024 * 1024 * 3);
    
  Uint32 rest = real_mem % (1024 * 1024);
  if (rest > 0) 
    file->write((char*) data, rest * 3);
  

  printf("Rendering Area (%i/%i) to (%i/%i)\n", nConfig::minx, nConfig::miny, nConfig::maxx, nConfig::maxy);
  
  int bxcount = t_width / 1024;
  int bycount = t_height / 1024;
  
  int size = 1024 / divisor;
  
  printf("Rendering %i blocks ", (bxcount + 1) * (bycount + 1));
  
  int idx = 0;

  for (int blocky = 0; blocky <= bycount; blocky ++)
     for (int blockx = 0; blockx <= bxcount; blockx ++) {
       int xcount = size; if (blockx == bxcount) xcount = (t_width % 1024) / divisor;
       int ycount = size; if (blocky == bycount) ycount = (t_height % 1024) / divisor;
       printf("...%i", idx); 
       SDL_FillRect(SDLscreen->screen, NULL, 0);
       pGame.GetRenderer()->Rebuild(- blockx * 1024,- blocky * 1024);
       
       Uint8 * pixels = (Uint8 *) SDLscreen->screen->pixels;
       if (detail > 0) {
              int w = 1024;
              int h = 1024;
        
              for (int i = 0; i < detail; i ++) {
                 w /= 2; h /= 2;
                 Uint8 * newp = (Uint8 *) malloc (w * h * 3);
                 for (int y = 0; y < h; y++) {
                    Uint8 * src = pixels + y * w * 12;
                    Uint8 * dst = newp + y * w * 3;
                    for (int x = 0; x < w; x++) {
                       *dst = (*src + *(src + 3) + *(src + w * 6) + *(src + w * 6 + 3)) / 4;                    
                       *(dst+1) = (*(src+1) + *(src + 4) + *(src + w * 6 + 1) + *(src + w * 6 + 4)) / 4;                    
                       *(dst+2) = (*(src+2) + *(src + 5) + *(src + w * 6 + 2) + *(src + w * 6 + 5)) / 4;                    
                       src += 6; dst += 3;
                    } 
                 }
                 if (i > 0)
                     free(pixels);
                 pixels = newp; 
              }
       }
  
       for (int y = 0; y < ycount; y++) {
         Uint8 * src = (pixels) + y * nConfig::width / divisor * 3;
         Uint8 * dst = data;
         for (int x = 0; x < xcount; x++) {
            *dst = *(src + 2);
            *(dst + 1) = *(src + 1);
            *(dst + 2) = *src;
            dst += 3; src += 3;
         }
         
         file->seekp(54 + (t_width / divisor * (t_height / divisor - 1 - blocky * size - y) + blockx * size)* 3, ios::beg);
         file->write((char *) data, xcount * 3);
         
       }
       
       if (detail > 0) 
              free(pixels); 
              
       idx++;
  }
  printf("...done\n");
  delete file;
  free(data);

  pGame.DeInit();
  
  delete SDLevent;
  delete SDLscreen;

  return -1;
}

