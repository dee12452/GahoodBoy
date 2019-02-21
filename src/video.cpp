#include "video.hpp"
#include <stdio.h>

Video::Video(Memory &memory)
{
	const unsigned int fpsMsTime = 1000 / GAHOOD_BOY_MAX_FPS;
	renderTimer = Timer(fpsMsTime);
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
	background = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 256, 256);
	if (!background)
	{
		Gahood::criticalSdlError("Failed to create the background texture");
	}

	if (SDL_SetRenderTarget(renderer, background) < 0)
	{
		Gahood::criticalSdlError("Failed to target the background texture");
	}
	if (SDL_RenderClear(renderer) < 0)
	{
		Gahood::criticalSdlError("Failed to clear the background texture");
	}
	if (SDL_SetRenderTarget(renderer, NULL) < 0)
	{
		Gahood::criticalSdlError("Failed to target the window");
	}
	if (SDL_RenderClear(renderer) < 0)
	{
		Gahood::criticalSdlError("Failed to clear the window");
	}
	SDL_RenderPresent(renderer);

	currentClocks = 0;
}

Video::~Video()
{
	SDL_DestroyTexture(background);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

void Video::render(Memory &memory, const cycle clocks)
{
	refresh(memory);
	if (!lcdEnabled)
	{
		return;
	}
	update(memory, clocks);
}

void Video::refresh(Memory &memory)
{
	const byte lcdControl = memory.read(0xFF40);
	lcdEnabled = Gahood::bitOn(lcdControl, 7);
	lcdWindowTileMapSelect = Gahood::bitOn(lcdControl, 6);
	lcdWindowDisplayEnabled = Gahood::bitOn(lcdControl, 5);
	lcdWindowBgTileSelect = Gahood::bitOn(lcdControl, 4);
	lcdBgTileMapDisplaySelect = Gahood::bitOn(lcdControl, 3);
	spriteSizeDetermine = Gahood::bitOn(lcdControl, 2);
	spriteSizeDisplayEnabled = Gahood::bitOn(lcdControl, 1);
	bgCgbDisplay = Gahood::bitOn(lcdControl, 0);

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
void Video::update(Memory &memory, const cycle clocks)
{
	currentClocks += clocks;

	// LYC Coincidence Flag
	if (lYCoord == lYCompare)
	{
		memory.write(0xFF41, lcdStatus | 0x04);
	}
	else
	{
		memory.write(0xFF41, lcdStatus & 0xFB);
	}

	switch (lcdStatus & 0x03)
	{
	case 0x00: // H-Blank 201-207 clks
		if (currentClocks >= 201)
		{
			if (lYCoord == static_cast<byte> (143))
			{
				memory.write(0xFF41, (lcdStatus & 0xFC) | 0x01);
			}
			else
			{
				memory.write(0xFF41, (lcdStatus & 0xFC) | 0x02);
			}
			memory.write(0xFF44, lYCoord + 1);
			currentClocks = 0;
		}
		break;
	case 0x01: // V-Blank 456 clks
	{
		const cycle clocksToPass = 456 / (153 - 144);
		if (lYCoord == static_cast<byte> (153))
		{
			if (currentClocks < clocksToPass)
			{
				break;
			}
			memory.write(0xFF41, (lcdStatus & 0xFC) | 0x02);
			memory.write(0xFF44, 0);
			currentClocks = 0;
		}
		if (currentClocks >= clocksToPass)
		{
			memory.write(0xFF44, lYCoord + 1);
			currentClocks = 0;
		}
		break;
	}
	case 0x02: // OAM-RAM Search 77-83 clks
		if (currentClocks >= 77)
		{
			memory.write(0xFF41, (lcdStatus & 0xFC) | 0x03);
		}
		break;
	case 0x03: // LCD Driver Transfer 169-175 clks
		if (currentClocks >= 169)
		{
			memory.write(0xFF41, (lcdStatus & 0xFC) | 0x00);
		}
		break;
	default:
		Gahood::criticalError("Undefined LCD mode %x", lcdStatus & 0x03);
	}

	if (renderTimer.checkAndReset())
	{
		draw(memory);
	}
}


void Video::draw(Memory &memory) const
{
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

	if (SDL_SetRenderTarget(renderer, background) < 0)
	{
		Gahood::criticalSdlError("Failed to target the background texture");
	}
	if (SDL_RenderClear(renderer) < 0)
	{
		Gahood::criticalSdlError("Failed to clear the background texture");
	}

	unsigned char bX = 0;
	unsigned char bY = 0;
	for (unsigned char row = 0; row < 32; row++)
	{
		for (unsigned char column = 0; column < 32; column++)
		{
			// lcdWindowBgTileSelect == true : $8000-$8FFF with unsigned pattern
			// else : $8800-$97FF with signed pattern
			const address currentTile = lcdWindowBgTileSelect ?
				0x8000 + (bgTileNums[row][column] * 16):
				0x9000 + (static_cast<signed char> (bgTileNums[row][column]) * 16);

			// draw pixels to background texture
		}
	}

	if (SDL_SetRenderTarget(renderer, NULL) < 0)
	{
		Gahood::criticalSdlError("Failed to target the window");
	}
	if (SDL_RenderClear(renderer) < 0)
	{
		Gahood::criticalSdlError("Failed to clear the window");
	}
	if (SDL_RenderCopy(renderer, background, NULL, NULL) < 0)
	{
		Gahood::criticalSdlError("Failed to draw the texture to the window");
	}
	SDL_RenderPresent(renderer);
}