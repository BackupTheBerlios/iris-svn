//
// File: ParticleEngine.h
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

#ifndef _PARTICLEENGINE_H_
#define _PARTICLEENGINE_H_

#ifdef WIN32
#include <windows.h>
#endif

#include "SDL/SDL.h"
#include <map>

#include "loaders/StaticModels.h"
#include "renderer/particles/ParticleHandler.h"

class cParticleEngine 
{
  private:
    Uint32 current_handle;
    std::map <Uint32, Particle::cParticleHandler *> particle_handlers;
  public:
    cParticleEngine ();
    ~cParticleEngine ();

    void Reset ();
    
    void Handle ();
    void Render();
    
    Uint32 AddEffect (cStaticModelParticleEffectInfo * effect_info, float x, float y, float z);
    void RemoveEffect (Uint32 id);
};

extern cParticleEngine pParticleEngine;


#endif //_PARTICLEENGINE_H_
