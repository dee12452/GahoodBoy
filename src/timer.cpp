#include "timer.hpp"
#include <SDL2/SDL_timer.h>

Timer::Timer()
{
	target = 0;
	reset();
}

Timer::Timer(unsigned int targetMs)
{
	target = targetMs;
	reset();
}

void Timer::reset()
{
	start = SDL_GetTicks();
	end = start;
}

bool Timer::check()
{
	if (getElapsedMs() >= target)
	{
		return true;
	}
	return false;
}

bool Timer::checkAndReset()
{
	const bool checked = check();
	if (checked)
	{
		reset();
	}
	return checked;
}

unsigned int Timer::getElapsedMs()
{
	end = SDL_GetTicks();
	if (start > end) // Catch cases where get ticks wraps
	{
		reset();
	}

	return end - start;
}