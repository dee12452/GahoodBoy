#include "timer.hpp"

Timer::Timer()
{
    reset();
}

void Timer::reset()
{
    milliS = 0;
    microS = 0;
}

milliseconds Timer::getElapsedMilliseconds() const
{
    return Gahood::getCurrentMilliseconds() - milliS;
}

milliseconds Timer::getElapsedMillisecondsAndReset()
{
    const milliseconds elapsed = getElapsedMilliseconds();
    reset();
    return elapsed;
}

microseconds Timer::getElapsedMicroseconds() const
{
    return Gahood::getCurrentMicroseconds() - microS;
}

microseconds Timer::getElapsedMicrosecondsAndReset()
{
    const microseconds elapsed = getElapsedMicroseconds();
    reset();
    return elapsed;
}