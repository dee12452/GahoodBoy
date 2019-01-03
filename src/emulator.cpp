#include "emulator.hpp"

#include <SDL2/SDL_log.h>
#include "cpu.hpp"

void Emulator::run(int argc, char **argv)
{
    if(argc != 2)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Invalid arguments passed. You must pass only the path to the ROM");
        exit(EXIT_FAILURE);
    }
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Loading ROM %s", argv[1]);

    Cpu cpu(0x100);
    while(cpu.processNext(NULL, 0))
    {
        // Update emulator
    }
}