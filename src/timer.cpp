#include "timer.hpp"

Timer::Timer()
{
    reset();
}

void Timer::reset()
{
    milliStart = Gahood::getCurrentMilliseconds();
    milliEnd = milliStart;
    microStart = Gahood::getCurrentMicroseconds();
    microEnd = microStart;
}

milliseconds Timer::getElapsedMilliseconds()
{
    milliEnd = Gahood::getCurrentMilliseconds();
    if(milliEnd < milliStart) // In case of overflow
    {
        reset();
    }
    return milliEnd - milliStart;
}

milliseconds Timer::getElapsedMillisecondsAndReset()
{
    const milliseconds elapsed = getElapsedMilliseconds();
    reset();
    return elapsed;
}

microseconds Timer::getElapsedMicroseconds()
{
    microEnd = Gahood::getCurrentMicroseconds();
    if(microEnd < microStart) // In case of overflow
    {
        reset();
    }
    return microEnd - microStart;
}

microseconds Timer::getElapsedMicrosecondsAndReset()
{
    const microseconds elapsed = getElapsedMicroseconds();
    reset();
    return elapsed;
}