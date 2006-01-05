//
// File: SDLScreen.h
// Created by: Gustav Nylander - blobba@ultima-iris.de
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


#include "renderer/SDLScreen.h"
#include "Config.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_net.h"
#include "loaders/HueLoader.h"

GLfloat mat_specular[] = { 0.05, 0.05, 0.05, 1.0 };
GLfloat mat_shininess[] = { 0.3 };
GLfloat mat_ambient[] = { 0.4, 0.4, 0.4, 1.0 };
GLfloat mat_diffuse[] = { 0.3, 0.3, 0.3, 1.0 };

GLfloat fade_specular[] = { 0.05, 0.05, 0.05, 1.0 };
GLfloat fade_ambient[] = { 0.4, 0.4, 0.4, 1.0 };
GLfloat fade_diffuse[] = { 0.3, 0.3, 0.3, 1.0 };

  /* Ambient Light Values */
GLfloat LightAmbient[] = { 0.7f, 0.7f, 0.7f, 1.0f };

  /* Diffuse Light Values */
GLfloat LightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };

  /* Light Position */
GLfloat LightPosition[] = { -1.0f, 1.0f, 1.0f, 0.0f };

int flag_cullbackfaces = 0;

SDLScreen::SDLScreen ()
{
  act_alpha = 255;
  ratio = 1.0f;
  light_factor = 1.0f;
  screen = NULL;
  Init (nConfig::width, nConfig::height, nConfig::bpp);
  ResizeWindow (nConfig::width, nConfig::height);
  InitGL ();
}

SDLScreen::~SDLScreen ()
{
  if (screen)
    SDL_FreeSurface (screen);
  ClearFonts ();
  TTF_Quit ();
  SDL_QuitSubSystem (SDL_INIT_VIDEO);
}

int SDLScreen::Init (int width, int height, int bpp)
{

  /* initialize SDL video and SDL event handling */
  if (SDL_InitSubSystem( SDL_INIT_VIDEO ) == -1) {
     return (-1);
  } 

  /* Fetch the video info */
  videoInfo = SDL_GetVideoInfo ();
  if (!videoInfo)
      {
        cerr << "Video query failed: " << SDL_GetError ();
        return (1);
      }

  /* the flags to pass to SDL_SetVideoMode */
  videoFlags = SDL_OPENGL;      /* Enable OpenGL in SDL */
  videoFlags |= SDL_HWPALETTE;  /* Store the palette in hardware */
  if (nConfig::startfullscreen)
    videoFlags |= SDL_FULLSCREEN;

#ifndef WIN32
  videoFlags |= SDL_RESIZABLE;        /* Enable window resizing */
#endif

  /* This checks to see if surfaces can be stored in memory */
  if (videoInfo->hw_available)
      {
        videoFlags |= SDL_HWSURFACE;
      }
  else
      {
        videoFlags |= SDL_SWSURFACE;
      };

  /* Sets up OpenGL double buffering */
  SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute (SDL_GL_DEPTH_SIZE, nConfig::depthbuffer);

  // Set name of application
  SDL_WM_SetCaption ("Iris", NULL);

  /* get a SDL surface */
  screen = SDL_SetVideoMode (width, height, bpp, videoFlags);

  /* Verify there is a surface */
  if (!screen)
      {
        cerr << "Video mode set failed: " << SDL_GetError () << endl;
        exit (1);
      }

  /* Enable key repeat */
  if ((SDL_EnableKeyRepeat
       (SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL)))
      {
        cerr << "Setting keyboard repeat failed: " << SDL_GetError () << endl;
        exit (1);
      }

  if (TTF_Init () < 0)
      {
        cerr << "TTF Initialisation failed: " << SDL_GetError () << endl;
      }

  SDL_EnableUNICODE (1);

  if (nConfig::cursor == 1)
    SDL_ShowCursor (SDL_DISABLE);

  return true;
}

/* function to reset our viewport after a window resize */
int SDLScreen::ResizeWindow (int width, int height)
{
  if (!screen)
    return true;

  /* Protect against a divide by zero */
  if (height == 0)
    height = 1;

  ratio = (GLfloat) width / (GLfloat) height;

  /* Setup our viewport. */
  glViewport (0, 0, (GLint) width, (GLint) height);

  SetPerspective ();

  return (true);
}

void SDLScreen::ToggleFullScreen ()
{
  if (!screen)
    return;

  SDL_WM_ToggleFullScreen (screen);
}

