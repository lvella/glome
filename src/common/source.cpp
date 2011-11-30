/*
------------------------------------------------------------------------
This file is part of LibUFUG.
For the latest info, see http://www.games.facom.ufu.br

Copyright (c) 2006 - 2009 Universidade Federal de Uberlândia

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
------------------------------------------------------------------------
*/

#include "source.hpp"
#include <AL/al.h>

using namespace UFUG::Audio;

Source::Source(float x, float y, float z) {
   m_Source = 0;
   alGenSources(1, &m_Source);
   /*if(alGetError() != AL_NO_ERROR) {
      // TODO: Log this in DEBUG mode.
      m_Source = 0;
      return;
   }*/
   if(!m_Source)
	   return;

   alSourcef(m_Source, AL_PITCH, 1.0f);
   alSourcef(m_Source, AL_GAIN, 1.0f);
   alSource3f(m_Source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
   alSource3f(m_Source, AL_POSITION, x, y, z);
}

Source::~Source()
{
   if(m_Source)
      alDeleteSources(1, &m_Source);
}

void Source::Play(Sound *pSound, bool loop)
{
   pSound->SelfPlay(m_Source, loop);
}

void Source::SetRelative(bool relative)
{
   if(m_Source)
      alSourcei(m_Source, AL_SOURCE_RELATIVE, relative ? AL_TRUE : AL_FALSE);
}

void Source::SetPosition(float x, float y, float z) {
   if(m_Source)
      alSource3f(m_Source, AL_POSITION, x, y, z);
}

void Source::SetDirection(float x, float y, float z) {
   if(m_Source)
      alSource3f(m_Source, AL_DIRECTION, x, y, z);
}

void Source::SetVelocity(float x, float y, float z) {
   if(m_Source)
      alSource3f(m_Source, AL_VELOCITY, x, y, z);
}
