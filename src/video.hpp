#ifndef _GAHOOD_BOY_VIDEO_HPP_
#define _GAHOOD_BOY_VIDEO_HPP_

#include "memory.hpp"
#include "timer.hpp"

class Video
{
public:
	Video(Memory &memory);
	~Video();

	void render(Memory &memory, const cycle clocks);

private:
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *background;

	bool lcdEnabled;
	bool lcdWindowTileMapSelect;
	bool lcdWindowDisplayEnabled;
	bool lcdWindowBgTileSelect;
	bool lcdBgTileMapDisplaySelect;
	bool spriteSizeDetermine;
	bool spriteSizeDisplayEnabled;
	bool bgCgbDisplay;

	byte scrollX;
	byte scrollY;
	byte lYCoord;
	byte lYCompare;
	byte windowX;
	byte windowY;
	byte bgPallette;
	byte objPallette0;
	byte objPallette1;
	byte lcdStatus;

	Timer renderTimer;
	cycle currentClocks;

	void refresh(Memory &memory);
	void update(Memory &memory, const cycle clocks);
	void draw(Memory &memory) const;
};

#endif