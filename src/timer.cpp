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
	clockStart = clock();
	clockEnd = clockStart;
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

clock_t Timer::getElapsedClocks()
{
	clockEnd = clock();
	if (clockEnd < clockStart)
	{
		clockStart = clock();
	}
	return clockEnd - clockStart;
}

clock_t Timer::getElapsedClocksAndReset()
{
	const clock_t elapsed = getElapsedClocks();
	reset();
	return elapsed;
}