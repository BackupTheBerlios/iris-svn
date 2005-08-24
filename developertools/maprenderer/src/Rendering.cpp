#include "Rendering.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <windows.h>
#include <process.h>
#include "Config.h"
#include "Game.h"
#include "loaders/Map.h"
#include "SDLEvent.h"
#include "renderer/SDLScreen.h"
#include "Image.h"
#include "IMG_savepng.h"


#define BMPFILE 1
#define PNGFILE 2

extern HWND FromX, FromY, ToX, ToY, hMainWnd, Detail, FileName, Memo;
extern int detail, divisor;
extern SDLScreen *SDLscreen;
extern bool ReadyToPaint, IsRendering;
extern HBITMAP hBitmap;
extern HPALETTE hPalette;
extern HWND StartButton;
int outputType = 0;


unsigned int __stdcall RunRenderer( void*)
{
  char *text;
  char *output, *out,gettext[100000],filen[1024];
  text = (char*)malloc(5);
  memset(text,0,5);
  char pngfile[1024];

  
  output = (char*)malloc(256);
  memset(output, 0, 5);
  out = (char*)malloc(100000);
  memset(out, 0, 100000);
  memset(gettext, 0, 100000);
  int len,ind=0;
  char ext[5];
  memset(ext,0,5);
  
  
  SendMessage(FromX,WM_GETTEXT, 5, (LPARAM)text);
  nConfig::minx = atoi(text);

  SendMessage(FromY,WM_GETTEXT, 5, (LPARAM)text);
  nConfig::miny = atoi(text);

  SendMessage(ToX,WM_GETTEXT, 5, (LPARAM)text);
  nConfig::maxx = atoi(text);

  SendMessage(ToY,WM_GETTEXT, 5, (LPARAM)text);
  nConfig::maxy = atoi(text);

  SendMessage(Detail,WM_GETTEXT, 5, (LPARAM)text);
  nConfig::detail = atoi(text);

  memset(filen, 0, 1024);
  SendMessage(FileName,WM_GETTEXT, 1024, (LPARAM)filen);
  
  sprintf(pngfile,"%s",filen);
  sprintf(ext,"%s", strtok(filen, "."));
  sprintf(ext,"%s", strtok(0,"."));

  if(strcmp(ext,"bmp") == 0 )
  {
      outputType = BMPFILE;
      sprintf(filen, "%s.bmp",filen);
  }
  else
  {
  	  outputType = PNGFILE;
      sprintf(filen, "%s.png",filen);
  }
  nConfig::output = filen;
  SetWindowText(FileName,filen);

  if( ((nConfig::maxx - nConfig::minx) < 50) || ((nConfig::maxy - nConfig::miny) < 50) )
  {
      _endthreadex(1);
  }

  nConfig::minblockx = nConfig::minx / 8;
  nConfig::minblocky = nConfig::miny / 8;
  nConfig::maxblockx = nConfig::maxx / 8;
  nConfig::maxblocky = nConfig::maxy / 8;
     
  detail = nConfig::detail;
  if (detail < 0) detail = 0;
  if (detail > 9) detail = 9;
  divisor = 1 << detail;
     
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
  
  if( (real_mem * 3) > 134217728 )
  {
      if(MessageBox(hMainWnd,"You're about to create a bitmap, which will use more than 128MB. Proceed anyway?","Question", MB_YESNO) == IDNO )
      {
            pGame.DeInit();
            _endthreadex(2);
      }
  }
  
  int blockcount = real_mem / (1024 * 1024);
  
  Uint8 * data = (Uint8 *) malloc (1024 * 1024 * 3);
  memset(data, 0, 1024 * 1024 * 3);
  
  sprintf(out,"Creating Bitmap (%i/%i): %i bytes (Zoom 1/%i)....\r\n", (*width), (*height), real_mem * 3, divisor);
  SetWindowText(Memo,out);

  Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    rmask = 0xff000000;
    gmask = 0x00ff0000;
    bmask = 0x0000ff00;
    amask = 0x000000ff;
#else
    rmask = 0x000000ff;
    gmask = 0x0000ff00;
    bmask = 0x00ff0000;
    amask = 0xff000000;
#endif
    
  SDL_Surface *png = SDL_CreateRGBSurface(SDL_SWSURFACE, t_width, t_height, 32, rmask, gmask, bmask, amask); 
  SDL_Rect srect,drect;
  srect.x=0;
  srect.y=0;
  srect.w=1024;
  srect.h=1024; 
  drect.w=1024;
  drect.h=1024;
  ofstream * file;
  
  if(outputType==BMPFILE)
  {
	  file = new ofstream(nConfig::output.c_str(), ios::out | ios::binary);
  	  file->write((char*) bmpheader, 54);
  	  for (int i = 0; i < blockcount; i++)
          file->write((char*) data, 1024 * 1024 * 3);
    
  	  Uint32 rest = real_mem % (1024 * 1024);
  	  if (rest > 0) 
          file->write((char*) data, rest * 3);
  }

  SendMessage(Memo, WM_GETTEXT, 100000, (LPARAM)gettext);
  sprintf(out,"%sRendering Area (%i/%i) to (%i/%i)\r\n", gettext, nConfig::minx, nConfig::miny, nConfig::maxx, nConfig::maxy);
  SetWindowText(Memo,out);
  
  int bxcount = t_width / 1024;
  int bycount = t_height / 1024;
  
  int size = 1024 / divisor;

  SendMessage(Memo, WM_GETTEXT, 100000, (LPARAM)gettext);
  sprintf(out,"%sRendering %i blocks ", gettext, (bxcount + 1) * (bycount + 1));
  SetWindowText(Memo,out);  
  
  int idx = 0;

  SendMessage(Memo, WM_GETTEXT, 100000, (LPARAM)gettext);
  for (int blocky = 0; blocky <= bycount; blocky ++){
     if(!IsRendering)break;     
     for (int blockx = 0; blockx <= bxcount; blockx ++) {
       if(!IsRendering)break;
       int xcount = size; if (blockx == bxcount) xcount = (t_width % 1024) / divisor;
       int ycount = size; if (blocky == bycount) ycount = (t_height % 1024) / divisor;
       sprintf(out,"%s ... %i ", gettext, idx+1);
       SetWindowText(Memo,out);  
       idx++;
       
       SDL_FillRect(SDLscreen->screen, NULL, 0);
       pGame.GetRenderer()->Rebuild(- blockx * 1024,- blocky * 1024);
       
       drect.x = blockx*1024;
       drect.y = blocky*1024;
  	   SDL_BlitSurface( SDLscreen->screen, &srect, png, &drect);	   
       if(outputType == BMPFILE)
       {
       
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
	              
        }
  	 }
   }
   if(IsRendering == false)
	 _endthreadex(3);

   if(outputType==PNGFILE) IMG_SavePNG(nConfig::output.c_str(),png,5);

   if(outputType==BMPFILE) delete file;
   free(data);
   free(png);

  pGame.DeInit();
  _endthreadex(0);
}


