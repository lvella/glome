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
#include <cstdio>
#include <cerrno>
#include <list>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <AL/al.h>
#include <AL/alc.h>
#include <utility>

#include "config.hpp"
#include "source.hpp"
#include "effect.hpp"

using namespace UFUG::Audio;
using namespace std;
using namespace UFUG;

static map<string, Effect*> effects;

Effect::Effect():
	m_Buffer(0),
	ref_count(1)
{
}

Effect::Effect(const string& rFilename, iter iterator): 
	Audio::Sound(), 
	m_Buffer(0), 
	ref_count(1),
	it(iterator)
{
   OggVorbis_File vf;

   int err_code;
	stringstream dir;
	dir << DATA_DIR << "/" << rFilename << ".ogg";
	if(!(err_code = ov_fopen((char*)dir.str().c_str(), &vf))) 
	{
      LoadAndClear(vf);
	}
   else {
      // TODO: Find a way to log this error. For now, just stay silent.
      /*const char* what;
      switch(err_code)
	 {
	 case OV_EREAD:
	    what = "Error on reading file.";
	    break;
	 case OV_ENOTVORBIS:
	    what = "Bitstream does not contain any Vorbis data.";
	    break;
	 case OV_EVERSION:
	    what = "Vorbis version mismatch.";
	    break;
	 case OV_EBADHEADER:
	    what = "Invalid Vorbis bitstream header.";
	    break;
	 default:
	    what = "Bug or heap/stack corruption on Ogg Vorbis.";
	    break;
	 }

      throw string("Could not open sound effect file \"") + rFilename + "\".\n"
      + what;*/
   }
}

Effect::~Effect() {
   alDeleteBuffers(1, &m_Buffer);
}

void Effect::LoadAndClear(OggVorbis_File &vf)
{
   // Gera o buffer de áudio
   alGenBuffers(1, &m_Buffer);
   if(alGetError() == AL_NO_ERROR) {
      vorbis_info* info = ov_info(&vf, -1);
      ogg_int64_t size = ov_pcm_total(&vf, -1) * 2 * info->channels;
      ogg_int64_t count = 0;
      char* buffer = new char[size];
      int position;

      while(count < size)
	 count += ov_read(&vf, buffer + count, size - count, 0, 2, 1, &position);

      alBufferData(m_Buffer,
		   (info->channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
		   buffer, size, info->rate);

      delete buffer;
   }
   else {
      // TODO: Log the error...
   }

   ov_clear(&vf);
}

Effect*
Effect::getEffect(const string& soundid)
{
	pair<iter,bool> p = effects.insert(make_pair(soundid, (Effect*) NULL));
	if (p.second)
		p.first->second = new Effect(soundid, p.first);
	++p.first->second->ref_count;
	return p.first->second;
}

void
Effect::releaseEffect(Effect* soundEffect)
{
	--soundEffect->ref_count;
	if (!soundEffect->ref_count)
	{
		effects.erase(soundEffect->it);
		delete(soundEffect);
	}
}

void Effect::SelfPlay(ALuint src, bool loop) {
   if(m_Buffer)
   {
      alSourcei(src, AL_BUFFER, m_Buffer);
      alSourcei(src, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
      alSourcePlay(src);
   }
}


