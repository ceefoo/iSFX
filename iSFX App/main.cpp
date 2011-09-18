/*===============================================================================================
 PlaySound Example
 Copyright (c), Firelight Technologies Pty, Ltd 2004-2011.

 This example shows how to simply load and play multiple sounds.  This is about the simplest
 use of FMOD.
 This makes FMOD decode the into memory when it loads.  If the sounds are big and possibly take
 up a lot of ram, then it would be better to use the FMOD_CREATESTREAM flag so that it is 
 streamed in realtime as it plays.
===============================================================================================*/
#include "fmod.hpp"
#include "fmod_errors.h"
#include "../FMOD Programmers API/examples/common/wincompat.h"
#include <stdio.h>
#include <iostream>
#include <algorithm>


void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
}


int main(int argc, char *argv[])
{
    FMOD::System     *system;
    FMOD::Sound      *sound;
    FMOD::Channel    *channel = 0;
    FMOD::Channel    *fade_channel = 0;
    FMOD_RESULT       result;
    int               key;
    unsigned int      version;

    /*
        Global Settings
    */
    result = FMOD::System_Create(&system);
    ERRCHECK(result);

    result = system->getVersion(&version);
    ERRCHECK(result);

    if (version < FMOD_VERSION)
    {
        printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
        getch();
        return 0;
    }

    result = system->init(32, FMOD_INIT_NORMAL, NULL);
    ERRCHECK(result);

    std::cout << "Looking for file: " << argv[1] << std::endl;
    result = system->createSound(argv[1], FMOD_SOFTWARE, 0, &sound);
    ERRCHECK(result);
    
    result = sound->setMode(FMOD_LOOP_OFF);
    ERRCHECK(result);
    
    printf("\n");
    printf("Press '1' to Play\n");
    printf("Press 'p' to Pause\n");
    printf("Press 'i' to Skip Back\n");
    printf("Press 'o' to Skip Forward\n");
    printf("Press 'k' to Volume Down\n");
    printf("Press 'l' to Volume Up\n");
    printf("Press 'm' to Fade In\n");
    printf("Press 'n' to Stop Fade\n");
    printf("Press 'b' to Fade Out\n");
    printf("Press 'v' to Toggle Pause On Fade Out\n");
    printf("Press 'c' to Toggle Stop On Fade Out\n");
    printf("Press 'x' to Crossfade\n");
    printf("Press 'Esc' to quit\n");
    printf("\n");

    /*
        Main loop.
    */
          
    bool         paused = 0;
    unsigned int ms = 0;
    unsigned int lenms = 0;
    bool         playing = 0;
    int          channelsplaying = 0;
    float volume = 0;
    double fade = 0;
    bool fade_pauses = false;
    bool fade_stops = false;
    FMOD::Sound *currentsound = 0;
    bool crossfade = false;
    float crossfade_vol;
    
    do
    {
        if (kbhit())
        {
            key = getch();

            switch (key)
            {
                case '1' :
                {
                    result = system->playSound(FMOD_CHANNEL_FREE, sound, 0, &channel);
                    ERRCHECK(result);
                    break;
                }
                case 'p' :
                {
                  channel->setPaused(!paused);
                  if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
                  {
                      ERRCHECK(result);
                  }
                  break;
                }
                case 'i' :
                {
                  unsigned int newms;
                  if ((double)ms-lenms/500.0 < 0) {
                    newms = 0;
                  } else {
                    newms = (unsigned int)((double)ms-lenms/500.0);
                  }
                  result = channel->setPosition(newms, FMOD_TIMEUNIT_MS);
                  if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
                  {
                    std::cout << "while seeking to: " << newms << std::endl;
                      ERRCHECK(result);
                  }
                  break;
                }
                case 'o' :
                {
                  unsigned int newms;
                  if ((unsigned int)((double)ms+lenms/500.0) > lenms-1) {
                    newms = lenms-1;
                  } else {
                    newms = (unsigned int)((double)ms+lenms/500.0);
                  }
                  result = channel->setPosition(newms, FMOD_TIMEUNIT_MS);
                  if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
                  {
                    std::cout << "while seeking to: " << newms << " of " << lenms << std::endl;
                      ERRCHECK(result);
                  }
                  break;
                }
                case 'k' :
                {
                  result = channel->setVolume(std::max(volume-0.01,0.0));
                  if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
                  {
                      ERRCHECK(result);
                  }
                  break;
                }
                case 'l' :
                {
                  result = channel->setVolume(std::min(volume+0.01,1.0));
                  if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
                  {
                      ERRCHECK(result);
                  }
                  break;
                }
                case 'b' :
                {
                  if (fade < 0.01 && fade > -0.01) {
                    fade = -1.0;
                  }
                  break;
                }
                case 'n' :
                {
                  fade = 0;
                  break;
                }
                case 'm' :
                {
                  if (fade < 0.01 && fade > -0.01) {
                    fade = 1.0;
                  }
                  if (fade_pauses && paused) {
                    channel->setPaused(false);
                    if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
                    {
                        ERRCHECK(result);
                    }
                  }
                  break;
                }
                case 'v' :
                {
                  fade_pauses = !fade_pauses;
                  break;
                }
                case 'c' :
                {
                  fade_stops = !fade_stops;
                  break;
                }
                case 'u' :
                {
                  result = currentsound->setLoopPoints(5000, FMOD_TIMEUNIT_MS, 10000, FMOD_TIMEUNIT_MS);
                  ERRCHECK(result);
                  result = currentsound->setMode(FMOD_LOOP_OFF);
                  ERRCHECK(result);
                  break;
                }
                case 'x' :
                {
                  crossfade = true;
                  fade = -1.0;
                  result = system->playSound(FMOD_CHANNEL_FREE, sound, 0, &fade_channel);
                  ERRCHECK(result);
                  result = fade_channel->setVolume(0.0);
                  ERRCHECK(result);
                  break;
                }
            }
        }

        system->update();

        {

            if (channel)
            {

                if (fade < 0) {
                  fade += 0.01;
                  if (crossfade) {
                    result = fade_channel->getVolume(&crossfade_vol);
                    std::cout << crossfade_vol << std::endl;
                    if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN)) { ERRCHECK(result); }
                    result = fade_channel->setVolume(std::min(crossfade_vol + 0.01, 1.0));
                    if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN)) { ERRCHECK(result); }
                  }
                  result = channel->setVolume(std::max(volume-0.01,0.0));
                  if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
                  {
                      ERRCHECK(result);
                  }
                  if (fade <= 0.00 && fade > -0.01) {
                    fade = 0;
                    if (fade_pauses) {
                      result = channel->setPaused(true);
                      if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
                      {
                          ERRCHECK(result);
                      }
                    }
                    if (fade_stops) {
                      result = channel->stop();
                      if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
                      {
                          ERRCHECK(result);
                      }
                    }
                  }
                } else if (fade > 0) {
                  fade -= 0.01;
                  result = channel->setVolume(std::min(volume+0.01,1.0));
                  if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
                  {
                      ERRCHECK(result);
                  }
                }

                result = channel->isPlaying(&playing);
                if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
                {
                    ERRCHECK(result);
                }

                result = channel->getPaused(&paused);
                if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
                {
                    ERRCHECK(result);
                }
                
                result = channel->getVolume(&volume);
                if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
                {
                    ERRCHECK(result);
                }

                result = channel->getPosition(&ms, FMOD_TIMEUNIT_MS);
                if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
                {
                    ERRCHECK(result);
                }
               
                channel->getCurrentSound(&currentsound);
                if (currentsound)
                {
                    result = currentsound->getLength(&lenms, FMOD_TIMEUNIT_MS);
                    if ((result != FMOD_OK) && (result != FMOD_ERR_INVALID_HANDLE) && (result != FMOD_ERR_CHANNEL_STOLEN))
                    {
                        ERRCHECK(result);
                    }
                }
            }

            system->getChannelsPlaying(&channelsplaying);

            printf("\rTime %02d:%02d:%02d/%02d:%02d:%02d : %s : Channels Playing %2d", ms / 1000 / 60, ms / 1000 % 60, ms / 10 % 100, lenms / 1000 / 60, lenms / 1000 % 60, lenms / 10 % 100, paused ? "Paused " : playing ? "Playing" : "Stopped", channelsplaying);
            fflush(stdout);
        }

        Sleep(10);

    } while (key != 27);

    printf("\n");

    /*
        Shut down
    */
    result = sound->release();
    ERRCHECK(result);
    result = system->close();
    ERRCHECK(result);
    result = system->release();
    ERRCHECK(result);

    return 0;
}


