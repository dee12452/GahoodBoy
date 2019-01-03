#include "cpu.hpp"

#include <SDL2/SDL_log.h>

Cpu::Cpu(const uint16_t programCounterStart)
{
    registers.A = 0x00;
    registers.B = 0x00;
    registers.D = 0x00;
    registers.H = 0x00;
    registers.flags = 0x0;
    registers.C = 0x0;
    registers.E = 0x0;
    registers.L = 0x0;
    registers.stackPointer = 0x00;
    registers.programCounter = programCounterStart;
}

bool Cpu::processNext(uint8_t *memory, uint16_t memorySize)
{
    if(!memory || memorySize == 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "CPU tried processing empty memory. Was game ROM loaded properly?\n");
        return false;
    }

    if(registers.programCounter > memorySize) 
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "CPU program counter is outside the bounds of the memory size\n");
        return false;
    }

    const uint16_t nextOpCode = memory[registers.programCounter];
    switch(nextOpCode)
    {
        default:
            SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "CPU encountered unknown op-code %x\n", nextOpCode & 0xff);
            return false;
    }
    return true;;
}