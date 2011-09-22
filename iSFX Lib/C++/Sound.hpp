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
//#include "Actions.hpp"

namespace iSFX {

////////////////////////////////////////////////////////////////////////////////
///
///   Sound Declaration
///
////////////////////////////////////////////////////////////////////////////////
 
struct Sound {
  System &system;
  FMOD::Sound *sound;
  FMOD::Channel *channel;
  //Scheduler scheduler;

  std::string url;
  std::string name;

  bool fade_stop;
  bool fade_pause;
  float fade_amount;
  unsigned int fade_duration; //ms


  bool paused;
  bool playing;
  
  unsigned int open_state;
  bool opening;
  unsigned int buffered;
  bool starving;
  bool disk_busy;
  
  unsigned int length_ms;
  unsigned int length_pcm;
  unsigned int length_pcmbytes;
  
  double percent;
  unsigned int position;
  unsigned int start;
  unsigned int stop;
  float volume;
  struct {
    float volume;
    float frequency;
    float pan;
    int priority;
  } defaults;  
  
  float fadein;
  unsigned int fadein_length;
  
  float fadeout;
  unsigned int fadeout_length;
  
  
  FMOD_SOUND_TYPE type;
  FMOD_SOUND_FORMAT format;
  int channels;
  int bits;
  int numtags;
  std::map<std::string, std::vector<FMOD_TAG> > tags;

  boost::signals2::scoped_connection system_update_connection;
  boost::signals2::signal<void( unsigned int )> now_at_position;
  boost::signals2::signal<void( double       )> now_at_percent;
  boost::signals2::signal<void(              )> now_playing;
  boost::signals2::signal<void(              )> now_paused;
  boost::signals2::signal<void(              )> now_unpaused;
  boost::signals2::signal<void(              )> now_stopped;
  boost::signals2::signal<void(              )> now_finished;
  
  boost::signals2::signal<void( unsigned int )> now_in_state;
  boost::signals2::signal<void( bool         )> now_starving;
  
  boost::signals2::signal<void( std::string  )> new_name;
  boost::signals2::signal<void( unsigned int )> new_length_ms;
  
  Sound(System* sys, std::string u);
  void Reset();
  void Update();
  ~Sound();
  
  void GetStaticInfo();

  /////////////////////////////////////////////////////////////////////////////
  ///                                                                          
  ///   Getters and Setters                                                   
  ///                                                                          
  ///

  std::string GetUrl() { return url; }
  std::string GetName() {
    if (opening) return url;
    return name;
  }

  void SetStartPosition(unsigned int ms) { start = ms; }
  void SetStopPosition(unsigned int ms) { stop = ms; }

  std::string GetTag(std::string name, int i=0);

  unsigned int GetLengthMs() { return length_ms; }
  unsigned int GetLengthPCM() { return length_pcm; }
  unsigned int GetLengthPCMBytes() { return length_pcmbytes; }
  
  void GetPCMData();
  unsigned int MemoryUsed();

  void SetPositionMs(long ms);
  //void Skip(unsigned int to);
  void SetDefaults(float v = -2, float pa = -2, float f = -2, int pr = -2);
  
  void Play(bool pause=0);
  void Pause();
  void Unpause();
  void Stop();
  void PlayStop();
  void PlayPause();
  
protected:
  void _Play(bool);
  void _Pause();
  void _Unpause();
  void _Stop();

public:
  //void ScheduleEffect(unsigned int t, fun_type f);
  //
  ////template<typename _Callable>
  ////void CallAt( _Callable& callable, unsigned int at);
  //
  //void ScheduleFade(double dv, unsigned int start, unsigned int len, std::string method);
  void SetFadeOnEnter(double max, unsigned int len, std::string method);
  void SetFadeOnExit(unsigned int len, std::string method);
  void SetFadeOnStop(unsigned int len, std::string method);
  void SetFadeOnPause(unsigned int len, std::string method);
  void Fade(double dv, unsigned int len, std::string method);


  void SetLoop(unsigned int begin, unsigned int end, int count = 1);

  void UnsetLoop();
  
