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

#ifndef SOUND_H
#define SOUND_H

#include <AL/alc.h>

#include "Source.hpp"

namespace UFUG {
   /** Relacionado ao Audio.
    *
    * Tudo que esta relacionado ao audio esta dentro deste namespace.
    */
   namespace Audio {
      /** Algum som que pode ser tocado.
       *
       * Esta é a interface que generaliza um som. A implementação deve lidar
       * com as especificidades de cada formato de áudio.
       */
      class Sound {
      public:
         /** Destrutor abstrato. */
         virtual ~Sound(void) = 0;

         /** Inicia o subsistema de som. */
         static void Initialize();

         /** Desliga o subsistema de som. */
         static void Shutdown();

      protected:
	 friend class Source;

	 /** Toca o som em uma determinada fonte OpenAL.
	  * Método chamado pelo método Play de Source.
	  */
         virtual void SelfPlay(ALuint src, bool loop) = 0;

         /** Dispositivo de som. */
         static ALCdevice* m_Device;

         /** Contexto de som. */
         static ALCcontext* m_Context;
      };

   }
}
#endif