/* general OpenGL initialization function */
int SDLScreen::InitGL (GLvoid)
{
  if (!screen)
    return true;

  glClearColor (0.0, 0.0, 0.0, 1.0);
  glEnable (GL_CULL_FACE);

  glClearDepth (1.0);           // Enables Clearing Of The Depth Buffer
  glDepthFunc (GL_LESS);        // The Type Of Depth Test To Do
  glEnable (GL_DEPTH_TEST);     // Enables Depth Testing
  glShadeModel (GL_SMOOTH);     // Enables Smooth Color Shading
  
  glMatrixMode (GL_PROJECTION);
  glEnable (GL_TEXTURE_2D);

/*  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

  glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);

  glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);


  glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);
  glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, LightPosition);
  glLightfv(GL_LIGHT0, GL_POSITION, LightPosition);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
*/

  GLuint fogMode[]= { GL_EXP, GL_EXP2, GL_LINEAR }; // Storage For Three Types Of Fog
  GLuint fogfilter= 2; // Which Fog To Use
  GLfloat fogColor[4] = { 168.0f/255.0f, 168.0f/255.0f, 180.0f/255.0f , 1.0f }; // skybox
  glEnable(GL_FOG);
  glClearColor(0.5f,0.5f,0.5f,1.0f); // We'll Clear To The Color Of The Fog
  glFogi(GL_FOG_MODE, fogMode[fogfilter]); // Fog Mode
//  glFogfv(GL_FOG_COLOR, fogColor); // Set Fog Color
  glFogf(GL_FOG_DENSITY, 0.45f); // 0.35f How Dense Will The Fog Be
  glHint(GL_FOG_HINT, GL_DONT_CARE); // Fog Hint Value
  glMatrixMode (GL_MODELVIEW);
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glEnable(GL_BLEND);

//  glEnableClientState(GL_VERTEX_ARRAY);
//  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//  glEnableClientState(GL_NORMAL_ARRAY);

  glAlphaFunc (GL_GREATER, 0.9);
  glEnable (GL_ALPHA_TEST);
  return (true);
}