unsigned int __stdcall RunPreview( void*)
{
  if(hBitmap)
    DeleteObject(hBitmap);
  if(hPalette)
    DeleteObject(hPalette);
    
  ReadyToPaint = false;
  SetWindowText(Memo, "Calculating Preview. Please wait...");
  char *text;
  char *output, *out,gettext[100000];
  text = (char*)malloc(5);
  memset(text,0,5);
  
  output = (char*)malloc(256);
  memset(output, 0, 5);
  out = (char*)malloc(100000);
  memset(out, 0, 100000);
  memset(gettext, 0, 100000);
  
  
  SendMessage(FromX,WM_GETTEXT, 5, (LPARAM)text);
  nConfig::minx = atoi(text);

  SendMessage(FromY,WM_GETTEXT, 5, (LPARAM)text);
  nConfig::miny = atoi(text);

  SendMessage(ToX,WM_GETTEXT, 5, (LPARAM)text);
  nConfig::maxx = atoi(text);

  SendMessage(ToY,WM_GETTEXT, 5, (LPARAM)text);
  nConfig::maxy = atoi(text);

  if( ((nConfig::maxx - nConfig::minx) < 50) || ((nConfig::maxy - nConfig::miny) < 50) )
  {
      MessageBox(hMainWnd,"These coordinates doesn't make sense. Please choose a minimum difference of 50 between From & To.","Warning",MB_ICONEXCLAMATION);
      ReadyToPaint = true;
      _endthreadex(0);
  }

  nConfig::minblockx = nConfig::minx / 8;
  nConfig::minblocky = nConfig::miny / 8;
  nConfig::maxblockx = nConfig::maxx / 8;
  nConfig::maxblocky = nConfig::maxy / 8;

     
  detail = 5;
  if (detail < 0) detail = 0;
  if (detail > 9) detail = 9;
  divisor = 1 << detail;
     
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
  if( (real_mem * 3) > 134217728 )
  {
      if(MessageBox(hMainWnd,"You're creating now a bitmap, which will use more than 128MB. Are you sure to proceed?","Question", MB_YESNO) == IDNO )
      {
            sprintf(out,"Rendering canceled!");
            SetWindowText(Memo,out);
            SetWindowText(StartButton, "Start Rendering");
            pGame.DeInit();
            _endthreadex(0);
      }
  }
  
  int blockcount = real_mem / (1024 * 1024);
  
  Uint8 * data = (Uint8 *) malloc (1024 * 1024 * 3);
  memset(data, 0, 1024 * 1024 * 3);
  
  ofstream * file = new ofstream("preview.bmp", ios::out | ios::binary);
  file->write((char*) bmpheader, 54);
  for (int i = 0; i < blockcount; i++)
    file->write((char*) data, 1024 * 1024 * 3);
    
  Uint32 rest = real_mem % (1024 * 1024);
  if (rest > 0) 
    file->write((char*) data, rest * 3);
   
  int bxcount = t_width / 1024;
  int bycount = t_height / 1024;
  
  int size = 1024 / divisor;
   
  int idx = 0;

  for (int blocky = 0; blocky <= bycount; blocky ++)
     for (int blockx = 0; blockx <= bxcount; blockx ++) {
       int xcount = size; if (blockx == bxcount) xcount = (t_width % 1024) / divisor;
       int ycount = size; if (blocky == bycount) ycount = (t_height % 1024) / divisor;
       
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
  SetWindowText(Memo, "Preview ready!");
   delete file;
   free(data);
  if( !LoadBitmapFromBMPFile( "preview.bmp", &hBitmap, &hPalette ) )
  {
        sprintf(out, "LoadBitmapFromBMPFile() failed: %s",GetLastError());
        SetWindowText(Memo,out);
  
  }
  else
  {
        RedrawWindow(hMainWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
        ReadyToPaint = true;
  }
        
  pGame.DeInit();
  

  RedrawWindow(hMainWnd, NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
  _endthreadex(0);  
}
