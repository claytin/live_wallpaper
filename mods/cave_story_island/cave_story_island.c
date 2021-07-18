#include "cave_story_island.h"
#include "Wallpaper.h"
#include <stdio.h>
#include <time.h>
#include <SDL.h>

#define SCALE_FACTOR 2

static Wallpaper *wallpaper;

struct Sprite{
	SDL_Texture *texture;
	SDL_Rect destRect;
	int (*render)(struct Sprite*, time_t time);
	int nth;
};
typedef struct Sprite Sprite;

static Sprite *sprites;
int num_sprites = 6;

//draw each of the elements in different functions
int drawTrees(Sprite*, time_t);
int drawMountains(Sprite*, time_t);
int drawContinuous(Sprite*, time_t, int speed);

int drawIsland(Sprite*, time_t);
int drawClouds(Sprite*, time_t);

int redraw(void);
int signal(int cmd, char* data);
int destory(void);
int init(Wallpaper* _wallpaper);

int init(Wallpaper* _wallpaper){
	wallpaper = _wallpaper;

	sprites = malloc(sizeof(Sprite) * (unsigned long)num_sprites);

	sprites[0].texture = SDL_CreateTextureFromSurface(
		wallpaper->renderer,
		SDL_LoadBMP_RW(SDL_RWFromMem(res_cloud0_bmp, (int)res_cloud0_bmp_len), 1));
	sprites[0].render = drawClouds;
	sprites[0].nth = 0;

	sprites[1].texture = SDL_CreateTextureFromSurface(
		wallpaper->renderer,
		SDL_LoadBMP_RW(SDL_RWFromMem(res_cloud1_bmp, (int)res_cloud1_bmp_len), 1));
	sprites[1].render = drawClouds;
	sprites[1].nth = 1;

	sprites[2].texture = SDL_CreateTextureFromSurface(
		wallpaper->renderer,
		SDL_LoadBMP_RW(SDL_RWFromMem(res_cloud2_bmp, (int)res_cloud2_bmp_len), 1));
	sprites[2].render = drawClouds;
	sprites[2].nth = 2;

	sprites[3].texture = SDL_CreateTextureFromSurface(
		wallpaper->renderer,
		SDL_LoadBMP_RW(SDL_RWFromMem(res_island_bmp, (int)res_island_bmp_len), 1));
	sprites[3].render = drawIsland;

	sprites[4].texture = SDL_CreateTextureFromSurface(
		wallpaper->renderer,
		SDL_LoadBMP_RW(SDL_RWFromMem(res_mountains_bmp, (int)res_mountains_bmp_len), 1));
	sprites[4].render = drawMountains;

	sprites[5].texture = SDL_CreateTextureFromSurface(
		wallpaper->renderer,
		SDL_LoadBMP_RW(SDL_RWFromMem(res_trees_bmp, (int)res_trees_bmp_len), 1));
	sprites[5].render = drawTrees;

	for(int i = 0; i < num_sprites; i++){
		SDL_QueryTexture(
			sprites[i].texture,
			NULL, NULL,
			&sprites[i].destRect.w,
			&sprites[i].destRect.h);

		sprites[i].destRect.w *= SCALE_FACTOR;
		sprites[i].destRect.h *= SCALE_FACTOR;
	}

	SDL_SetRenderDrawColor(wallpaper->renderer, 16, 65, 132, 0);

	return 0;
}

int destroy(void){
	return 0;
}

int signal(int cmd, char* data){
	return 0;
}

int redraw(void){
	for(int i = 0; i < num_sprites; i++){
		if(sprites[i].render(&sprites[i], time(NULL))){
			SDL_RenderCopy(
				wallpaper->renderer,
				sprites[i].texture,
				NULL,
				&sprites[i].destRect);
		}
	}

	return 0;
}

int drawClouds(Sprite *sprite, time_t time){
	int pan = (2 * time) / ((sprite->nth % 3) + 1);
	int wrapX = (wallpaper->width + sprite->destRect.w);

	sprite->destRect.x =
		(((sprite->nth * (wallpaper->width / 3)) + pan)
		% wrapX)
		- sprite->destRect.w;

	sprite->destRect.y =
		(int)(sprite->nth * (wallpaper->width / 5))
		% (int)(wallpaper->height / 3)
		+ (wallpaper->height / 3);

	return 1;
}

int drawIsland(Sprite *sprite, time_t time){
	sprite->destRect.x =
		(int)(sin(time / 1000.0) * (sprite->destRect.w))
		+ (wallpaper->width / 4);

	sprite->destRect.y =
		(int)(cos(time / 250.0) * sprite->destRect.h)
		+ (wallpaper->height / 3);

	return 1;
}

int drawTrees(Sprite *sprite, time_t time){
	return drawContinuous(sprite, time, 10);
}
int drawMountains(Sprite *sprite, time_t time){
	return drawContinuous(sprite, time, 15);
}

int drawContinuous(Sprite *sprite, time_t time, int speed){
	sprite->destRect.x = -(time / speed) % sprite->destRect.w;
	sprite->destRect.y = wallpaper->height - sprite->destRect.h;

	do{
		SDL_RenderCopy(
			wallpaper->renderer,
			sprite->texture,
			NULL,
			&sprite->destRect);

	}while((sprite->destRect.x += sprite->destRect.w) < (int)wallpaper->width);

	return 0;
}
