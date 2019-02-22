#include "io.hpp"

IO::IO()
{
	updateTimer = Timer(2); // TODO innacurate
}

bool IO::update(Memory &memory)
{
	updateTimers(memory);
	while (SDL_PollEvent(&currentEvent))
	{
		if (currentEvent.type == SDL_QUIT)
		{
			return false;
		}
		else if(currentEvent.type == SDL_KEYUP && currentEvent.key.keysym.scancode == SDL_SCANCODE_V) // Toggle verbose logging
		{
			Gahood::setVerboseMode(!Gahood::isVerboseMode());
		}
	}
	const unsigned char *keys = SDL_GetKeyboardState(NULL);
	const byte joypad = memory.read(0xFF00);
	const bool isButtonKeys = (joypad & 0x20) == 0x00;
	const bool isDirectionKeys = (joypad & 0x10) == 0x00;
	if(isButtonKeys)
	{
		if(keys[SDL_SCANCODE_A]) memory.write(0xFF00, joypad & 0xFE); else memory.write(0xFF00, joypad | 0x01);
		if(keys[SDL_SCANCODE_B]) memory.write(0xFF00, joypad & 0xFD); else memory.write(0xFF00, joypad | 0x02);
		if(keys[SDL_SCANCODE_LSHIFT]) memory.write(0xFF00, joypad & 0xFB); else memory.write(0xFF00, joypad | 0x04);
		if(keys[SDL_SCANCODE_RETURN]) memory.write(0xFF00, joypad & 0xF7); else memory.write(0xFF00, joypad | 0x08);
	}
	else if(isDirectionKeys)
	{
		if(keys[SDL_SCANCODE_RIGHT]) memory.write(0xFF00, joypad & 0xFE); else memory.write(0xFF00, joypad | 0x01);
		if(keys[SDL_SCANCODE_LEFT]) memory.write(0xFF00, joypad & 0xFD); else memory.write(0xFF00, joypad | 0x02);
		if(keys[SDL_SCANCODE_UP]) memory.write(0xFF00, joypad & 0xFB); else memory.write(0xFF00, joypad | 0x04);
		if(keys[SDL_SCANCODE_DOWN]) memory.write(0xFF00, joypad & 0xF7); else memory.write(0xFF00, joypad | 0x08);
	}
	return true;
}

void IO::updateTimers(Memory &memory)
{
	if(updateTimer.checkAndReset())
	{
		memory.write(0xFF04, memory.read(0xFF04) + 0x01);
		const byte TIMA = memory.read(0xFF05);
		if(TIMA == 0xFF)
		{
			memory.write(0xFF05, memory.read(0xFF06));
			memory.write(0xFF0F, memory.read(0xFF0F) | 0x04); // Set the IF register to say we are in Timer Interrupt
		}
		else
		{
			memory.write(0xFF05, TIMA + 0x01);
		}
	}
}