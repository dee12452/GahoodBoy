#include "video.hpp"

typedef unsigned char BitNumber;

Video::Video()
{
	window = SDL_CreateWindow("GahoodBoy", 100, 100, 500, 500, SDL_WINDOW_OPENGL);
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

	if (!window)
	{
		Gahood::criticalSdlError("Failed to create the window");
	}
	if (!renderer)
	{
		Gahood::criticalSdlError("Failed to create the window renderer");
	}
	if (SDL_RenderSetLogicalSize(renderer, 500, 500) < 0)
	{
		Gahood::criticalSdlError("Failed to clip the resolution");
	}
}

Video::~Video()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

void Video::render(Memory &memory)
{
	refresh(memory);
	draw(memory);
}

bool bitOn(const byte checkByte, const BitNumber bitNum)
{
	switch (bitNum)
	{
	case 0:
		return (checkByte & 0x01) == 0x01;
	case 1:
		return (checkByte & 0x02) == 0x02;
	case 2:
		return (checkByte & 0x04) == 0x04;
	case 3:
		return (checkByte & 0x08) == 0x08;
	case 4:
		return (checkByte & 0x10) == 0x10;
	case 5:
		return (checkByte & 0x20) == 0x20;
	case 6:
		return (checkByte & 0x40) == 0x40;
	case 7:
		return (checkByte & 0x80) == 0x80;
	default:
		Gahood::log("Warning: check byte reached unexpected value %x", checkByte & 0xFF);
		return false;
	}
}

void Video::refresh(Memory &memory)
{
	const byte lcdControl = memory.read(0xFF40);
	lcdEnabled = bitOn(lcdControl, 7);
	lcdWindowTileMapSelect = bitOn(lcdControl, 6);
	lcdWindowDisplayEnabled = bitOn(lcdControl, 5);
	lcdWindowBgTileSelect = bitOn(lcdControl, 4);
	lcdBgTileMapDisplaySelect = bitOn(lcdControl, 3);
	spriteSizeDetermine = bitOn(lcdControl, 2);
	spriteSizeDisplayEnabled = bitOn(lcdControl, 1);
	bgCgbDisplay = bitOn(lcdControl, 0);

	scrollX = memory.read(0xFF42);
	scrollY = memory.read(0xFF43);
	lYCoord = memory.read(0xFF44);
	lYCompare = memory.read(0xFF45);
	windowX = memory.read(0xFF4B) - 0x07;
	windowY = memory.read(0xFF4A);
	bgPallette = memory.read(0xFF47);
	objPallette0 = memory.read(0xFF48);
	objPallette1 = memory.read(0xFF49);
}

void Video::draw(Memory &)
{
	if (SDL_RenderClear(renderer) < 0)
	{
		Gahood::criticalSdlError("Failed to clear the current screen");
	}
	SDL_RenderPresent(renderer);
}