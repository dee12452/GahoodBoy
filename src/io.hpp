#ifndef _GAHOOD_BOY_IO_HPP_
#define _GAHOOD_BOY_IO_HPP_

#include "memory.hpp"
#include "timer.hpp"

class IO
{
public:
	IO();
	bool update(Memory &memory);

private:
	SDL_Event currentEvent;
	Timer updateTimer;

	void updateTimers(Memory &memory);
};

#endif