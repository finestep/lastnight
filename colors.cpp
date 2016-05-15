#include "colors.h"

SDL_Color makecolor(Uint8 red,Uint8 green,Uint8 blue) {
SDL_Color col;
col.r=red;
col.g=green;
col.b=blue;
col.unused=0;
//printf("Created color %i %i %i\n",red,green,blue);
return col;
}

void makecolors(SDL_Color* pal){
SDL_Color C_BLACK=makecolor(0,0,0);
SDL_Color C_WHITE=makecolor(255,255,255);
SDL_Color C_RED=makecolor(255,0,0);
SDL_Color C_GREEN=makecolor(0,255,0);
SDL_Color C_MAGENTA=makecolor(255,0,255);
SDL_Color C_CYAN=makecolor(0,255,255);
SDL_Color C_GRAY=makecolor(123,123,123);
SDL_Color C_IDK=makecolor(0,0,0);
SDL_Color pal2[8]={C_BLACK,C_WHITE,C_RED,C_GREEN,C_MAGENTA,C_CYAN,C_GRAY,C_IDK};
memcpy((void*)pal,(void*)pal2,8*sizeof(SDL_Color));
printf("Copied palette to memory\n");
}