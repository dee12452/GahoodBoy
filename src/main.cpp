#include <SDL2/SDL.h> // Needed for Android
#include "emulator.hpp"

int main(int argc, char **argv)
{
    return Emulator::run(argc, argv);
}