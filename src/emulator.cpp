#include "emulator.hpp"

#include "cpu.hpp"
#include "video.hpp"
#include "io.hpp"

static void init();
static void quit();

int Emulator::run(int argc, char **argv)
{
    init();

    if(argc < 2)
    {
        Gahood::criticalError("Invalid arguments passed, must at least pass the path to the rom file.");
    }

    char *romPath = argv[1];
    for(int i = 2; i < argc; i++)
    {
        if(Gahood::stringLiteralEquals(argv[i], "-d"))
        {
            Gahood::log("Debug mode enabled.");
            Gahood::setDebugMode(true);
        }
        else if(Gahood::stringLiteralEquals(argv[i], "-v"))
        {
            Gahood::log("Very verbose mode enabled.");
            Gahood::setVerboseMode(true);
        }
        else
        {
            Gahood::log("Ignoring passed argument %s.", argv[i]);
        }
    }

    Gahood::log("Loading ROM %s", romPath);

	Cartridge cartridge(romPath);
	Memory memory(cartridge);

	Cpu cpu;
	Video video(memory);
	IO io;

	cycle clocksSpent;
    while((clocksSpent = cpu.update(memory) >= 0) && io.update(memory))
    {
		video.render(memory, clocksSpent);
    }

    if(Gahood::isDebugMode())
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