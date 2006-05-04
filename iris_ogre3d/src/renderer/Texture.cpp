//
// File: Texture.cpp
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


#include <iostream>
#include "renderer/Texture.h"
#include "Logger.h"
#include "SDL/SDL_image.h"
#include <cassert>
#include <Ogre.h>
#include "pathsearch.h" // by ghoulsblade
#include "ogrewrapper.h" // by ghoulsblade




using namespace std;

Texture::Texture() : gltex( 0 ), width( 0 ), height( 0 ), assigned( false ), bitmask( NULL )
{PROFILE

}

Texture::~Texture()
{PROFILE/* TODO : ghoulsblade : port me to ogre, opengl commands are not available directly
  
	if ( assigned && gltex )
	{
		glDeleteTextures( 1, &gltex );
	}
	*/
	gltex = 0;

	SAFE_DELETE( bitmask );
}

int Texture::GetGLTex ()
{PROFILE
	return gltex;
}

bool Texture::GetAssigned (void)
{PROFILE
	return assigned;
}

const char* Texture::GetModelMaterial () {PROFILE
	if (msModelMaterialName.size() == 0 && msTextureName.size() > 0) {
		// Create material for use by the uo-granny models
		msModelMaterialName = cOgreWrapper::GetUniqueName();
		Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(msModelMaterialName, "General");
		material->getTechnique(0)->getPass(0)->createTextureUnitState(msTextureName);
		material->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBF_SOURCE_ALPHA,Ogre::SBF_ONE_MINUS_SOURCE_ALPHA);
		material->getTechnique(0)->getPass(0)->setAlphaRejectSettings(Ogre::CMPF_GREATER,230); // 0.9*255 ~ 230
		//material->getTechnique(0)->getPass(0)->getTextureUnitState(0)->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
		material->load();
	}
	if (msModelMaterialName.size() > 0)
			return msModelMaterialName.c_str();
	else	return "BaseWhiteNoLighting";
}


const char* Texture::GetGUIMaterial () {PROFILE
	if (msGUIMaterialName.size() == 0 && msTextureName.size() > 0) {
		//printf("Texture::GetGUIMaterial : msImageFile='%s'\n",msImageFile.c_str());
		// Create material for GUI (2d use)
		msGUIMaterialName = cOgreWrapper::GetUniqueName();
		Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().create(msGUIMaterialName, "General");
		material->getTechnique(0)->getPass(0)->createTextureUnitState(msTextureName);
		material->getTechnique(0)->getPass(0)->setDepthCheckEnabled(false);
		material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);
		material->getTechnique(0)->getPass(0)->setLightingEnabled(false);
		//material->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_MODULATE);
		material->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBF_SOURCE_ALPHA,Ogre::SBF_ONE_MINUS_SOURCE_ALPHA);
		//material->getTechnique(0)->getPass(0)->setAlphaRejectSettings(Ogre::CMPF_GREATER,1);
		material->load();
		/*
		SBT_TRANSPARENT_ALPHA 	Make the object transparent based on the final alpha values in the texture.
		SBT_TRANSPARENT_COLOUR 	Make the object transparent based on the colour values in the texture (brighter = more opaque).
		SBT_ADD 	Add the texture values to the existing scene content.
		SBT_MODULATE 	Multiply the 2 colours together.
		SBT_REPLACE 	The default blend mode where source replaces destination.
		
		SBF_ONE 	
		SBF_ZERO 	
		SBF_DEST_COLOUR 	
		SBF_SOURCE_COLOUR 	
		SBF_ONE_MINUS_DEST_COLOUR 	
		SBF_ONE_MINUS_SOURCE_COLOUR 	
		SBF_DEST_ALPHA 	
		SBF_SOURCE_ALPHA 	
		SBF_ONE_MINUS_DEST_ALPHA 	
		SBF_ONE_MINUS_SOURCE_ALPHA 
		*/
	}
	if (msGUIMaterialName.size() > 0)
			return msGUIMaterialName.c_str();
	else	return "BaseWhiteNoLighting";
}


