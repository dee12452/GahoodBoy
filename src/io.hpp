#ifndef _GAHOOD_BOY_IO_HPP_
#define _GAHOOD_BOY_IO_HPP_

#include "memory.hpp"

class IO
{
public:
	bool update(Memory &memory);

private:
	SDL_Event currentEvent;
};

#endif