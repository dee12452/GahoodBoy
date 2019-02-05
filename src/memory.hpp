#ifndef _GAHOOD_BOY_MEMORY_HPP_
#define _GAHOOD_BOY_MEMORY_HPP_

#include "cartridge.hpp"

class Memory
{
    public:
        Memory(const Cartridge &cartridge);
        Memory(const Memory &other);
        Memory& operator=(const Memory &other);
        ~Memory();

        byte read(const address addr) const;
        void write(const address addr, const byte byteToWrite);
        void dumpToFile(const char *filePath) const;

    private:
        byte *memoryBytes;
        address memorySize;
};

#endif