#include "memory.hpp"

#include <SDL2/SDL.h>

static const uint16_t GAMEBOY_MEMORY_MAP_SIZE = 0xFFFF;

Memory::Memory(const Rom &rom)
{
    memorySize = GAMEBOY_MEMORY_MAP_SIZE;
    memory = (uint8_t *) malloc(sizeof(uint8_t) * GAMEBOY_MEMORY_MAP_SIZE);
    for(uint16_t i = 0; i < memorySize; i++)
    {
        if(i < rom.getProgramMemorySize())
        {
            memory[i] = rom.getProgramMemory()[i];
        }
        else
        {
            memory[i] = 0x00;
        }
    }
}

Memory::Memory(const Memory &mem)
{
    memorySize = mem.memorySize;
    memory = (uint8_t *) malloc(sizeof(uint8_t) * memorySize);
    for(uint16_t i = 0; i < memorySize; i++)
    {
        memory[i] = mem.memory[i];
    }
}

Memory& Memory::operator=(const Memory &mem)
{
    if(memory)
    {
        free(memory);
    }
    memorySize = mem.memorySize;
    memory = (uint8_t *) malloc(sizeof(uint8_t) * memorySize);
    for(uint16_t i = 0; i < memorySize; i++)
    {
        memory[i] = mem.memory[i];
    }
    return *this;
}

Memory::~Memory()
{
    if(memory)
    {
        free(memory);
    }
    memorySize = 0;
}

uint8_t& Memory::readByte(const uint16_t memoryAddress) const
{
    if(memoryAddress >= memorySize)
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Tried reading byte at out of bounds memory address %x", memoryAddress & 0xFFFF);
        exit(EXIT_FAILURE);
    }
    return memory[memoryAddress];
}

void Memory::writeByte(const uint8_t byte, const uint16_t memoryAddress)
{
    if(memoryAddress >= memorySize)
    {
        SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "Tried writing byte %x at out of bounds memory address %x", byte & 0xFF, memoryAddress & 0xFFFF);
        exit(EXIT_FAILURE);
    }
    memory[memoryAddress] = byte;
}
