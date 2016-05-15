#include <cstdio>
#include <ctime>

#include "SDL/SDL.h"

#include "init.h"

#define VERSION 1.25



/*
//qweasdzxc
#define K_Q 113 
#define K_W 119
#define K_E 101
#define K_A 97
#define K_S 115
#define K_D 100 
#define K_Z 122
#define K_X 120
#define K_C 99 */

#define K_MN keys[0]
#define K_ME keys[1]
#define K_MS keys[2]
#define K_MW keys[3]
#define K_MNE keys[4]
#define K_MSE keys[5]
#define K_MNW keys[6]
#define K_MSW keys[7]

#define K_AN keys[8]
#define K_AE keys[9]
#define K_AS keys[10]
#define K_AW keys[11]
#define K_ANE keys[12]
#define K_ASE keys[13]
#define K_ANW keys[14]
#define K_ASW keys[15]

#define K_S keys[16]



//glyphs//////////////
//ents
#define CH_AVA 64
#define CH_ZED 90
//sword
#define CH_S0 124
#define CH_S1 47
#define CH_S2 196
#define CH_S3 92
#define CH_S4 124
#define CH_S5 47
#define CH_S6 196
#define CH_S7 92
//slash
#define CH_SL0 246
#define CH_SL1 254
#define CH_SL2 219
//gore
#define CH_BL0 247
#define CH_BL1 126

#define CH_SPLT 176

#define CH_CRPS 37

#define CH_MNGL0 38
#define CH_MNGL1 225

#define SIZE_X 70
#define SIZE_Y 70

#define ZED_MAX 255
#define EFF_MAX 255

typedef unsigned long ulong;
typedef unsigned int uint;
typedef unsigned char uchar;

SDLKey getkey(Sint32 delay){
	SDL_Event event;
	Sint32 time=delay;
	event.type=NULL;
	SDLKey key=SDLK_UNKNOWN;
	if(delay<0) {
		while(event.type!=SDL_KEYDOWN) {
			if(event.type==SDL_QUIT) exit(0);
		SDL_WaitEvent(&event);
		if(event.type==SDL_KEYDOWN)	key=(SDLKey)event.key.keysym.sym;
		}
		printf("%i\n",int(key));
		return key;
	} else {
		while(time>=0) {
			SDL_PollEvent(&event);
			SDL_Delay(5);
			time-=5;
			if(event.type==SDL_QUIT) exit(0);
			if(event.type==SDL_KEYDOWN) return event.key.keysym.sym;
		}
	}
}


struct zed {
	uchar x,y,move,attack;
	char health;
};

struct eff {
	uchar x,y,movephase;
	char vx,vy;
	int life;
	long ch;
	uchar bg,fg;
	bool stay;
};

uint seed1 = 1916299;
uint seed2 = 1001507;
uint seed3;

uint startseed;

SDL_Surface* fntbmp;
SDL_Surface** const fontbmp=&fntbmp;

uint xorrandom(void) {
	uint swap = seed1;
	uint a;
	 a = (seed1<<1)^(seed2<<5)^(seed3<<5);
	seed1 ^=(seed1<<7)^(seed2<<5);
	seed2 ^=(seed2<<5)^(seed3>>12);
	seed3 ^=(seed3<<1)^(swap<<1);
	return a;
}

eff* effs[EFF_MAX] = { NULL };
uint effcount=0;

void add_eff(uchar x,uchar y,char vx, char vy,long ch,int life,uchar bg,uchar fg, bool stay) {
	if(effcount==EFF_MAX-1) return;
	eff* e = new eff;
	e->x=x;
	e->y=y;
	e->vx=vx;
	e->vy=vy;
	e->ch=ch;
	e->life=life;
	e->bg=bg;
	e->fg=fg;
	e->stay=stay;
	e->movephase=0;
	uchar i=0;
	while(effs[i]!=NULL) {i++; if(i>EFF_MAX) i=0; }
	effs[i]=e;
	effcount++;
	return;
}

SDL_Surface* screen;
conscreen* console;
SDL_Surface* icon;

void init() {

	printf("init\n");
	if( SDL_Init(SDL_INIT_VIDEO)<0 ) {
		printf("Init error: %s",SDL_GetError());
		exit(1);
	}
	atexit(SDL_Quit);
	//chswp = SDL_CreateRGBSurface(0,8,8,8,0,0,0,0);
	screen=init_screen(SIZE_X,SIZE_Y+2);
	console=new conscreen(SIZE_X,SIZE_Y+2);
	init_font(fntbmp);
	icon=make_icon();
	SDL_WM_SetIcon(icon,NULL);
	SDL_WM_SetCaption("Last Night","Last Night");
}

