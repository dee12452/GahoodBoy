#ifndef _GAHOOD_BOY_VIDEO_HPP_
#define _GAHOOD_BOY_VIDEO_HPP_

#include "memory.hpp"

class Video
{
public:
	Video();
	~Video();

	void render(Memory &memory);

private:
	SDL_Window *window;
	SDL_Renderer *renderer;

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

	void refresh(Memory &memory);
	void draw(Memory &memory);
};

#endif