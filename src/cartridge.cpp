#include "cartridge.hpp"

static char * readRomName(byte *cartridgeMemory);
static void checkHeaderChecksum(byte *cartridgeMemory);

Cartridge::Cartridge(const char *romFile)
{
    cartridgeMemory = Gahood::readFileAsBytes(romFile, cartridgeMemorySize);
    if(cartridgeMemorySize < 0x8000)
    {
        Gahood::criticalError("Cartridge size is less than expected, it is likely corrupted.");
    }
    romName = readRomName(cartridgeMemory);
    cgb = cartridgeMemory[0x0143] == 0x80 || cartridgeMemory[0x0143] == 0xC0;
    sgb = cartridgeMemory[0x0146] == 0x03;
    checkHeaderChecksum(cartridgeMemory);
}

Cartridge::Cartridge(const Cartridge &other)
{
    cartridgeMemorySize = other.cartridgeMemorySize;
    if(cartridgeMemorySize < 0x8000)
    {
        Gahood::criticalError("Cartridge size is less than expected, it is likely corrupted.");
    }
    cartridgeMemory = (byte *) malloc(sizeof(byte) * cartridgeMemorySize);
    for(size_t i = 0; i < cartridgeMemorySize; i++)
    {
        cartridgeMemory[i] = other.cartridgeMemory[i];
    }
    romName = readRomName(cartridgeMemory);
    cgb = cartridgeMemory[0x0143] == 0x80 || cartridgeMemory[0x0143] == 0xC0;
    sgb = cartridgeMemory[0x0146] == 0x03;
    checkHeaderChecksum(cartridgeMemory);
}

Cartridge& Cartridge::operator=(const Cartridge &other)
{
    cartridgeMemorySize = other.cartridgeMemorySize;
    if(cartridgeMemorySize < 0x8000)
    {
        Gahood::criticalError("Cartridge size is less than expected, it is likely corrupted.");
    }
    if(cartridgeMemory)
    {
        free(cartridgeMemory);
    }
    if(romName)
    {
        free(romName);
    }
    cartridgeMemory = (byte *) malloc(sizeof(byte) * cartridgeMemorySize);
    for(size_t i = 0; i < cartridgeMemorySize; i++)
    {
        cartridgeMemory[i] = other.cartridgeMemory[i];
    }
    romName = readRomName(cartridgeMemory);
    cgb = cartridgeMemory[0x0143] == 0x80 || cartridgeMemory[0x0143] == 0xC0;
    sgb = cartridgeMemory[0x0146] == 0x03;
    checkHeaderChecksum(cartridgeMemory);
    return *this;
}

Cartridge::~Cartridge()
{
    if(cartridgeMemory)
    {
        free(cartridgeMemory);
    }
    if(romName)
    {
        free(romName);
    }
    cartridgeMemorySize = 0;
}

char * Cartridge::getRomName() const
{
    return romName;
}

byte * Cartridge::getCartridgeMemory() const
{
    return cartridgeMemory;
}

size_t Cartridge::getCartridgeMemorySize() const
{
    return cartridgeMemorySize;
}

bool Cartridge::isCgbEnabled() const
{
    return cgb;
}

bool Cartridge::isSgbEnabled() const
{
    return sgb;
}

static char * readRomName(byte *cartridgeMemory)
{
    char *romName = (char *) malloc(sizeof(char) * 17);
    size_t index = 0;
    for(address addr = 0x0134; addr < 0x0143; addr += 0x01, index++)
    {
        romName[index] = cartridgeMemory[addr];
    }
    romName[index] = '\0';
    return romName;
}

static void checkHeaderChecksum(byte *cartridgeMemory)
{
    byte x = 0;
    for(address i = 0x0134; i <= 0x014C; i += 0x01)
    {
        x = x - cartridgeMemory[i] - 1;
    }
    if(x != cartridgeMemory[0x014D])
    {
        Gahood::criticalError("Header checksum failed for cratridge");
    }
}