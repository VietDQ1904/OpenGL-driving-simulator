#include <AL/al.h>
#include <AL/alc.h>
#include <fstream>
#include <iostream>
#include <cstring>
#include <map>

#ifndef SOUNDTRACK_CLASS
#define SOUNDTRACK_CLASS

class Sound
{

public:
   Sound();
   ~Sound();
   void loadAudio(std::string fileName, std::string soundTrackName);
   void playAudio(const std::string &soundTrackName, bool loop);
   void stopAudio(const std::string &soundTrackName);
   void pauseAudio(const std::string &soundTrackName);
   void setVolume(float volume);

private:
   ALCdevice *device;
   ALCcontext *context;
   std::map<std::string, ALuint> soundTrackSources;
   std::map<std::string, ALuint> soundTrackBuffers;

   bool soundTrackExists(const std::string &soundTrackName) const
   {
      return (soundTrackSources.find(soundTrackName) != soundTrackSources.end());
   }

   static bool isBigEndian(void)
   {
      int a = 1;
      return !((char *)&a)[0];
   }

   static int convertToInt(char *buffer, int length)
   {
      int a = 0;
      if (!isBigEndian())
      {
         for (int i = 0; i < length; ++i)
         {
            ((char *)&a)[i] = buffer[i];
         }
      }
      else
      {
         for (int i = 0; i < length; ++i)
         {
            ((char *)&a)[3 - i] = buffer[i];
         }
      }

      return a;
   }

   static char *loadWAVSource(std::string fileName, int &channels, int &sampleRate,
                              int &bps, int &size)
   {
      char buffer[4];
      std::ifstream input(fileName.c_str(), std::ios::binary);

      if (!input.is_open())
      {
         std::cout << "OPENAL::CANNOT_LOAD_AUDIO::FILE_NOT_FOUND..." << std::endl;
      }

      input.read(buffer, 4); // Characters RIFF

      if (std::strncmp(buffer, "RIFF", 4) != 0)
      {
         std::cout << "OPENAL::CANNOT_LOAD_AUDIO::FILE_DOES NOT CONTAIN RIFF..." << std::endl;
      }

      input.read(buffer, 4); // Size of the file, unused.
      input.read(buffer, 4); // Characters WAVE

      if (std::strncmp(buffer, "WAVE", 4) != 0)
      {
         std::cout << "OPENAL::CANNOT_LOAD_AUDIO::FILE_DOES_NOT_CONTAIN_WAVE..." << std::endl;
      }

      input.read(buffer, 4); // Format Maker

      if (std::strncmp(buffer, "fmt ", 4) != 0)
      {
         std::cout << "OPENAL::CANNOT_LOAD_AUDIO::FILE_DOES_NOT CONTAIN FORMAT MAKER..." << std::endl;
      }

      input.read(buffer, 4); // Length of format data, should be 16 for PCM.

      if (convertToInt(buffer, 4) != 16)
      {
         std::cout << "OPENAL::CANNOT_LOAD_AUDIO::INCORRECT_FORMAT_LENGTH..." << std::endl;
      }

      input.read(buffer, 2); // Type of format, PCM = 1.

      if (convertToInt(buffer, 2) != 1)
      {
         std::cout << "OPENAL::CANNOT_LOAD_AUDIO::FILE_NOT_IN_PCM_FORMAT..." << std::endl;
      }

      input.read(buffer, 2); // Get number of channels

      channels = convertToInt(buffer, 2);

      input.read(buffer, 4); // Get sample rate.

      sampleRate = convertToInt(buffer, 4);

      input.read(buffer, 4); // Byte Rate
      input.read(buffer, 2); // Block align
      input.read(buffer, 2); // Get bits per sample

      bps = convertToInt(buffer, 2);

      input.read(buffer, 4);
      input.read(buffer, 4); // Size of data

      size = convertToInt(buffer, 4);

      if (size < 0)
      {
         std::cout << "OPENAL::CANNOT_LOAD_AUDIO::INVALID_FILE_SIZE..." << std::endl;
      }

      char *data = new char[size];

      input.read(data, size);
      input.close();

      return data;
   }
};

#endif
