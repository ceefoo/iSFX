#ifndef _iSFX_Sound_Hpp_
#define _iSFX_Sound_Hpp_

#include <iostream>
#include <cmath>
#include <boost/signals2.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "fmod.hpp"
#include "fmod_errors.h"

#include "Error.hpp"
#include "System.hpp"

namespace iSFX {
  
struct Sound {
  System &system;
  FMOD::Sound *sound;
  FMOD::Channel *channel;
  std::string url;
  bool paused;
  bool playing;
  FMOD_RESULT result;
  
  boost::signals2::scoped_connection update_connection;
  
  boost::signals2::signal<void(long)> song_length_change;
  boost::signals2::signal<void(long)> song_position_update;
  boost::signals2::signal<void()> song_stopped_now;
  boost::signals2::signal<void()> song_playing_now;
  
  unsigned int length_ms;
  unsigned int length_pcm;
  unsigned int length_pcmbytes;
  long position;
  float volume;
  
  bool fade_stop;
  bool fade_pause;
  float fade_amount;
  unsigned int fade_duration; //ms
  std::string name;
  
  FMOD_SOUND_TYPE type;
  FMOD_SOUND_FORMAT format;
  int channels;
  int bits;
  int numtags;
  std::map<std::string, std::vector<FMOD_TAG> > tags;
  
  
  Sound(System* sys, std::string u) 
    : system((System&)*sys),  // WARNING THIS IS BAD
      update_connection(sys->Connect(boost::bind(&Sound::Update, this, _1)))
  {
    playing = false;
    position = 0;
    url = u;
    checked(system->createSound(url.c_str(), FMOD_HARDWARE | FMOD_ACCURATETIME, 0, &sound), __LINE__);
    checked(sound->setMode(FMOD_LOOP_OFF), __LINE__);
    checked(sound->getLength(&length_ms, FMOD_TIMEUNIT_MS), __LINE__);
    checked(sound->getLength(&length_pcm, FMOD_TIMEUNIT_PCM), __LINE__);
    checked(sound->getLength(&length_pcmbytes, FMOD_TIMEUNIT_PCMBYTES), __LINE__);
    checked(sound->getFormat(&type, &format, &channels, &bits), __LINE__);
    checked(sound->getNumTags(&numtags, NULL), __LINE__);
    
    
    for(int i = 0; i < numtags; i++) {
      FMOD_TAG t;
      checked(sound->getTag(NULL, i, &t), __LINE__);
      tags[t.name].push_back(t);
      //std::cout << t.name << ": " << std::string((char*)t.data, t.datalen) << std::endl;
    }
    
    paused = false;
    fade_stop = false;
    fade_pause = false;
    
    const int len = 256;
    char buf[len] = {0};
    checked(sound->getName(buf, len), __LINE__);
    name = std::string(buf); // stops at null
  }
  
  std::string GetTag(std::string name, int i=0) {
    std::vector<FMOD_TAG> &matches = tags[name];
    if (matches.empty())
      return "";
    return std::string((char*)matches[i].data, matches[i].datalen);
  }
  
  std::string GetName() {
    return name;
  }
  
  unsigned int GetLengthMs() {
    return length_ms;
  }
  
  void SetPositionMs(long ms) {
    if (ms < 0) ms = 0;
    if (ms > length_ms-1) ms = length_ms-1;
    lose_check(channel->setPosition((unsigned int)ms, FMOD_TIMEUNIT_MS), __LINE__);
  }
  
  unsigned int GetLengthPCM() {
    return length_pcm;
  }
  
  unsigned int GetLengthPCMBytes() {
    return length_pcmbytes;
  }
  
  unsigned int MemoryUsed() {
    unsigned int bits;
    checked(sound->getMemoryInfo(FMOD_MEMBITS_ALL, FMOD_EVENT_MEMBITS_ALL, &bits, 0), __LINE__);
    return bits/8 + (bits%8 != 0);
  }
  
  void TogglePaused() {
    lose_check(channel->setPaused(!paused), __LINE__);
    paused = !paused;
  }
  
  void Unpause() {
    lose_check(channel->setPaused(false), __LINE__);
    paused = false;
  }
  
  void SetPause(bool p) {
    lose_check(channel->setPaused(p), __LINE__);
    paused = p;
  }
  
  void Play() {
    playing = true;
    if (paused) {
      Unpause();
    } else {
      checked(system->playSound(FMOD_CHANNEL_REUSE, sound, 0, &channel), __LINE__);
    }
  }
  
  void Pause() {
    SetPause(true);
    lose_check(channel->setPaused(true), __LINE__);
  }
  
  void Stop() {
    playing = false;
    paused = false;
    lose_check(channel->stop(), __LINE__);
    channel = NULL; // trying to fix a crash
  }
  
  void Fade(double dv, unsigned int dt, unsigned int method, unsigned int then) {
    fade_amount = dv;
    fade_duration = dt;
    if (then == 1) {
      fade_stop = true;
      fade_pause = false;
    } else if (then == 2) {
      fade_stop = false;
      fade_pause = true;
    } else {
      fade_stop = false;
      fade_pause = false;
    }
  }
  
  void GetPCMData() {
    unsigned int  offset;
    unsigned int  length;
    void *  ptr1;
    void *  ptr2;
    unsigned int  len1;
    unsigned int  len2;
  
    sound->lock(0, length_pcmbytes, &ptr1, &ptr2, &len1, &len2);
    std::cout << len1 << " " << len2 << "fmt:" << bits << std::endl;
  }
  
  void Update(unsigned int microsec) {
    // TODO: Define behavior when position is changed or song is paused during a fade.
    
    unsigned int lenms, ms;
    static unsigned int dt;
    bool isplaying;
    
    if (channel != NULL) {
      // Get the Channel/Songs Current State
      lose_check(channel->getVolume(&volume), __LINE__);
      lose_check(channel->getPosition(&ms, FMOD_TIMEUNIT_MS), __LINE__);
      lose_check(channel->isPlaying(&isplaying), __LINE__);
    
    // if they are the same, then hopefully it is paused and there is no volume adjustment
    if (ms != position) {
      dt = round(microsec/1000.0);
      
      if (fade_amount != 0 && fade_duration != 0) {
        if ((fade_amount < 0 && volume <= 0.0) || (fade_amount > 0 && volume >= 1.0)) {
          fade_amount = 0; fade_duration = 0;
        } else {
          lose_check(channel->setVolume(volume+(float)(((double)fade_amount)/fade_duration*dt)), __LINE__);
          if (fade_duration < dt) {
            fade_duration = 0;
          } else {
            fade_duration -= dt;
          }
        }
      }
      position = ms;
      song_position_update(position);
    }
    
  }
    
    if (isplaying != playing && isplaying) {
      playing = true;
      song_playing_now();
    }
    if (isplaying != playing && !isplaying) {
      playing = false;
      song_stopped_now();
    }
  }
  
  ~Sound() {
    if (playing)
      Stop();
    checked(sound->release(), __LINE__);
    std::cout << "Deleting sound \"" << url << "\"..." << std::endl;
  }
  
};

}

#endif