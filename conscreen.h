#pragma once
#ifndef CONSCREEN_H
#define CONSCREEN_H

#include "SDL/SDL.h"

struct character {
	Uint8 ch;
	Uint8 col_bg, col_fg;
};

struct conscreen {
	Uint8 size_x, size_y;
	character* chars;
	conscreen(){};
	conscreen(Uint8 sx, Uint8 sy);
	void clear();
	void addchar(Uint8 c,Uint8 bg,Uint8 fg, Uint8 x, Uint8 y);
	void copy(conscreen* dst, Uint8 x, Uint8 y, Uint8 w, Uint8 h, bool O = false);
	void addstr(Uint8 x,Uint8 y,const char* str,Uint8 bg, Uint8 fg);
	void addhline(Uint8 x,Uint8 y, Uint8 w, Uint8 bg, Uint8 fg, Uint8 c=205);
	void draw(SDL_Surface* dst, Uint8 x=0, Uint8 y=0);
	~conscreen();
};

//extern SDL_Surface **const charswap;
extern SDL_Surface **const fontbmp;

#endif