void SDLScreen::ClearScreen ()
{
  if (!screen)
    return;
  /* Clear The Screen And The Depth Buffer */
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void SDLScreen::ClearZBuffer ()
{
  if (!screen)
    return;
  /* Clear The Depth Buffer */
  glClear (GL_DEPTH_BUFFER_BIT);
}

/* Blit the Screen*/

int SDLScreen::DrawGL (GLvoid)
{
  if (!screen)
    return true;

  //Display FPS
  DisplayFps ();

  // Draw Screen
  glFinish ();
  SDL_GL_SwapBuffers ();
  return (true);
}

void SDLScreen::DisplayFps ()
{
  if (!screen)
    return;
  static float framesPerSecond = 0.0f;    // This will store our fps
  static float lastTime = 0.0f; // This will hold the time from the last frame
  static char strFrameRate[40] = { "Fps: " };

  float currentTime = SDL_GetTicks () * 0.001f;

  ++framesPerSecond;
  if (currentTime - lastTime > 1)
      {
        lastTime = currentTime;
        // Copy the frames per second into a string to display in the window title bar
        sprintf (strFrameRate, "Iris, fps: %d", int (framesPerSecond));

        // Set the window title bar to our string
        SDL_WM_SetCaption (strFrameRate, NULL); // The First argument is the window title
        // Reset the frames per second
        framesPerSecond = 0;

      };
}

int SDLScreen::ScreenSave ()
{
  if (!screen)
    return 1;
#ifndef WIN32
  int screenInfo[4];

  glGetIntegerv (GL_VIEWPORT, screenInfo);

  unsigned char *image;
  image = new unsigned char[screenInfo[2] * screenInfo[3] * 3];

  glReadPixels (0, 0, screenInfo[2], screenInfo[3], GL_BGR,
                GL_UNSIGNED_BYTE, image);

  printf ("creating screenshot...\n");

  ofstream screenshot;

  screenshot.open ("screenshot.tga", ios::out | ios::binary);

  if (!screenshot.is_open ())
      {
        cerr << "Could not open file writing\n";
        return 0;
      };

  unsigned char Fileheader[12] = { 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
  unsigned char Imageheader[6] = { (screenInfo[2] % 256),
    (screenInfo[2] / 256),
    (screenInfo[3] % 256),
    (screenInfo[3] / 256), 24, 0
  };

  screenshot.write ((char *) &Fileheader, sizeof (Fileheader));
  screenshot.write ((char *) &Imageheader, sizeof (Imageheader));
  screenshot.write ((char *) image, screenInfo[2] * screenInfo[3] * 3);

  screenshot.close ();
  delete[]image;
#endif

  return 1;
}



void SDLScreen::SetPerspective ()
{
  if (!screen)
    return;
  /* change to the projection matrix and set our viewing volume. */
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();

  gluPerspective (45.0f, ratio, 1.0f, 100.0f);
//SiENcE:  gluPerspective(45.f, 800.f / 600.f, 1.f, 60.0f);

  /* Make sure we're chaning the model view and not the projection */
  glMatrixMode (GL_MODELVIEW);

  /* Reset The View */
  glLoadIdentity ();
  /* Set our perspective */
}

float SDLScreen::GetRatio (void)
{
  return ratio;
}

void SDLScreen::SetAlpha (Uint8 alpha, bool force)
{
  if ((alpha == act_alpha) && !force)
    return;

  act_alpha = alpha;
  float value = 1.0f;

  if (act_alpha != 255)
    value = act_alpha / 255.0f;

/*	fade_specular[3] = value;
	fade_ambient[3] = value;
	fade_diffuse[3] = value;*/


/*	glMaterialfv(GL_FRONT, GL_SPECULAR, fade_specular);
	glMaterialfv(GL_FRONT, GL_AMBIENT, fade_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, fade_diffuse);*/
  glColor4f (1.0f, 1.0f, 1.0f, value);
  glAlphaFunc (GL_GREATER, 0.9f * value);

}

void SDLScreen::SetHue (Uint16 hue)
{
  float facs[4];
  int i;

  if (hue)
      {
        const struct stHue *sthue = pHueLoader.getHue (hue);
        if (!sthue)
          hue = 0;
        else
            {
              Uint32 col32 = sthue->colors[31];
              Uint8 *col8 = (Uint8 *) & col32;
              for (i = 0; i < 3; i++)
                facs[i] = (float) col8[i] / 255.0f * light_factor;

            }
      }

  if (!hue)
    for (i = 0; i < 3; i++)
      facs[i] = light_factor;

  facs[3] = (float) act_alpha / 255.0f;


  glColor4f (facs[0], facs[1], facs[2], facs[3]);
/*    GLfloat hue_specular[4];
    GLfloat hue_ambient[4];
    GLfloat hue_diffuse[4];

    for (i = 0; i < 4; i++) {
	  hue_specular[i] = mat_specular[i] * facs[i];
	  hue_ambient[i] = mat_ambient[i] * facs[i];
	  hue_diffuse[i] = mat_diffuse[i] * facs[i];
	}
	

  glMaterialfv(GL_FRONT, GL_SPECULAR, hue_specular);
  glMaterialfv(GL_FRONT, GL_AMBIENT, hue_ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, hue_diffuse); */

}

void SDLScreen::SetLight (float factor)
{
/*  GLfloat light[4];
  int i;
  for (i = 0; i < 3; i++) {
  	light [i] = LightAmbient[i] * factor;
  }
  light[3] = 1.0f;
  
  glLightfv(GL_LIGHT0, GL_AMBIENT, light);
  light_factor = factor * 1.5f;
  light_factor += nConfig::brightness * 0.1f;
  if (light_factor > 1.5f)
  	light_factor = 1.5f; 
        
  for (i = 0; i < 3; i++) {
     fade_specular[i] = mat_specular[i] * light_factor;
     fade_ambient[i] = mat_ambient[i] * light_factor;
     fade_diffuse[i] = mat_diffuse[i] * light_factor;
  }
  SetAlpha(act_alpha, true); */
}


void SDLScreen::ClearFonts ()
{
  std::map < Uint32, TTF_Font * >::iterator iter;
  for (iter = fonts.begin (); iter != fonts.end (); iter++)
    TTF_CloseFont (iter->second);
  fonts.clear ();
}

void SDLScreen::RegisterFont (Uint32 id, std::string filename, Uint32 size,
                              Uint16 defaulthue)
{
  UnregisterFont (id);
  TTF_Font *font = TTF_OpenFont (filename.c_str (), size);
  if (!font)
      {
        cerr << "TTF Load failed: " << SDL_GetError () << endl;
        return;
      }
  fonts.insert (make_pair (id, font));

  default_hues.erase (id);
  if (defaulthue)
      {
        default_hues.insert (make_pair (id, defaulthue));
      }
}

void SDLScreen::UnregisterFont (Uint32 id)
{
  std::map < Uint32, TTF_Font * >::iterator iter;
  iter = fonts.find (id);
  if (iter != fonts.end ())
      {
        TTF_CloseFont (iter->second);
        fonts.erase (iter);
      }
}

TTF_Font *SDLScreen::GetFont (Uint32 id)
{
  std::map < Uint32, TTF_Font * >::iterator iter;
  iter = fonts.find (id);
  if (iter != fonts.end ())
    return iter->second;
  else
    return NULL;
}

Uint16 SDLScreen::GetDefaultHue (Uint32 id)
{
  std::map < Uint32, Uint16 >::iterator iter;
  iter = default_hues.find (id);
  if (iter != default_hues.end ())
    return iter->second;
  else
    return 0;
}
