#include <vector>
#include <sstream>
#include <map>
#include <AL/al.h>
#include <AL/alc.h>

#include "config.hpp"
#include "audio_source.hpp"
#include "audio_effect.hpp"

using namespace Audio;
using namespace std;

static map<string, Effect*> effects;

Effect::Effect():
	m_Buffer(0),
	ref_count(1)
{}

Effect::Effect(const string& rFilename, iter iterator):
	m_Buffer(0),
	ref_count(1),
	it(iterator)
{
   OggOpusFile* of;

   int err_code;
	stringstream dir;

	dir << DATA_DIR << "/sound/" << rFilename << ".opus";
   string fname = dir.str();
   of = op_open_file(fname.c_str(), &err_code);

	if(!err_code) {
      num_samples = LoadAndClear(of);
	} else {
      cerr << "Error while loading file \"" << fname << "\".\n";
   }
}

Effect::~Effect() {
   alDeleteBuffers(1, &m_Buffer);
}

size_t Effect::LoadAndClear(OggOpusFile *of)
{
   size_t filled = 0;

   // Generate audio buffer
   alGenBuffers(1, &m_Buffer);
   if(alGetError() == AL_NO_ERROR) {
      vector<opus_int16> buf(1024*1024);
      int ret;

      do {
         ret = op_read(of, &buf[filled], buf.size() - filled, nullptr);
         filled += ret;
         if(buf.size() - filled < 5760) {
            buf.resize(2 * buf.size());
         }
      } while(ret > 0);

      alBufferData(m_Buffer, AL_FORMAT_MONO16, &buf[0], filled, 48000);
   }
   else {
      // TODO: Log the error...
   }

   op_free(of);

   return filled;
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

void Effect::selfPlay(ALuint src, bool loop, float offset) {
   if(m_Buffer)
   {
      alSourcei(src, AL_BUFFER, m_Buffer);
      alSourcei(src, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
      alSourcef(src, AL_SAMPLE_OFFSET, offset * num_samples);
      alSourcePlay(src);
   }
}
