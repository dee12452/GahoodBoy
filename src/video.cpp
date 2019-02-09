#include "video.hpp"
#include <stdio.h>

typedef unsigned char BitNumber;

Video::Video(Memory &memory)
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

	clockTimer.reset();
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

	lcdStatus = memory.read(0xFF41);
}

void Video::draw(Memory &memory)
{
	if (!lcdEnabled)
	{
		return;
	}

	switch (lcdStatus & 0x03)
	{
	case 0x00: // H-Blank 201-207 clks
		if (clockTimer.getElapsedClocks() >= 201)
		{
			if (lYCoord == static_cast<byte> (144))
			{
				memory.write(0xFF41, (lcdStatus & 0xFC) | 0x01);
			}
			else
			{
				memory.write(0xFF44, lYCoord + 1);
				memory.write(0xFF41, (lcdStatus & 0xFC) | 0x02);

				// LYC Coincidence Flag
				if (lYCoord == lYCompare)
				{
					memory.write(0xFF41, lcdStatus | 0x04);
				}
				else
				{
					memory.write(0xFF41, lcdStatus & 0xFB);
				}
			}
			clockTimer.reset();
		}
		break;
	case 0x01: // V-Blank 4560 clks
		if (lYCoord == static_cast<byte> (153) && clockTimer.getElapsedClocks() >= 456)
		{
			memory.write(0xFF44, 0);
			memory.write(0xFF41, (lcdStatus & 0xFC) | 0x02);

			// LYC Coincidence Flag
			if (0 == lYCompare)
			{
				memory.write(0xFF41, lcdStatus | 0x04);
			}
			else
			{
				memory.write(0xFF41, lcdStatus & 0xFB);
			}

			clockTimer.reset();
		}
		else
		{
			if (clockTimer.getElapsedClocks() >= (4560 - 456) / (153 - 144))
			{
				memory.write(0xFF44, lYCoord + 1);

				// LYC Coincidence Flag
				if (lYCoord == lYCompare)
				{
					memory.write(0xFF41, lcdStatus | 0x04);
				}
				else
				{
					memory.write(0xFF41, lcdStatus & 0xFB);
				}

				clockTimer.reset();
			}
		}
		break;
	case 0x02: // OAM-RAM Search 77-83 clks
		if (clockTimer.getElapsedClocks() >= 77)
		{
			memory.write(0xFF41, (lcdStatus & 0xFC) | 0x03);
			clockTimer.reset();
		}
		break;
	case 0x03: // LCD Driver Transfer 169-175 clks
		if (clockTimer.getElapsedClocks() >= 169)
		{
			memory.write(0xFF41, (lcdStatus & 0xFC) | 0x00);
			clockTimer.reset();
		}
		break;
	default:
		Gahood::criticalError("Undefined LCD mode %x", lcdStatus & 0x03);
	}

	byte bgTileNums[32][32];
	address start, end;
	if(lcdBgTileMapDisplaySelect) // 9C00-9FFF
	{
		start = 0x9C00;
		end = 0x9FFF;
	}
	else // 9800-9BFF
	{
		start = 0x9800;
		end = 0x9BFF;
	}
	byte row = 0, col = 0;
	address addr;
	for(addr = start; addr <= end; addr += 0x01)
	{
		if(col != 0 && col % 32 == 0)
		{
			row++;
			col = 0;
		}
		bgTileNums[row][col] = memory.read(addr);
		col++;
	}

	if(lcdWindowBgTileSelect) // $8000-$8FFF
	{

	}
	else // $8800-$97FF
	{

	}

	if (SDL_RenderClear(renderer) < 0)
	{
		Gahood::criticalSdlError("Failed to clear the current screen");
	}
	SDL_RenderPresent(renderer);
}