#ifndef __iSFX__Error_hpp__
#define __iSFX__Error_hpp__
#include "fmod_errors.h"

namespace iSFX {

struct Error {};

inline void checked(FMOD_RESULT result) {
  if (result != FMOD_OK) {
    printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
    throw Error();
  }
}

}

#endif //__iSFX__Error_hpp__