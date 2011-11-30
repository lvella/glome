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

#ifndef EFFECT_H
#define EFFECT_H

#include <map>
#include <ogg/os_types.h>
#include <vorbis/vorbisfile.h>
#include <iostream>
#include "sound.hpp"

namespace UFUG 
{
	namespace Audio 
	{
		/** Som de efeito.
		 *
		 * Decodifica e carrega o áudio inteiro na memória, deixando-o pronto para ser reproduzido.
		 * Ocupa bastante espaço, pois mantém o áudio descompactado em memória.
		 * Use somente para efeitos sonoros.
		 *
		 * Nao deve ser copiada ou derivada. Nao tem construtor default porque cada instancia
		 * precisa de um arquivo de som para funcionar.
		 */
		class Effect: public Audio::Sound 
		{
			public:  
			static Effect* getEffect(const std::string& soundid);
			static void releaseEffect(Effect* soundEffect);

			/** Destrutor. */
			virtual ~Effect(void);

			protected:
			typedef std::map<std::string, Effect*>::iterator iter;
			/** Construtor.
			*
			* Constroi um objeto num arquivo Ogg Vorbis.
			*
			* @param rFilename Nome do arquivo.
			*/
			Effect(const std::string& rFilename, iter iterator);
			/** Constructor for no audio.
			*/
			Effect(void);

			/** Loads the buffer.
			* From an open Ogg file structure, decodes the stream,
			* creates the OpenAL buffer, and frees the original
			* structure. Used on initialization of the object.
			*/
			void LoadAndClear(OggVorbis_File &vf);

			/** Especialização para tocar som de efeito. */
			virtual void SelfPlay(ALuint src, bool loop);

			private:
			ALuint m_Buffer;
			unsigned int ref_count;
			iter it;
		};
	}
}

#endif
