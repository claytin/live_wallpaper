#include "Wallpaper.h"
#include <stdio.h>
#include <time.h>
#include <SDL.h>

#define SCALE_FACTOR 2

Wallpaper *this;

SDL_Surface* treeTexture = NULL;
SDL_Surface* mountainTexture = NULL;
SDL_Surface* islandTexture = NULL;
SDL_Surface** cloudTextures = NULL;

//sdl helper function
SDL_Surface *scaleSurface(SDL_Surface *_sourceSurface, int _scale){
	SDL_Rect sourceRect;
	SDL_GetClipRect(_sourceSurface, &sourceRect);

	SDL_Surface *scaledSurface = SDL_CreateRGBSurface(
		0,
		sourceRect.w * _scale,
		sourceRect.h * _scale,
		32, 0, 0, 0, 0);

	SDL_BlitScaled(_sourceSurface, NULL, scaledSurface, NULL);

	return scaledSurface;
}

//draw each of the elements in different functions
void drawTrees(void);
void drawMountains(void);
void drawIsland(void);
void drawClouds(void);

int init(Wallpaper* _wallpaper){
	this = _wallpaper;
	_wallpaper->refresh = 16;
	/*_wallpaper->refresh = 1000;*/

	treeTexture = scaleSurface(SDL_LoadBMP("trees.bmp"), SCALE_FACTOR);
	SDL_SetColorKey(treeTexture, SDL_TRUE, 0x000000);

	mountainTexture = scaleSurface(SDL_LoadBMP("mountains.bmp"), SCALE_FACTOR);
	SDL_SetColorKey(mountainTexture, SDL_TRUE, 0x000000);

	islandTexture = scaleSurface(SDL_LoadBMP("island.bmp"), SCALE_FACTOR);
	SDL_SetColorKey(islandTexture, SDL_TRUE, 0x000000);

	cloudTextures = malloc(sizeof(SDL_Surface) * 3);
	cloudTextures[0] = scaleSurface(SDL_LoadBMP("cloud0.bmp"), SCALE_FACTOR);
	SDL_SetColorKey(cloudTextures[0], SDL_TRUE, 0x000000);
	cloudTextures[1] = scaleSurface(SDL_LoadBMP("cloud1.bmp"), SCALE_FACTOR);
	SDL_SetColorKey(cloudTextures[1], SDL_TRUE, 0x000000);
	cloudTextures[2] = scaleSurface(SDL_LoadBMP("cloud2.bmp"), SCALE_FACTOR);
	SDL_SetColorKey(cloudTextures[2], SDL_TRUE, 0x000000);

	return 0;
}

int destroy(void){
	return 0;
}

int signal(int cmd, char* data){
	return 0;
}

int draws = 0;

int redraw(void){
	draws++;
	SDL_FillRect(this->surface, NULL, 0x104184);
	drawMountains();
	drawTrees();
	drawIsland();
	drawClouds();
	return 0;
}

int ltime(void *i){
	return draws;
	/*return time(i);*/
}

void drawClouds(void){
	SDL_Rect cloudRect;
	for(int i = 0; i < 10; i++){
		int cloudOffset = ltime(NULL);
		SDL_GetClipRect(cloudTextures[i % 3], &cloudRect);

		SDL_Rect destRect = {
			(i * (this->width / 5) + cloudOffset)
				% (this->width + cloudRect.w)
				- cloudRect.w,
			(i * (this->height / 5))
				% (int)(this->height * .5),
			0, 0
		};

		SDL_BlitSurface(cloudTextures[i % 3], NULL, this->surface, &destRect);
	}
}

void drawIsland(void){
	SDL_Rect islandRect;
	SDL_GetClipRect(islandTexture, &islandRect);

	SDL_Rect destRect = {
		((this->width - islandRect.w) * 0.6)
			* ((sin(ltime(NULL) / (float)240) / 2) + 0.5)
			+ ((this->width - islandRect.w) * 0.15),
		((this->height  - islandRect.h) * 0.6)
			* ((sin(ltime(NULL) / (float)200) / 2) + 0.5)
			+ ((this->height - islandRect.h) * 0.15)
			+ (sin(ltime(NULL) / (float)20) * 20),
		0, 0};

	SDL_BlitSurface(islandTexture, NULL, this->surface, &destRect);
}

void drawMountains(void){

	SDL_Rect mountainRect;
	SDL_GetClipRect(mountainTexture, &mountainRect);

	int mountainOffset = (ltime(NULL) / 10) % mountainRect.w;

	SDL_Rect destRect = {-mountainOffset, this->height - mountainRect.h, 0, 0};
	int i = 0;
	while(destRect.x < this->width){
		i++;
		SDL_Rect tempRect = destRect;
		SDL_BlitSurface(mountainTexture, NULL, this->surface, &tempRect);
		destRect.x += mountainRect.w;
	}
}

void drawTrees(void){

	SDL_Rect treeRect;
	SDL_GetClipRect(treeTexture, &treeRect);

	int treeOffset = (ltime(NULL) / 5) % treeRect.w;

	SDL_Rect destRect = {-treeOffset, this->height - treeRect.h, 0, 0};
	int i = 0;
	while(destRect.x < this->width){
		i++;
		SDL_Rect tempRect = destRect;
		SDL_BlitSurface(treeTexture, NULL, this->surface, &tempRect);
		destRect.x += treeRect.w;
	}
}