int Texture::LoadFromData (void *data, int width, int height,
                           int bits_per_pixel, int filtertype,
                           bool do_bitmask)
{PROFILE
	msModelMaterialName = "";
  GLuint texture;

  if ((bits_per_pixel != 24) && (bits_per_pixel != 32))
      {
        Logger::WriteLine("Wrong Pixelformat in Texture::LoadFromData(void *, int, int, int)",
           __FILE__, __LINE__, LEVEL_ERROR);
        return (false);
      }

	  
	 
	msTextureName = cOgreWrapper::GetUniqueName();
	Ogre::DataStreamPtr imgstream(new Ogre::MemoryDataStream(data, width * height * (bits_per_pixel/8)));
	Ogre::TexturePtr tex = Ogre::TextureManager::getSingleton().loadRawData(msTextureName,
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			//imgstream, width, height, (bits_per_pixel == 32) ? Ogre::PF_A8B8G8R8 : Ogre::PF_B8G8R8);
			imgstream, width, height, (bits_per_pixel == 32) ? Ogre::PF_BYTE_RGBA  : Ogre::PF_BYTE_RGB);
	tex->load();
	imgstream->close ();
	
	// PF_R8G8B8.
	// TODO : 24 bit color ordering has not yet been tested, maybe PF_B8G8R8 or PF_R8G8B8  (red and blue switched)
	// TODO : filtertype is ignored for now...

  gltex = 0;
	  
	  if (0) {
		  /* TODO : ghoulsblade : port me to ogre, opengl commands are not available directly
  
			// ghoulsblade : replaced by the ogre code above
		  if (assigned && gltex)
			glDeleteTextures (1, &gltex);
		  gltex = 0;

		  glGenTextures (1, &texture);
		  glBindTexture (GL_TEXTURE_2D, texture);

		  glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);    // Texture blends with object background 

		  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtertype);   // only first two can be used  
		  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);  // all of the above can be used 

		  if (bits_per_pixel == 32)
			gluBuild2DMipmaps (GL_TEXTURE_2D, GL_RGBA, width, height, GL_RGBA,
							   GL_UNSIGNED_BYTE, data);
		  else
			gluBuild2DMipmaps (GL_TEXTURE_2D, 3, width, height, GL_RGB,
							   GL_UNSIGNED_BYTE, data);

		   //Select a filtering type. BiLinear filtering produces very good results with little performance impact
			 //GL_NEAREST               - Basic texture (grainy looking texture)
			 //GL_LINEAR                - BiLinear filtering
			 //GL_LINEAR_MIPMAP_NEAREST - Basic mipmapped texture
			 //GL_LINEAR_MIPMAP_LINEAR  - BiLinear Mipmapped texture
		   */

			gltex = texture;
	  }
	assigned = true;
	this->width = width;
	this->height = height;

	if ( do_bitmask && ( bits_per_pixel == 32 ) )
	{
		SAFE_DELETE( bitmask );
		bitmask = new cBitmask();
		assert( bitmask );
		bitmask->Create( (Uint32 *)data, width, height );
	}

	return true;
}



int Texture::LoadFromSurface (SDL_Surface * surface, int filter)
{PROFILE
  if (!surface)
      {
        Logger::WriteLine("NULL surface pointer in Texture::LoadFromSurface(SDL_Surface *)",
           __FILE__, __LINE__, LEVEL_ERROR);
        return (false);
      }

  if (!LoadFromData
      (surface->pixels, surface->w, surface->h, surface->format->BitsPerPixel,
       filter))
      {
        Logger::WriteLine("call loadfromdata in Texture::LoadFromSurface(SDL_Surface *)",
               __FILE__, __LINE__, LEVEL_WARNING);
        return (false);
      }
  return (true);
}



