#ifndef _GAHOOD_BOY_TIMER_HPP_
#define _GAHOOD_BOY_TIMER_HPP_

#include "util.hpp"

#include <time.h>

class Timer
{
public:
    Timer();

    void reset();
    milliseconds getElapsedMilliseconds();
    milliseconds getElapsedMillisecondsAndReset();
    microseconds getElapsedMicroseconds();
    microseconds getElapsedMicrosecondsAndReset();
	clock_t getElapsedClocks();
	clock_t getElapsedClocksAndReset();

private:
    milliseconds milliStart, milliEnd;
    microseconds microStart, microEnd;
	clock_t clockStart, clockEnd;
};

#endif