#include "emulator.hpp"

#include <SDL2/SDL.h>
#include "rom.hpp"
#include "cpu.hpp"
#include "utils.hpp"

static const uint16_t GAMEBOY_PROGRAM_COUNTER_START = 0x100;
static const uint16_t GAMEBOY_STACK_POINTER_START = 0xFFFE;

static void init();
static void quit();

void Emulator::run(int argc, char **argv)
{
    init();

    if(argc != 2)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Invalid arguments passed. You must pass only the path to the ROM");
        exit(EXIT_FAILURE);
    }
    SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "Loading ROM %s", argv[1]);

    Rom rom(argv[1]);

    Cpu cpu(GAMEBOY_PROGRAM_COUNTER_START, GAMEBOY_STACK_POINTER_START);
    while(cpu.processNext(rom.getProgramMemory(), rom.getProgramMemorySize()))
    {
        // Update emulator
    }

    quit();
}

static void init()
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize SDL2: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}

static void quit()
{
    SDL_Quit();
}