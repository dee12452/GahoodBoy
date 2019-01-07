#ifndef _GAHOOD_BOY_ROM_HPP_
#define _GAHOOD_BOY_ROM_HPP_

#include <stdint.h>

class Rom
{
    private:
        static const char * const ROM_GAMEBOY_FILE_EXTENSIONS[];

    public:
        Rom(const char *romFilePath);
        Rom(const Rom &otherRom);
        Rom& operator=(const Rom &otherRom);
        ~Rom();

        uint8_t * getProgramMemory() const;
        uint16_t getProgramMemorySize() const;

    private:
        uint8_t *programMemory;
        uint16_t memorySize;
};

#endif