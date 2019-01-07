#ifndef _GAHOOD_BOY_MEMORY_HPP_
#define _GAHOOD_BOY_MEMORY_HPP_

#include <stdint.h>
#include "rom.hpp"

class Memory
{
    public:
        Memory(const Rom &rom);
        Memory(const Memory &mem);
        Memory& operator=(const Memory &mem);
        ~Memory();

        uint8_t& readByte(const uint16_t memoryAddress) const;
        void writeByte(const uint8_t byte, const uint16_t memoryAddress);

    private:
        uint8_t *memory;
        uint16_t memorySize;
};

#endif