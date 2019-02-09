#include "emulator.hpp"

#include "cpu.hpp"
#include "video.hpp"
#include "io.hpp"

static void init();
static void quit();

int Emulator::run(int argc, char **argv)
{
    init();

    char *romPath = NULL;
    bool isDebug = false;
    if(argc == 2)
    {
        romPath = argv[1];
    }
    else if(argc == 3 && Gahood::stringLiteralEquals(argv[2], "debug"))
    {
        romPath = argv[1];
        isDebug = true;
    }
    else
    {
        Gahood::criticalError("Invalid arguments passed");
    }

    Gahood::log( "Loading ROM %s", romPath);

	Cartridge cartridge(romPath);
	Memory memory(cartridge);

	Cpu cpu;
	Video video(memory);
	IO io;

    while(cpu.process(memory) && io.update(memory))
    {
		video.render(memory);
    }

    if(isDebug)
    {
        memory.dumpToFile("debug/memoryDump.txt");
    }

    quit();

	Gahood::log("Quitting emulator");

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