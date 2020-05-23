#pragma once

#include <map>
#include <opus/opusfile.h>
#include <iostream>
#include "audio_sound.hpp"

namespace Audio
{
	/** Sound effect.
	 *
	 * Decodes and loads the whole audio in-memory, ready to be reproduced.
	 * Takes up a lot of space, so, use only for sound effects, not music.
	 *
	 */
	class Effect: public Audio::Sound
	{
		public:
		static Effect* getEffect(const std::string& soundid);
		static void releaseEffect(Effect* soundEffect);

		/** Destrutor. */
		virtual ~Effect(void);

		protected:
		using iter = std::map<std::string, Effect*>::iterator;

		/** Builds an effect from a Ogg Opus file. */
		Effect(const std::string& rFilename, iter iterator);

		/** Constructor for no audio.	*/
		Effect(void);

		/** Loads the buffer.
		* From an open Ogg file structure, decodes the stream,
		* creates the OpenAL buffer, and frees the original
		* structure. Used on initialization of the object.
		*/
		size_t LoadAndClear(OggOpusFile *of);

		/** Plays the sound in a OpenAL source. */
		virtual void selfPlay(ALuint src, bool loop, float offset=0) override;

		private:
		ALuint m_Buffer;
		unsigned int ref_count;
		iter it;

		size_t num_samples;
	};
}
