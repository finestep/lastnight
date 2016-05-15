#include <assert.h>
#include <math.h>

#include "conscreen.h"
#include "colors.h"

conscreen::conscreen(Uint8 sx, Uint8 sy) {
	size_x = sx;
	size_y = sy;
	chars = (character*) malloc(sx*sy*sizeof(character));
	clear();
}

conscreen::~conscreen(){
	free(chars);
	chars = NULL;
}

void conscreen::clear() {
	memset(chars,0,size_x*size_y*sizeof(character));
}

void conscreen::addchar(Uint8 c,Uint8 bg,Uint8 fg, Uint8 x, Uint8 y) {
	assert(x>=0);
	assert(y>=0);
	assert(x<size_x);
	assert(y<size_y);
	void* addr=(void*)chars+sizeof(character)*(x+size_x*y);
	memset(addr,c,1);
	memset(addr+1,bg,1);
	memset(addr+2,fg,1);
	//printf("Added character %d at %i %i\n",c,x,y);
}

void conscreen::copy(conscreen* dst, Uint8 x, Uint8 y, Uint8 w, Uint8 h, bool O) {
	assert(x>=0&&w>=0);
	assert(y>=0&&h>=0);
	assert(w<size_x);
	assert(h<size_y);
	assert(x+w<dst->size_x);
	assert(y+h<dst->size_y);
	Uint32 ind;
	for(Uint8 ix=0;ix<=w;ix++) {
		for(Uint8 iy=0;iy<=h;iy++) {
			ind = (ix+size_x*iy);
			if(O || chars[ind].ch != 0) dst->addchar(chars[ind].ch,chars[ind].col_bg,chars[ind].col_fg,x+ix,y+iy);
		}
	}
}
void conscreen::addstr(Uint8 x,Uint8 y,const char* str,Uint8 bg, Uint8 fg){
	//assert(strlen(str)+x<=size_x);
	assert(y>=0&&x>=0&&y<=size_y&&x<=size_x);
	for(Uint8 i=0;i<strlen(str);i++) {
		if(x+i>size_x-1) return;
		addchar(str[i],bg,fg,x+i,y);
	}
}
void conscreen::addhline(Uint8 x,Uint8 y, Uint8 w,Uint8 bg, Uint8 fg,Uint8 c) {
	assert(w+x<=size_x);
	assert(y>=0&x>=0&&y<=size_y&&x<=size_x);
	for(Uint8 i=0;i<w;i++) {
		addchar(c,bg,fg,x+i,y);
	}
}
void conscreen::draw(SDL_Surface* dst,Uint8 x,Uint8 y) {
	SDL_Rect* glyphrect= new SDL_Rect();
	glyphrect->w=8;
	glyphrect->h=8;
	SDL_Rect* blitrect = new SDL_Rect();
	blitrect->w=8;
	blitrect->h=8;
	Uint8 r,g,b;
	SDL_Color col;
	SDL_Color blck=makecolor(0,0,0);
	SDL_Color palette[2];
	palette[0]=blck;
	character charac;
	//SDL_LockSurface(dst);
	for(Uint8 ix=0;ix<size_x;ix++) {
		for(Uint8 iy=0;iy<size_y;iy++) {
			charac=chars[ix+size_x*iy];

			glyphrect->x=(x+ix)*8;
			glyphrect->y=(y+iy)*8;
			SDL_FillRect(dst,glyphrect,(Uint32)charac.col_bg);
			if(charac.ch>0) {
				
				SDL_GetRGB(charac.col_fg,dst->format,&r,&g,&b);
				col=makecolor(r,g,b);
				palette[1]= col;
				SDL_SetPalette(*fontbmp,SDL_LOGPAL,palette,0,2);
				//printf("Printing character %i of color %i %i at %i %i\n",
				//		charac.ch,charac.col_bg,charac.col_fg,x+ix,y+iy);
				
				glyphrect->x=charac.ch%32*8;
				glyphrect->y=floor(charac.ch/32)*8;
				blitrect->x=(x+ix)*8;
				blitrect->y=(y+iy)*8;
				//printf("Blitting character %i to %i %i\n",chars[ind].ch,blitrect->x,blitrect->y);
				SDL_BlitSurface(*fontbmp,glyphrect,dst,blitrect);
					
			}
			//SDL_UpdateRects(dst,1,blitrect);
		}
	}
	//SDL_UnlockSurface(dst);
	SDL_UpdateRect(dst,x,y,x+size_x*8,y+size_y*8);
	
	delete glyphrect;
	delete blitrect;
}
