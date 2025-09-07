#include "soundTrack.hpp"

Sound::Sound()
{

   device = alcOpenDevice(nullptr); // Default device.

   if (!device)
   {
      std::cout << "OPENAL::CANNOT_OPEN_DEVICE..." << std::endl;
   }

   context = alcCreateContext(device, nullptr); // Create context

   if (!context)
   {
      std::cout << "OPENAL::CANNOT_CREATE_CONTEXT..." << std::endl;
   }

   alcMakeContextCurrent(context);
}

Sound::~Sound()
{
   for (auto &i : soundTrackSources)
   {
      alDeleteSources(1, &i.second);
   }

   for (auto &i : soundTrackBuffers)
   {
      alDeleteBuffers(1, &i.second);
   }

   alcMakeContextCurrent(nullptr);
   alcDestroyContext(context);
   alcCloseDevice(device);
}

void Sound::loadAudio(std::string fileName, std::string soundTrackName)
{
   int channels, bps, sampleRate, size;
   char *data;

   ALuint buffer;
   alGenBuffers(1, &buffer);

   // Load audio file.
   data = loadWAVSource(fileName, channels, sampleRate, bps, size);

   if (!data)
   {
      std::cout << "OPENAL::LOAD_AUDIO::READ_DATA_FAILURE." << std::endl;
      return;
   }

   // Pick a format based on the number of channels.
   ALenum format;
   if (channels == 1)
   {
      format = (bps == 8) ? AL_FORMAT_MONO8 : AL_FORMAT_MONO16;
   }
   else
   {
      format = (bps == 8) ? AL_FORMAT_STEREO8 : AL_FORMAT_STEREO16;
   }

   // Generate buffer
   alBufferData(buffer, format, data, size, sampleRate);

   // Generate source
   ALuint source;
   alGenSources(1, &source);
   alSourcei(source, AL_BUFFER, buffer);

   soundTrackBuffers[soundTrackName] = buffer;
   soundTrackSources[soundTrackName] = source;

   delete[] data;
}

void Sound::playAudio(const std::string &soundTrackName, bool loop)
{
   if (!soundTrackExists(soundTrackName))
   {
      std::cout << "OPENAL::INVALID_SOUNDTRACK NAME::" << soundTrackName << "..." << std::endl;
      return;
   }

   ALuint source = soundTrackSources.at(soundTrackName);

   // Set looping.
   if (loop)
   {
      alSourcei(source, AL_LOOPING, AL_TRUE);
   }

   // Play the audio
   alSourcePlay(source);
}

void Sound::stopAudio(const std::string &soundTrackName)
{
   if (!soundTrackExists(soundTrackName))
   {
      std::cout << "OPENAL::INVALID_SOUNDTRACK NAME::" << soundTrackName << "..." << std::endl;
      return;
   }

   ALuint source = soundTrackSources.at(soundTrackName);
   alSourceStop(source);
}

void Sound::pauseAudio(const std::string &soundTrackName)
{
   if (!soundTrackExists(soundTrackName))
   {
      std::cout << "OPENAL::INVALID_SOUNDTRACK NAME::" << soundTrackName << "..." << std::endl;
      return;
   }

   ALuint source = soundTrackSources.at(soundTrackName);
   alSourcePause(source);
}