int Texture::LoadFromFile (const char *filename)
{PROFILE
  SDL_Surface *image;
  image = IMG_Load (filename);

	// ghoulsblade's patch : try searching for the path with ignore case
	if (!image) {
		std::string realpath = rob_pathsearch(filename);
		if (realpath.size() > 0) image = IMG_Load(realpath.c_str());
	}
		
  if (!image)
      {
        char errorStr[512];
        sprintf (errorStr, "Error loading texture: %s", filename);
        Logger::WriteLine (errorStr, __FILE__, __LINE__, LEVEL_ERROR);
        return false;
      }

  // Problem here: TGA Files save their bits in BGRA order, so we need to
  // swap the bytes (Pixel value starts with B)
  if (image->format->Bshift == 0 && image->format->BitsPerPixel == 32)
      {
        unsigned int *data = (unsigned int *) image->pixels;

        for (unsigned int i = 0; (int) i < image->w * image->h; ++i)
            {
              unsigned int pixel = *data;

              // Convert to RGBA (Someone should find a more effective Algorithm here)
              // NeHe Says the following on (http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=33)
              // Steve Thomas Adds: I've got a little speedup in TGA loading code. It concerns switching BGR into RGB using only 3 binary operations. Instead of using a temp variable you XOR the two bytes 3 times. 
              // 1st Byte XOR 3rd Byte XOR 1st Byte XOR 3rd Byte
              // texture->imageData[cswap] ^= texture->imageData[cswap+2] ^=
              // texture->imageData[cswap] ^= texture->imageData[cswap+2];
              // Just wanted to get it work for now, it's based on per-byte access
              *data = (((pixel >> image->format->Rshift) & 0xFF)) |     // Red
                (((pixel >> image->format->Gshift) & 0xFF) << 8) |      // Green
                (((pixel >> image->format->Bshift) & 0xFF) << 16) |     // Blue
                (((pixel >> image->format->Ashift) & 0xFF) << 24);      // Alpha

              data++;
            }

        image->format->Rshift = 0;
        image->format->Gshift = 8;
        image->format->Bshift = 16;
        image->format->Ashift = 24;
      }

  int result = LoadFromSurface (image);

  SDL_FreeSurface (image);

  return result;
}

int Texture::LoadFromSurfaceWithColorKey (SDL_Surface * surface, int colorkey)
{PROFILE


  if ((surface->format->BitsPerPixel != 24) || (colorkey == -1))
      {
        int result = LoadFromSurface (surface);
        return result;
      }

  int size = surface->w * surface->h;
  Uint8 *data = (Uint8 *) malloc (size * 4);
  Uint8 *dst = data;
  Uint8 *src = (Uint8 *) surface->pixels;
  Uint8 *key = (Uint8 *) (&colorkey);

  for (int index = 0; index < size; index++)
      {
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
        int dr = src[0] - key[0];
        int dg = src[1] - key[1];
        int db = src[2] - key[2];
        int dist = (dr * dr) + (dg * dg) + (db * db);
        dst[3] = (dist < 1000 ? 0 : 255);
        dst += 4;
        src += 3;
      }

  int result = LoadFromData (data, surface->w, surface->h, 32, GL_LINEAR);

  free (data);

  return result;

}


int Texture::LoadFromFileWithTransparency (const char *filename,
                                           unsigned char transparency)
{PROFILE
  SDL_Surface *image;

  image = IMG_Load (filename);

  // ghoulsblade's patch : try searching for the path with ignore case
	if (!image) {
		std::string realpath = rob_pathsearch(filename);
		if (realpath.size() > 0) {
			image = IMG_Load(realpath.c_str());
		}
	}

if (!image)
      {
        Logger::WriteLine ("Loading Image in Texture::LoadFromFile(char *)",
                    __FILE__, __LINE__, LEVEL_ERROR);
        return (false);
      }


  if ((image->format->BitsPerPixel != 24))
      {
        int result = LoadFromSurface (image);
        SDL_FreeSurface (image);
        return result;
      }

  int size = image->w * image->h;
  Uint8 *data = (Uint8 *) malloc (size * 4);
  Uint8 *dst = data;
  Uint8 *src = (Uint8 *) image->pixels;


  for (int index = 0; index < size; index++)
      {
        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
        dst[3] = transparency;
        dst += 4;
        src += 3;
      }

  int result = LoadFromData (data, image->w, image->h, 32, GL_LINEAR);

  free (data);
  SDL_FreeSurface (image);

  return result;

}



int Texture::GetHeight (void)
{PROFILE
  return height;
}


int Texture::GetWidth (void)
{PROFILE
  return width;
}


bool Texture::CheckPixel (int x, int y, bool swapy)
{PROFILE
  if (bitmask)
      {
        return bitmask->CheckPixel (x, y, swapy);
      }
  else
    return ((x >= 0) && (x < real_width) && (y >= 0) && (y < real_height));
}
