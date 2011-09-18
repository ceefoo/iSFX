#ifndef _ISFX_SYSTEM_HPP_
#define _ISFX_SYSTEM_HPP_

#include <iostream>
#include <cmath>
#include <boost/signals2.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "fmod.hpp"
#include "fmod_errors.h"

#include "Error.hpp"

namespace iSFX {

  struct System {
    FMOD::System *system;
    FMOD_RESULT result;
    
    boost::signals2::signal<void(unsigned int)> update;
    
    System() {
      checked(FMOD::System_Create(&system), __LINE__);
      checked(system->init(32, FMOD_INIT_NORMAL, NULL), __LINE__);
    }
    
    void Update() {
      static boost::posix_time::ptime last (boost::posix_time::microsec_clock::local_time());
      system->update();
      
      boost::posix_time::ptime now (boost::posix_time::microsec_clock::local_time());
      update((now-last).total_microseconds());
      last = now;
    }
    
    template<typename anything>
    boost::signals2::connection Connect(anything a) {
      return update.connect(a);
    }
    
    operator FMOD::System* () {
      return system;
    }
    
    FMOD::System* operator -> () {
      return system;
    }
  };

}

#endif