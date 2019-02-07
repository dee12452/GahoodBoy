#include "memory.hpp"

Memory::Memory(const Cartridge &cartridge)
{
    memorySize = 0xFFFF;
    memoryBytes = (byte *) malloc(sizeof(byte) * (static_cast<unsigned long> (memorySize) + 0x01));
    for(address i = 0x0000; i < memorySize; i += 0x0001)
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
    for(address i = 0x0000; i <= memorySize; i += 0x0001)
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
    for(address i = 0x0000; i <= memorySize; i += 0x0001)
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
    memorySize = 0x0000;
}

byte Memory::read(const address addr) const
{
    return memoryBytes[addr];
}

void Memory::write(const address addr, const byte byteToWrite)
{
    if(addr > memorySize)
    {
        Gahood::criticalError("Attempted to write to memory at out of bounds address %x", addr & 0xFFFF);
    }
	switch (addr)
	{
	case 0xFF46: // LCD OAM DMA Transfers
	{
		const address startAddr = static_cast<address> (byteToWrite << 8);
		const address endAddr = startAddr + 0x0100;
		for (address wrAddr = startAddr; wrAddr < endAddr; wrAddr += 0x01)
		{
			memoryBytes[(wrAddr & 0x00FF) | 0xFE00] = memoryBytes[wrAddr];
		}
		break;
	}
    case 0xFF41: // LCD status register; bit 0-2 are Read Only
    {
        if((byteToWrite & 0x07) == 0x07)
        {
            Gahood::criticalError("Address %x: Tried writing %x to Read Only bits", addr, byteToWrite);
        }
        else
        {
            memoryBytes[addr] = byteToWrite;
        }
        break;
    }
	default:
		memoryBytes[addr] = byteToWrite;
        break;
	}
}

void Memory::dumpToFile(const char *filePath) const
{
    Gahood::log("Dumping last memory state to %s", filePath);
    Gahood::writeToFile(filePath, memoryBytes, static_cast<size> (memorySize));
}