#undef main

int main( int argc, char* args[] ) {

	bool god = false;
	bool seedoverride=false;

	init();
	char tmpstr[SIZE_X];
	
	for(int i=1;i<argc;i++) {
		char* arg = args[i];

		if(strcmp(arg,"-keyconf")==0) {
			char* dirs[8] = {"N","E","S","W","NE","SE","NW","SW"};
			long keys[17]={0};
			long ch;
			int pos, n;

			sprintf(tmpstr,"Last Night key config, esc to cancel");
			memset(tmpstr,0,70);
			console->addstr(0,0,tmpstr,0,1);
			for(int a=0;a<8;a++) {
				sprintf(tmpstr,"Press the key for move %s: %n",dirs[a],&n);
				console->addstr(0,1+a,tmpstr,0,1);
				console->draw(screen);
				memset(tmpstr,0,70);
				ch=getkey(-1);
				printf("%i: %i\n",a,ch);
				if(ch==27) exit(0);
				sprintf(tmpstr,"%s, keynum %i",SDL_GetKeyName((SDLKey)ch),ch);
				console->addstr(n+1,1+a,tmpstr,0,1);
				console->draw(screen);
				memset(tmpstr,0,70);
				keys[a]=ch;
			}
			for(int a=0;a<8;a++) {
				sprintf(tmpstr,"Press the key for attack %s: %n",dirs[a],&n);
				console->addstr(0,9+a,tmpstr,0,1);
				memset(tmpstr,0,70);
				console->draw(screen);
				ch=getkey(-1);
				printf("%i: %i\n",a+8,ch);
				if(ch==27) exit(0);
				sprintf(tmpstr,"%s, keynum %i",SDL_GetKeyName((SDLKey)ch),ch);
				console->addstr(n+1,9+a,tmpstr,0,1);
				memset(tmpstr,0,70);
				console->draw(screen);
				keys[a+8]=ch;
			}
				sprintf(tmpstr,"Press the key for change stance: %n",&n);
				console->addstr(0,17,tmpstr,0,1);
				memset(tmpstr,0,70);
				console->draw(screen);
				ch=getkey(-1);
				printf("16: %i\n",ch);
				if(ch==27) exit(0);
				sprintf(tmpstr,"%s, keynum %i",SDL_GetKeyName((SDLKey)ch),ch);
				console->addstr(n+1,17,tmpstr,0,1);
				memset(tmpstr,0,70);
				keys[16]=ch;
				SDL_Delay(200);

			SDL_RWops* file = SDL_RWFromFile("controls.dat", "w");
			SDL_RWwrite(file,keys,sizeof(long),17);
			SDL_RWclose(file);

			for(int i=0;i<17;i++) {
			printf("%i",long(keys[i]));
			}

			exit(0);
		} else if(strcmp(arg,"-seed")==0 && argc>i) {
			seed3 = atoi(args[i+1]);
			printf("Seed overridden with %i",seed3);
			seedoverride=true;
		}
	}

	long keys[17];
	SDL_RWops* file = SDL_RWFromFile("controls.dat", "r");
	if(file!=NULL) {
	SDL_RWread(file,keys,sizeof(long),17);
	SDL_RWclose(file);
	} else {
		sprintf(tmpstr,"Error loading controls. Please reconfigure them.");
		console->addstr(0,0,tmpstr,0,1);
		console->draw(screen);
		memset(tmpstr,0,70);
		getkey(-1);
		exit(2);
	}
	
	//printf("Using seed %i\n",seed3);


	//Intro////////////////////////////////

	int x,y;
	uchar n = xorrandom()%6;
	for(uchar i=0;i<12+n;i++) {
	x = xorrandom()%(SIZE_X-1);
	y = 10+xorrandom()%(SIZE_Y-11);
	console->addchar('"',COL_BLK,COL_RED,x,y);
	console->draw(screen);
	if (getkey(abs(320+i*i)) > 0) break;
	}
	console->clear();
	if( getkey(500) ) {
	SDL_FillRect(screen,NULL,COL_BLK);
	SDL_UpdateRect(screen,0,0,0,0);
		if(getkey(80)) {
		SDL_FillRect(screen,NULL,COL_WHT);
		SDL_UpdateRect(screen,0,0,0,0);
			if(getkey(200)) {
			SDL_FillRect(screen,NULL,COL_BLK);
			SDL_UpdateRect(screen,0,0,0,0);
			}
		}
	}
	//Main screen//////////////////////////////////////////////

	char* titlestr[4];
	titlestr[0] = "1l        A      ,sSs.  ttTtt     n\\  N  '*II*`  ,ggG,  H   H  ttTtt";
	titlestr[1] = "ll       a^a     sS,      T       N'\\ N    II    g   `  h,.,h    T";
	titlestr[2] = "lL      A`='A     `Sss    T       N \\.N    II    g `^g  H''`H    T";
	titlestr[3] = "LllLL  aa   aa   \"Sss'    t       N  \\n  .iIIi.  'GgG'  H   H    t";
	
	for(int i=0;i<4;i++) {
	console->addstr(1,6+i,titlestr[i],COL_BLK,COL_RED);
	}
	sprintf(tmpstr,"SDL %d.%d.%d",SDL_MAJOR_VERSION,SDL_MINOR_VERSION,SDL_PATCHLEVEL);
	console->addstr(30,64,tmpstr,COL_MGT,COL_WHT);
	memset(tmpstr,0,70);
	console->addstr(45,12,"C FinDude 2012",COL_BLK,COL_WHT);
	console->addstr(20,22,"Press the any key to start.",COL_BLK,COL_WHT);
	sprintf(tmpstr,"V %.1f",VERSION);
	console->addstr(SIZE_X-8,SIZE_Y+1,tmpstr,COL_BLK,COL_WHT);
	console->draw(screen);

	if (getkey(-1)=='z')
		if(getkey(500)=='e')
			if(getkey(500)=='d') {god=true; printf("Godmode enabled!\n"); }


	bool cont;
	conscreen* map = new conscreen(SIZE_Y,SIZE_X);
	zed* zeds[ZED_MAX];
	bool grabbed[ZED_MAX];

//OUTER GAME LOOP///////////////////////////////////////////////////////////////////////////////////////////
	do {

	if(!seedoverride) seed3 = time(NULL)%30000000;
	printf("New game!\nUsing seed %i\n",seed3);

	//GAMEVARS/////////////////////////////////////////////
	//AVATAR//////////
	char HP=100;
	uchar posx=SIZE_X/2,posy=(SIZE_Y-2)/2;
	uint kills = 0;
	uchar swordframe=0, swordtime=3, sworddir=0, sworddelay=0, swordspeed=7;
	uchar movespeed=3,movedelay=0;
	bool stance=false;
	char slashstrength=0;
	bool sworded=false;
	//GAME///////////
	long c=0;
	cont=false;
	long startTime;

	map->clear();
	//int x,y;
	//wcolor_set(map,COL_ZED,0);
	for(int mx=0;mx<SIZE_X;mx++) {
		for(int my=0;my<SIZE_Y;my++) {
			if(xorrandom()%11<3) {
				if(xorrandom()%5==0) map->addchar('.',COL_BLK,COL_GRN,mx,my); else map->addchar(',',COL_BLK,COL_GRN,mx,my);
			}
		}
	}

	for(uchar i=0;i<20+xorrandom()%14;i++) {
	x = xorrandom()%(SIZE_X-1);
	y = xorrandom()%(SIZE_Y-1);
	map->addchar('"',COL_BLK,COL_GRN,x,y);
	} 
	for(uchar i=0;i<15+xorrandom()%23;i++) {
	x = xorrandom()%(SIZE_X-1);
	y = xorrandom()%(SIZE_Y-1);
	map->addchar(';',COL_BLK,COL_GRN,x,y);
	}


	startTime = SDL_GetTicks();
	memset(effs,0,sizeof(eff*)*EFF_MAX);
	//ZEDS//////////
	memset(zeds,0,ZED_MAX*sizeof(zed*));
	memset(grabbed,false,ZED_MAX*sizeof(bool));
	uint zedcount=0;
	uchar zedhealth=30;
	uchar zedmove=15;
	uchar zedattack = 40;

//GAME LOOP///////////////////////////////////////////////////////////////////////////////////////////
	do {
		//map
		
	//	copywin(map,stdscr,0,0,3,0,SIZE_Y+1,SIZE_X,false);
		map->copy(console,0,2,SIZE_X-1,SIZE_Y-1,true);
		//Controls///////////////////////////////////////////
		if(c=='k') HP=0;
		char mx=0,my=0;
		
		/*
		if(c==KEY_A1 || c==KEY_B1 || c==KEY_C1) if(posx>1) mx--;
		if(c==KEY_A3 || c==KEY_B3 || c==KEY_C3) if(posx<SIZE_X-2) mx++;
		if(c==KEY_A1 || c==KEY_A2 || c==KEY_A3) if(posy>4) my--;
		if(c==KEY_C1 || c==KEY_C2 || c==KEY_C3) if(posy<SIZE_Y) my++;
		
		if(c==7 || c==4 || c==1) if(posx>1) mx--;
		if(c==9 || c==6 || c==3) if(posx<SIZE_X-2) mx++;
		if(c==7 || c==8 || c==9) if(posy>4) my--;
		if(c==1 || c==2 || c==3) if(posy<SIZE_Y-1) my++;
		*/
		if(c==K_MNW || c==K_MW || c==K_MSW) if(posx>1) mx--; //move west
		if(c==K_MNE || c==K_ME || c==K_MSE) if(posx<SIZE_X-2) mx++; //move east
		if(c==K_MNW || c==K_MN || c==K_MNE) if(posy>4) my--; //move north
		if(c==K_MSW || c==K_MS || c==K_MSE) if(posy<SIZE_Y-1) my++; //move south
		
		//uchar oldswrddir=sworddir;

		if(c==K_S) stance=!stance;
		uchar swordtrgt=255;
		if(sworddelay==0) {
			if(c==K_ANW) {sworded=true; swordtrgt=7; }
			else if(c==K_AN) {sworded=true; swordtrgt=0; }
			else if(c==K_ANE) {sworded=true; swordtrgt=1; }
			else if(c==K_AW) {sworded=true; swordtrgt=6; }
			else if(c==K_AE) {sworded=true; swordtrgt=2; }
			else if(c==K_ASW) {sworded=true; swordtrgt=5; }
			else if(c==K_AS) {sworded=true; swordtrgt=4; }
			else if(c==K_ASE) {sworded=true; swordtrgt=3; }
	} else sworddelay--;

		//Avatar////////////////////////////////////////
		//check grabs
		bool grab = false;
		char dx,dy;
		for(uint i=0;i<ZED_MAX;i++) {
			if(grabbed[i]) {
			grab=true;
			dx=posx-zeds[i]->x;
			dy=posy-zeds[i]->y;
			if(dx>0) mx=mx>0?0:mx;
			if(dx<0) mx=mx<0?0:mx;
			if(dy>0) my=my>0?0:my;
			if(dy<0) my=my<0?0:my;
			}
		}
		
		if( mx!=0 || my!=0 ) 
			if ( movedelay==0) {
			posx+=mx;
			posy+=my;
			movedelay=movespeed;
			}
		if ( movedelay>0) movedelay--;
		//draw ava
		if(grab) console->addchar(CH_AVA,COL_MGT,COL_WHT,posx,posy);
		else console->addchar(CH_AVA,COL_BLK,COL_WHT,posx,posy);
		//Sword//////////////////////////////////
		//start swinging
		uchar sx=0,sy=1;
		long ch;
		if (sworded) {
			sworded=false;
			slashstrength=(char)swordtrgt-(char)sworddir;
			if(slashstrength>4) slashstrength-=8;
			if(slashstrength<-4) slashstrength+=8;
			if(stance&&abs(slashstrength)==4) slashstrength*=-1;
			if(slashstrength!=0) {
			sworddelay = swordspeed+abs(slashstrength)*2;
			swordframe = 3+2*abs(slashstrength); }

		}
		//update swing
		if(slashstrength==0) { swordframe=0; }
		if(swordframe>0) {
			swordframe--;
			if ( swordframe%2==0 ) {
				sworddir+=slashstrength>0?1:-1;
				slashstrength+=slashstrength>0?-1:1;
				sworddir=sworddir==255?7:sworddir;
				sworddir=sworddir==8?0:sworddir;
			}
		}
		//pick position and glyph
			switch (sworddir) {
				case 0:
					sx=posx;
					sy=posy-1;
					ch=CH_S0;
					break;
				case 1:
					sx=posx+1;
					sy=posy-1;
					ch=CH_S1;
					break;
				case 2:
					sx=posx+1;
					sy=posy;
					ch=CH_S2;
					break;
				case 3:
					sx=posx+1;
					sy=posy+1;
					ch=CH_S3;
					break;
				case 4:
					sx=posx;
					sy=posy+1;
					ch=CH_S4;
					break;
				case 5:
					sx=posx-1;
					sy=posy+1;
					ch=CH_S5;
					break;
				case 6:
					sx=posx-1;
					sy=posy;
					ch=CH_S6;
					break;
				case 7:
					sx=posx-1;
					sy=posy-1;
					ch=CH_S7;
					break;

				}
		//draw sword
		if(abs(slashstrength)>0) ch=CH_SL2;
		console->addchar(ch,COL_BLK,COL_WHT,sx,sy);

		//Zeds/////////////////////////////////////////
		//Spawn///////
		uchar x,y;
		if(zedcount<ZED_MAX-1 && xorrandom()%150<10) {
			zed* z = new zed;
			//pick position
			if( xorrandom()%100>50 )
				if( xorrandom()%100>50 ) {
					x=0;
					y=2+xorrandom()%(SIZE_Y);
				} else {
					x=SIZE_X-1; 
					y=2+xorrandom()%(SIZE_Y);
				}
			else
				if( xorrandom()%100>50 ) {
					y=2;
					x=xorrandom()%(SIZE_X);
				} else {
					y=SIZE_Y+1;
					x=xorrandom()%(SIZE_X);
				}
			//initialize vars
			z->x = x;
			z->y = y;
			z->attack = 1;
			z->move = 5;
			z->health = zedhealth;
			//printf("Added zed at %i,%i\n",x,y);
			//find free mempos
			uint i=0;
			while (zeds[i]!=NULL) { i++; if(i==ZED_MAX) i=0; }
			zeds[i] = z;
			zedcount++;
		}
		uchar basedamage = 8-abs(slashstrength*2);
		basedamage*=basedamage;
		if((swordframe==0)) basedamage=0;
		uchar swap;
		//long ch;
		for(uint i=0;i<ZED_MAX;i++) {
			if(zeds[i]!=NULL) {
				zed* z = zeds[i];
				//update
				dx = posx-z->x;
				dy = posy-z->y;
				if(z->move==0 && abs(dx)<2&&abs(dy)<2&&z->attack==0 ) {
					//attack
					uchar dam = xorrandom()%5;
						if (!god) HP-=dam;
						if (dam>0) add_eff(posx,posy,0,0,42,2,COL_BLK,COL_RED,false);
						z->move = zedmove+xorrandom()%11;
						z->attack=zedattack;
						//grab
						if(!grabbed[i]&&xorrandom()%11>7) grabbed[i]=true;
					}
			if(z->move==0 && !grabbed[i]) {
				//Move
				if (dx>0) {z->x++; if(xorrandom()%11>6) if(xorrandom()&1) z->y++; else z->y--;}
				if (dx<0) {z->x--; if(xorrandom()%11>6) if(xorrandom()&1) z->y++; else z->y--;}
				if (dy>0) {z->y++; if(xorrandom()%11>6) if(xorrandom()&1) z->x++; else z->x--;}
				if (dy<0) {z->y--; if(xorrandom()%11>6) if(xorrandom()&1) z->x++; else z->x--;}
				z->move = zedmove+xorrandom()%11; } else z->move--;
				if(z->attack>0) z->attack--;
				if(sy==z->y&&sx==z->x) {
					//take damage
					swap = z->health;
					z->health-=basedamage;
					if(swap*2/3>basedamage) basedamage=1;
					else basedamage-=2/3*swap;
						for(uchar i=0;i<((int)swap-(z->health))/8;i++) {
							add_eff(z->x,z->y,xorrandom()%5-2,xorrandom()%5-2,xorrandom()%5>2?CH_BL0:CH_BL1,2+xorrandom()%5,COL_BLK,COL_RED,true);
						}
				}
				//draw zed
				if(grabbed[i]) console->addchar(CH_ZED,COL_MGT,COL_GRN,z->x,z->y); else console->addchar(CH_ZED,COL_BLK,COL_GRN,z->x,z->y);
				if(z->health<0) {
					//delete & update vars
					//printf("Zed killed at %i,%i with %i health\n",z->x,z->y,z->health);

					if(z->health>-20) map->addchar(CH_CRPS,COL_RED,COL_BLK,z->x,z->y-3);
					else {if(xorrandom()%5>2) console->addchar(CH_MNGL0,COL_RED,COL_BLK,z->x,z->y-3);
					else console->addchar(CH_MNGL1,COL_RED,COL_BLK,z->x,z->y-3);
						add_eff(z->x,z->y,xorrandom()%3-1,xorrandom()%3-1,xorrandom()%5>2?CH_MNGL0:CH_MNGL1,2+xorrandom()%4,COL_RED,COL_BLK,true); 
					}

					delete zeds[i];
					zedcount--;
					kills++;
					grabbed[i]=false;
					zeds[i]=NULL;
					}
			}
		}
		//Effects///////////////////////////////////////////////

		for(uint i=0;i<EFF_MAX;i++) {
			if(effs[i]!=NULL) {
			//update vars
				eff* e = effs[i];
				if(e->movephase==0) {
				e->x+=e->vx;
				e->y+=e->vy;
				e->movephase=2;
				} else e->movephase--;
				e->life--;
			
				console->addchar(e->ch,e->bg,e->fg,e->x,e->y);

				if (e->life<0) {
					if(e->stay) {
						//Settle to map
						ch=map->chars[e->x+e->y*map->size_x].ch;
						if(ch==CH_BL0||ch==CH_BL1) ch=CH_SPLT; 
						else if(ch==CH_CRPS||ch==CH_MNGL0||ch==CH_MNGL1) {}
						else if(ch==CH_SPLT) ch=CH_SPLT+1;
						else if(ch==CH_SPLT+1) ch=CH_SPLT+2;
						else if(ch==CH_SPLT+2) ch=CH_SPLT+2;
						else ch=e->ch;
						map->addchar(e->ch,e->bg,e->fg,e->x,e->y-3);
					}
					//delete
					delete effs[i];
					effcount--;
					effs[i]=NULL;
				}
			}
		}


		//UI///////////////////////////////////////////////////

		console->addhline(0,1,SIZE_X,COL_BLK,COL_CYN);
		if(!god) sprintf(tmpstr,"Hp: %i",HP); else sprintf(tmpstr,"Hp: %c",236);
		console->addstr(5,0,tmpstr,COL_BLK,COL_CYN);
		memset(tmpstr,0,SIZE_X);
		sprintf(tmpstr,"Undead: %i",zedcount);
		console->addstr(15,0,tmpstr,COL_BLK,COL_CYN);
		memset(tmpstr,0,SIZE_X);
		sprintf(tmpstr,"Redead: %i",kills);
		console->addstr(35,0,tmpstr,COL_BLK,COL_CYN);
		memset(tmpstr,0,SIZE_X);
		if (!stance) sprintf(tmpstr,"Stance: %s","CW");
		else sprintf(tmpstr,"Stance: %s","CCW");
		console->addstr(55,0,tmpstr,COL_BLK,COL_CYN);
		memset(tmpstr,0,SIZE_X);
		//mvprintw(1,40,"Damage: %i",basedamage);
		//mvprintw(1,53,"c: %c, %i",c,c);


		console->draw(screen);
		SDL_Delay(40);
		
		//nanosleep(40000000);
		c=getkey(0);
		if(c>0) printf("%i\n",c);
		if(HP>0) console->clear();
		//if(c==KEY_P) { pause loop
		} while (c!=SDLK_ESCAPE && HP>0);

		printf("Game over!\n");

		for(uint i=0;i<ZED_MAX;i++){
			if(zeds[i]!=NULL) {
				delete zeds[i];
			}
		}
		for(uint i=0;i<EFF_MAX;i++){
			if(effs[i]!=NULL) {
				delete effs[i];
			}
		}


		if(c!=SDLK_ESCAPE) {

			Uint32 t=(SDL_GetTicks()-startTime)/1000;
			sprintf(tmpstr,"You have perished.");
			console->addstr(20,30,tmpstr,COL_BLK,COL_CYN);
			memset(tmpstr,0,SIZE_X);
			sprintf(tmpstr,"You delayed the inevitable for %i minutes %i seconds.",int(t/60),t-int(t/60)*60);
			console->addstr(15,32,tmpstr,COL_BLK,COL_CYN);
			memset(tmpstr,0,SIZE_X);
			sprintf(tmpstr,"Press R to try again, Esc to quit.");
			console->addstr(17,34,tmpstr,COL_BLK,COL_CYN);
			memset(tmpstr,0,SIZE_X);
			console->draw(screen);
			do {
			c=getkey(-1);
			if(c=='r') cont=true;
			} while (c!=27&&c!='r');
		}
		} while (cont);

	printf("EoP\n");
	SDL_FreeSurface(screen);
	delete console;
	delete map;
	return 0;
}