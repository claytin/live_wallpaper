#include "Wallpaper.h"
#include <stdio.h>
#include <time.h>
#include <SDL.h>
#include <SDL_image.h>

#define SCALE_FACTOR 2

static Wallpaper *this;

SDL_Texture* treeTexture = NULL;
SDL_Texture* mountainTexture = NULL;
SDL_Texture* islandTexture = NULL;
SDL_Texture** cloudTextures = NULL;

//draw each of the elements in different functions
void drawTrees(void);
void drawMountains(void);
void drawIsland(void);
void drawClouds(void);

int init(Wallpaper* _wallpaper){
	this = _wallpaper;
	/*_wallpaper->refresh = 16;*/
	_wallpaper->refresh = 1000;

	int imgFlags = IMG_INIT_PNG;
	if(!(IMG_Init(imgFlags) & imgFlags)){
		printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
		return 1;
	}

	treeTexture = SDL_CreateTextureFromSurface(
		this->renderer, IMG_Load("trees.png"));

	mountainTexture = SDL_CreateTextureFromSurface(
		this->renderer, IMG_Load("mountains.png"));

	islandTexture = SDL_CreateTextureFromSurface(
		this->renderer, IMG_Load("island.png"));

	cloudTextures = malloc(sizeof(SDL_Surface) * 3);
	cloudTextures[0] = SDL_CreateTextureFromSurface(
		this->renderer, IMG_Load("cloud0.png"));
	cloudTextures[1] = SDL_CreateTextureFromSurface(
		this->renderer, IMG_Load("cloud1.png"));
	cloudTextures[2] = SDL_CreateTextureFromSurface(
		this->renderer, IMG_Load("cloud2.png"));

	SDL_SetRenderDrawColor(this->renderer, 16, 65, 132, 0);

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
	/*SDL_FillRect(this->surface, NULL, 0x104184);*/
	drawMountains();
	drawTrees();
	drawClouds();
	drawIsland();
	return 0;
}

void drawClouds(void){
	int width, height;
	for(int i = 0; i < 8; i++){
		int cloudOffset = time(NULL);
		/*SDL_GetClipRect(cloudTextures[i % 3], &cloudRect);*/
		SDL_QueryTexture(cloudTextures[i % 3], NULL, NULL, &width, &height);
		width *= SCALE_FACTOR;
		height *= SCALE_FACTOR;

		SDL_Rect destRect = {
			(i * (this->width / 5) + cloudOffset)
				% (this->width + width)
				- width,
			(i * (this->height / 50))
				% (int)(this->height * .25)
				+ (this->height - (this->height * .5)),
			width, height
		};

		SDL_RenderCopy(this->renderer, cloudTextures[i % 3], NULL, &destRect);
	}
}

void drawIsland(void){
	int width, height;
	SDL_QueryTexture(islandTexture, NULL, NULL, &width, &height);
	width *= SCALE_FACTOR;
	height *= SCALE_FACTOR;

	SDL_Rect destRect = {
		((this->width - width) * 0.6)
			* ((sin(time(NULL) / (float)240) / 2) + 0.5)
			+ ((this->width - width) * 0.15),
		((this->height  - height) * 0.2)
			* ((sin(time(NULL) / (float)200) / 2) + 0.5)
			+ ((this->height - height) * 0.5),
		width, height};

	SDL_RenderCopy(this->renderer, islandTexture, NULL, &destRect);
}

void drawMountains(void){

	int width, height;
	SDL_QueryTexture(mountainTexture, NULL, NULL, &width, &height);
	width *= SCALE_FACTOR;
	height *= SCALE_FACTOR;

	int mountainOffset = (time(NULL) / 10) % width;

	SDL_Rect destRect = {
		-mountainOffset,
		this->height - height,
		width,
		height};
	int i = 0;
	while(destRect.x < this->width){
		i++;
		SDL_Rect tempRect = destRect;
		SDL_RenderCopy(this->renderer, mountainTexture, NULL, &tempRect);
		destRect.x += width;
	}
}

void drawTrees(void){

	int width, height;
	SDL_QueryTexture(mountainTexture, NULL, NULL, &width, &height);
	width *= SCALE_FACTOR;
	/*height *= SCALE_FACTOR;*/

	int treeOffset = (time(NULL) / 5) % width;

	SDL_Rect destRect = {
		-treeOffset,
		this->height - height,
		width,
		height};
	int i = 0;
	while(destRect.x < this->width){
		i++;
		SDL_RenderCopy(this->renderer, treeTexture, NULL, &destRect);
		destRect.x += width;
	}
}
