#ifndef _GAHOOD_BOY_TIMER_HPP_
#define _GAHOOD_BOY_TIMER_HPP_

#include "util.hpp"

class Timer
{
public:
    Timer();

    void reset();
    milliseconds getElapsedMilliseconds();
    milliseconds getElapsedMillisecondsAndReset();
    microseconds getElapsedMicroseconds();
    microseconds getElapsedMicrosecondsAndReset();

private:
    milliseconds milliStart, milliEnd;
    microseconds microStart, microEnd;
};

#endif