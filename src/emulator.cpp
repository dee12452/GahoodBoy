#include "emulator.hpp"

#include "cpu.hpp"
#include "video.hpp"

static void init();
static void quit();

int Emulator::run(int argc, char **argv)
{
    init();

    if(argc != 2)
    {
        Gahood::criticalError("Invalid arguments passed. You must pass only the path to the ROM");
    }
    Gahood::log( "Loading ROM %s", argv[1]);

	Cpu cpu;
	Video video;
    Cartridge cartridge(argv[1]);
    Memory memory(cartridge);

    while(cpu.process(memory))
    {
        // Update emulator
		video.render(memory);
    }

    quit();

	return 0;
}

static void init()
{
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
    {
        Gahood::criticalSdlError("Failed to initialize SDL2");
    }
}

static void quit()
{
    SDL_Quit();
}