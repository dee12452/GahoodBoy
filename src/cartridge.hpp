#ifndef _GAHOOD_BOY_CARTRIDGE_HPP_
#define _GAHOOD_BOY_CARTRIDGE_HPP_

#include "util.hpp"

class Cartridge
{
    public:
        Cartridge(const char *romFile);
        Cartridge(const Cartridge &other);
        Cartridge& operator=(const Cartridge &other);
        ~Cartridge();

        char * getRomName() const;
        byte * getCartridgeMemory() const;
        size getCartridgeMemorySize() const;
        bool isCgbEnabled() const;
        bool isSgbEnabled() const;

    private:
        byte *cartridgeMemory;
        size cartridgeMemorySize;
        char *romName;
        bool cgb;
        bool sgb;
};

#endif
