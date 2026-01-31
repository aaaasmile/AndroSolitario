#ifndef _CURRTIME_H
#define _CURRTIME_H

#include <cstdint>

#include "Config.h"

class CurrentTime {
   public:
    CurrentTime() { Reset(); }
    STRING ToString();
    STRING ToStringMinSec();
    bool IsMoreThenOneSecElapsed();
    void Reset();
    int GetNumOfSeconds() { return _hour * 3600 + _min * 60 + _sec; };
    uint64_t GetDeltaFromLastUpdate() { return _deltaSec; }
    void StopTimer() { _updateStopped = true; }

   private:
    int _sec;
    int _min;
    int _hour;
    uint64_t _deltaSec;
    uint64_t _startSecondTick;
    bool _updateStopped;
};

#endif