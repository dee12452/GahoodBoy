#include "io.hpp"

bool IO::update(Memory &memory)
{
	while (SDL_PollEvent(&currentEvent))
	{
		if (currentEvent.type == SDL_QUIT)
		{
			return false;
		}
	}
	return true;
}