  //void _LinearFadeEffect(const boost::signals2::connection &conn, const unsigned int start, const unsigned int t, const unsigned int stop, double dv);
  //
  //template<typename _Callable>
  //void _CallFunction(const boost::signals2::connection &conn, _Callable& callable, const unsigned int t, const unsigned int at);
  //
  //template<typename _Callable>
  //void _CallFunction(const boost::signals2::connection &conn, _Callable& callable, const unsigned int t);
};


////////////////////////////////////////////////////////////////////////////////
///
///   Sound Implementation
///
////////////////////////////////////////////////////////////////////////////////
  
Sound::Sound(System* sys, std::string u) 
  : system((System&)*sys)  // WARNING THIS IS BAD
{
  url = u;
  checked(system->createSound(url.c_str(), FMOD_HARDWARE | FMOD_ACCURATETIME | FMOD_NONBLOCKING, 0, &sound), __LINE__);
  channel = NULL;
  opening = true;
  open_state = FMOD_OPENSTATE_MAX;
  buffered = 0;
  length_ms = 0;
  length_pcm = 0;
  length_pcmbytes = 0;
  starving = false;
  disk_busy = 0;
  
  start = 0;
  
  fadein_length = 1000;
  fadeout_length = 1000;
  
  fade_stop = false;
  fade_pause = false;
  
  Reset();
  system_update_connection = system.update.connect(boost::bind(&Sound::Update, this));
}

void Sound::GetStaticInfo() {
  checked(sound->setMode(FMOD_LOOP_OFF), __LINE__);
  checked(sound->getLength(&length_ms, FMOD_TIMEUNIT_MS), __LINE__);
  checked(sound->getLength(&length_pcm, FMOD_TIMEUNIT_PCM), __LINE__);
  checked(sound->getLength(&length_pcmbytes, FMOD_TIMEUNIT_PCMBYTES), __LINE__);
  checked(sound->getFormat(&type, &format, &channels, &bits), __LINE__);
  checked(sound->getNumTags(&numtags, NULL), __LINE__);
  checked(sound->getDefaults(&defaults.frequency, &defaults.volume, &defaults.pan, &defaults.priority), __LINE__);
  
  checked(sound->setDefaults(defaults.frequency, 0.0, defaults.pan, defaults.priority), __LINE__);
  stop = length_ms;
  
  for(int i = 0; i < numtags; i++) {
    FMOD_TAG t;
    checked(sound->getTag(NULL, i, &t), __LINE__);
    tags[t.name].push_back(t);
    //std::cout << t.name << ": " << std::string((char*)t.data, t.datalen) << std::endl;
  }
  
  const int len = 256;
  char buf[len] = {0};
  checked(sound->getName(buf, len), __LINE__);
  name = std::string(buf); // stops at null
  
  new_name(name);
  new_length_ms(length_ms);
}
  
void Sound::Reset() {
  playing = false;
  paused = false;
  position = 0;
  percent = 0;
}

Sound::~Sound() {
  if (playing) Stop();
  checked(sound->release(), __LINE__);
  std::cout << "Deleting sound \"" << url << "\"..." << std::endl;
}

/////////////////////////////////////////////////////////////////////////////
///                                                                          
///   Protected                                                   
///                                                                          
///

void Sound::_Play(bool pause) {
  checked(system->playSound(FMOD_CHANNEL_FREE, sound, pause, &channel), __LINE__);
  if (!system_update_connection.connected())
    system_update_connection = system.update.connect(boost::bind(&Sound::Update, this));
  //playing = true;
  //paused = pause;
  if (start != 0) SetPositionMs(start);
  now_playing();
}
void Sound::_Pause() {
  //paused = true;
  lazy_check(channel->setPaused(true), __LINE__);
  now_paused();
}
void Sound::_Unpause() {
  lazy_check(channel->setPaused(false), __LINE__);
  //paused = false;
  now_unpaused();
}
void Sound::_Stop() {
  //paused = false;
  //position = 0;
  lazy_check(channel->stop(), __LINE__);
  channel = NULL;
  now_stopped();
  // Possibly \/
  Reset();
}

/////////////////////////////////////////////////////////////////////////////
///                                                                          
///   Multiline Getters and Setters                                                   
///                                                                          
///
std::string Sound::GetTag(std::string name, int i) {
  std::vector<FMOD_TAG> &matches = tags[name];
  if (matches.empty())
    return "";
  return std::string((char*)matches[i].data, matches[i].datalen);
}

void Sound::SetPositionMs(long ms) {
  if (ms < 0) ms = 0;
  if (ms > length_ms-1) ms = length_ms-1;
  lazy_check(channel->setPosition((unsigned int)ms, FMOD_TIMEUNIT_MS), __LINE__);
}

//void Sound::Skip(unsigned int to) {
//  //if (to < 0) to = 0;
//  //else if (to > length_ms-1) to = length_ms-1;
//  //if (to == position) return;
//  //int i = position;
//  //if (to < position) { // reverse
//  //  Reset();
//  //  SetPositionMs(0);
//  //  i = 0;
//  //}
//  //for(;i < to; i++) {
//  //  Update(i);
//  //}
//}

unsigned int Sound::MemoryUsed() {
  unsigned int bits;
  checked(sound->getMemoryInfo(FMOD_MEMBITS_ALL, FMOD_EVENT_MEMBITS_ALL, &bits, 0), __LINE__);
  return bits/8 + (bits%8 != 0);
}

void Sound::PlayStop() {
  if (playing) {
    _Stop();
  } else {
    if (paused) _Unpause();
    else _Play(false);
  }
}

void Sound::PlayPause() {
  if (playing && paused) Unpause();
  else if (!playing) _Play(false);
  else _Pause();
}

void Sound::Play(bool pause) {
  if (!playing) {
    _Play(pause);
  }
}


void Sound::Pause() { if (!paused) { _Pause(); } }


void Sound::Unpause() {
  if (paused) {
    _Unpause();
  }
}


void Sound::Stop() {
  if (playing) {
    _Stop();
  }
}

void Sound::SetDefaults(float v, float pa, float f, int pr) {
  if (f > 0) defaults.frequency = f;
  if (v >= 0.0 && v <= 1.0) defaults.volume = v;
  if (pa >= -1.0 && pa <= 1.0) defaults.pan = pa;
  if (pr >= 0 && pr <= 255 ) defaults.priority = pr; // TODO fix arbitrary negative limit
  sound->setDefaults(defaults.frequency, defaults.volume, defaults.pan, defaults.priority);
}

void Sound::GetPCMData() {
  unsigned int  offset;
  unsigned int  length;
  void *  ptr1;
  void *  ptr2;
  unsigned int  len1;
  unsigned int  len2;

  sound->lock(0, length_pcmbytes, &ptr1, &ptr2, &len1, &len2);
  std::cout << len1 << " " << len2 << "fmt:" << bits << std::endl;
}

void Sound::SetLoop(unsigned int begin, unsigned int end, int count) {
  checked(sound->setLoopPoints(begin, FMOD_TIMEUNIT_MS, end, FMOD_TIMEUNIT_MS), __LINE__);
  checked(sound->setLoopCount(count), __LINE__);
}

void Sound::UnsetLoop() {
  checked(sound->setLoopCount(0), __LINE__);
}

void Sound::Update() {
  // TODO: Define behavior when position is changed or song is paused during a fade.
  unsigned int pos = position;
  float vol = volume;
  bool isplaying = playing;
  bool ispaused = paused;
  
  bool starv = starving;
  unsigned int buffer = buffered;
  FMOD_OPENSTATE state = (FMOD_OPENSTATE)open_state;
  bool disk = disk_busy;
  
  checked(sound->getOpenState(&state, &buffer, &starv, &disk), __LINE__);
  if (state != open_state) {
    open_state = state;
    
    if (state == FMOD_OPENSTATE_READY && opening) {
      opening = false;
      GetStaticInfo();
    }
    
    now_in_state(open_state);
  }
  if (starv != starving) {
    starving = starv;
    now_starving(starving);
  }
  
  
  if (channel != NULL) {
    // Get the Channel/Songs Current State
    lazy_check(channel->getVolume(&vol), __LINE__);
    lazy_check(channel->getPosition(&pos, FMOD_TIMEUNIT_MS), __LINE__);
    lazy_check(channel->isPlaying(&isplaying), __LINE__);
    lazy_check(channel->getPaused(&ispaused), __LINE__);
    // TODO: undefined behavior ^ when channel becomes NULL in the middle of them?
    
    if (vol != volume) volume = vol;
    // if they are the same, then hopefully it is paused and there is no volume adjustment
    
    unsigned int ellapsed = pos - position;
    if (pos != position) {
      // check for new effects to add
      position = pos;
      now_at_position(position);
      
      percent = 100.0*position/length_ms;
      if (percent == percent) {
        now_at_percent(percent);
      }
    }
    if (!isplaying && playing) {
      playing = isplaying; // redundant
      _Stop(); // redundant
      now_stopped();
    } else if (isplaying && !playing) {  
      playing = isplaying;
      now_playing();
    }
    
    if (!ispaused && paused) {
    paused = ispaused;
      now_unpaused();
    } else if (ispaused && !paused) {    
      paused = ispaused;
      now_paused();
    }
    if (position >= stop) { 
      Stop();
      now_finished();
    } else if (playing && position <= fadein_length+start) {
      lazy_check(channel->setVolume(volume+(1.0/fadein_length*ellapsed)), __LINE__);
      // TODO: fix the fades so they reach the target volume.
      //std::cout << "FadeIn: " << volume+(1.0/fadein_length) << std::endl;
    } else if (playing && position >= stop-fadeout_length) {
      lazy_check(channel->setVolume(volume-(1.0/fadeout_length*ellapsed)), __LINE__);
      //std::cout << "FadeOut: " << volume-(1.0/fadeout_length) << std::endl;
    }
  } else if (!opening) {
    paused = 0;
    playing = 0;
    position = 0;
    percent = 0;
    system_update_connection.disconnect();
    return;
  }
}

}

#endif