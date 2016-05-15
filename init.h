#pragma once
#include "SDL/SDL.h"
#include "colors.h"
#include "conscreen.h"

SDL_Surface* init_screen(Uint8 size_x, Uint8 size_y) {
	printf("Creating screen...\n");
	SDL_Surface* screen = SDL_SetVideoMode(size_x*8,size_y*8,8,SDL_HWSURFACE);
	
	SDL_Color screenpalette[8];
	makecolors(screenpalette);

//for(Uint8 i=0;i<8;i++) {
//		printf("Element %i of array: %i %i %i\n",i,screenpalette[i].r,screenpalette[i].g,screenpalette[i].b);
//	}

	SDL_SetPalette(screen,SDL_LOGPAL||SDL_PHYSPAL,screenpalette,0,8);
	if(screen==NULL) {
		printf("Error creating screen: %s\n",SDL_GetError());
		exit(2);	
	}
	return screen;
}

void init_font(SDL_Surface* &font) {
	printf("loading font...\n");
	font = SDL_LoadBMP("font.bmp");
	if(font==0) {
		printf("Error loading bmp: %s\n",SDL_GetError());
		exit(3);
	}
	//SDL_SetPalette(fntbmp,SDL_LOGPAL||SDL_PHYSPAL,screenpalette,0,2);
	SDL_Color col1=font->format->palette->colors[0];
	Uint32 col2=SDL_MapRGB(font->format,col1.r,col1.g,col1.b);
	SDL_SetColorKey(font,SDL_SRCCOLORKEY,col2);
}

SDL_Surface* make_icon() {
	conscreen* letter = new conscreen(1,1);
	letter->addchar('Z',0,1,0,0);
	SDL_Surface* icon = SDL_CreateRGBSurface(SDL_SWSURFACE,8,8,8,0,0,0,0);
	SDL_FillRect(icon,NULL,0);
	SDL_Color palette[2]={makecolor(0,0,0),makecolor(0,2550,0)};
	SDL_SetPalette(icon,SDL_PHYSPAL|SDL_LOGPAL,palette,0,2);
	SDL_SetColorKey(icon,SDL_SRCCOLORKEY,0);
	letter->draw(icon);
	delete letter;
	printf("Generated icon...\n");
	return icon;
}