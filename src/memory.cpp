#include "memory.hpp"

Memory::Memory(const Cartridge &cartridge)
{
    memorySize = 0xFFFF;
    memoryBytes = (byte *) malloc(sizeof(byte) * (static_cast<unsigned long> (memorySize) + 0x01));
    for(address i = 0x00; i < memorySize; i += 0x01)
    {
        if(i <= 0x8000)
        {
            memoryBytes[i] = cartridge.getCartridgeMemory()[i];
        }
        else
        {
            memoryBytes[i] = 0x00;
        }
    }
}

Memory::Memory(const Memory &other)
{
    memorySize = 0xFFFF;
    memoryBytes = (byte *) malloc(sizeof(byte) * (static_cast<unsigned long> (memorySize) + 0x01));
    for(address i = 0; i <= memorySize; i+=0x01)
    {
        memoryBytes[i] = other.memoryBytes[i];
    }
}

Memory& Memory::operator=(const Memory &other)
{
    if(memoryBytes)
    {
        free(memoryBytes);
    }
    memorySize = 0xFFFF;
    memoryBytes = (byte *) malloc(sizeof(byte) * (static_cast<unsigned long> (memorySize) + 0x01));
    for(address i = 0; i <= memorySize; i+=0x01)
    {
        memoryBytes[i] = other.memoryBytes[i];
    }
    return *this;
}

Memory::~Memory()
{
    if(memoryBytes)
    {
        free(memoryBytes);
    }
    memorySize = 0x00;
}

byte Memory::read(const address addr) const
{
    return memoryBytes[addr];
}

void Memory::write(const address addr, const byte byteToWrite)
{
    memoryBytes[addr] = byteToWrite;
}