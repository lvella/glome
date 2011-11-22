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

#ifndef SOURCE_H
#define SOURCE_H

#include <AL/al.h>

#include "Sound.hpp"

namespace UFUG {
   namespace Audio {
      class Sound;

      /** Origem do som no espaço 3D. */
      class Source {

      public:
         /** Construtor.
          * Recebe como parâmetro a posição inicial da fonte de som.
          */
         Source(float x = 0.0f, float y = 0.0f, float z = 0.0f);
         ~Source();

         /** Um determinado som nessa fonte de som. */
         void Play(Sound *pSound, bool loop = false);

         /** Define se a fonte de som é relativa.
          * Se a fonte de som for relativa, sua posição, direção e velocidade
          * serão relativas à posição do "listener".
          */
         //@{
         void SetRelative(bool relative);
         void SetPosition(float x, float y, float z);
         void SetDirection(float x, float y, float z);
         void SetVelocity(float x, float y, float z);
         //@}

      private:
         ALuint m_Source;

         /** Construtor de cópia. */
         Source(const Source *pSrc);

         /** Operador de atribuição. */
         Source& operator=(Source& rFrom);
      };
   }

}

#endif
