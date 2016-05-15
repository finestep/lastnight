#pragma once
#ifndef COLORS_H
#define COLORS_H

#include "SDL/SDL.h"

//colors
#define COL_BLK 0
#define COL_WHT 1
#define COL_RED 2
#define COL_GRN 3
#define COL_MGT 4
#define COL_CYN 5
#define COL_GRY 6
#define COL_IDK 7


SDL_Color makecolor(Uint8 red,Uint8 green,Uint8 blue);
void makecolors(SDL_Color* pal);

#endif