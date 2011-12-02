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

#include <cstdlib>
#include <AL/al.h>
#include <AL/alc.h>

#include "sound.hpp"

using namespace UFUG::Audio;
Sound::~Sound(void) {
}

void Sound::Initialize()
{
   m_Device = alcOpenDevice(NULL);
   if(m_Device == NULL)
      // Ignora silenciosamente, nenhum som será tocado.
      // TODO: Em modo DEBUG, logar o erro.
      return;

   // cria contexto
   m_Context = alcCreateContext(m_Device, NULL);
   // ativa contexto
   alcMakeContextCurrent(m_Context);
   // limpa os eventuais códigos de erro

   const ALfloat listenerPos[]={0.0, 0.0, 0.0};
   const ALfloat listenerVel[]={0.0, 0.0, 0.0};
   const ALfloat listenerOri[]={0.0, 0.0, 1.0, 0.0, 1.0, 0.0};

   alListenerfv(AL_POSITION, listenerPos);
   alListenerfv(AL_VELOCITY, listenerVel);
   alListenerfv(AL_ORIENTATION, listenerOri);
}

void Sound::Shutdown()
{
   alcDestroyContext(m_Context);

   if(m_Device)
      alcCloseDevice(m_Device);
}

ALCdevice* Sound::m_Device = NULL;

ALCcontext* Sound::m_Context = NULL;
