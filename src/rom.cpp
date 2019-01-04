#include "rom.hpp"

#include <string.h>
#include <SDL2/SDL.h>

const char * const Rom::ROM_GAMEBOY_FILE_EXTENSIONS[] = {".gb", ".gbc", "\0"};

bool isRomExtensionValid(const char * const[], const char *);

Rom::Rom(const char *romFilePath)
{
    if(!isRomExtensionValid(ROM_GAMEBOY_FILE_EXTENSIONS, romFilePath))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Invalid ROM given");
        exit(EXIT_FAILURE);
    }

    SDL_RWops *romFile = SDL_RWFromFile(romFilePath, "rb");
    if(!romFile)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to read ROM file: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    size_t currentMemorySize = 256; // start with 256 bytes allocated for memory
    programMemory = (uint8_t *) malloc(sizeof(uint8_t) * currentMemorySize);
    memorySize = 0;
    uint8_t byteBuffer[1];
    while(SDL_RWread(romFile, byteBuffer, sizeof(uint8_t), 1))
    {
        programMemory[memorySize] = byteBuffer[0];
        memorySize++;
        if(memorySize + 1 == currentMemorySize)
        {
            currentMemorySize *= 2;
            uint8_t *tmp = (uint8_t *) realloc(programMemory, sizeof(uint8_t) * currentMemorySize);
            if(!tmp)
            {
                exit(EXIT_FAILURE);
            }
            programMemory = tmp;
        }
    }

    // Try to remove the extra memory we took
    uint8_t *tmp = (uint8_t *) realloc(programMemory, sizeof(uint8_t) * memorySize);
    if(tmp)
    {
        programMemory = tmp;
    }
    programMemory[memorySize] = '\0';

    SDL_RWclose(romFile);
}

Rom::Rom(const Rom &otherRom)
{
    programMemory = (uint8_t *) malloc(sizeof(uint8_t) * otherRom.memorySize);
    for(size_t i = 0; i < otherRom.memorySize; i++)
    {
        programMemory[i] = otherRom.programMemory[i];
    }
    memorySize = otherRom.memorySize;
}

Rom& Rom::operator=(const Rom &otherRom)
{
    if(programMemory)
    {
        free(programMemory);
    }
    programMemory = (uint8_t *) malloc(sizeof(uint8_t) * otherRom.memorySize);
    for(size_t i = 0; i < otherRom.memorySize; i++)
    {
        programMemory[i] = otherRom.programMemory[i];
    }
    memorySize = otherRom.memorySize;
    return *this;
}

Rom::~Rom()
{
    if(programMemory)
    {
        free(programMemory);
    }
    memorySize = 0;
}

uint8_t * Rom::getProgramMemory() const
{
    return programMemory;
}

size_t Rom::getProgramMemorySize() const
{
    return memorySize;
}

bool isRomExtensionValid(const char * const validExtensions[], const char *romFilePath)
{
    size_t iterator = 0;
    while(validExtensions[iterator][0] != '\0')
    {
        const char *validExtension = validExtensions[iterator];
        size_t extIterator = 0, lastExt = 0;
        while(romFilePath[extIterator] != '\0')
        {
            if(romFilePath[extIterator] == '.')
            {
                lastExt = extIterator;
            }
            extIterator++;
        }
        extIterator = 0;
        while(validExtension[extIterator] == romFilePath[lastExt])
        {
            if(validExtension[extIterator] == '\0')
            {
                return true;
            }
            extIterator++;
            lastExt++;
        }
        iterator++;
    }
    return false;
}