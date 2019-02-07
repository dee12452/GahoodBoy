#ifndef _GAHOOD_BOY_TIMER_HPP_
#define _GAHOOD_BOY_TIMER_HPP_

#include "util.hpp"

class Timer
{
public:
    Timer();

    void reset();
    milliseconds getElapsedMilliseconds() const;
    milliseconds getElapsedMillisecondsAndReset();
    microseconds getElapsedMicroseconds() const;
    microseconds getElapsedMicrosecondsAndReset();

private:
    milliseconds milliS;
    microseconds microS;
};

#endif