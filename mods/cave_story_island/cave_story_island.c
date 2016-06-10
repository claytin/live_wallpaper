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
	void (*position)(struct Sprite*, time_t time);
};
typedef struct Sprite Sprite;

Sprite sprites[6];

//draw each of the elements in different functions
void drawContinuous(Sprite*, time_t time);
void drawIsland(Sprite*, time_t time);
void drawClouds(Sprite*, time_t time);

int init(Wallpaper* _wallpaper){
	wallpaper = _wallpaper;
	_wallpaper->refresh = 10;

	sprites[0].texture = SDL_CreateTextureFromSurface(
		wallpaper->renderer,
		SDL_LoadBMP_RW(SDL_RWFromMem(res_cloud0_bmp, res_cloud0_bmp_len), 1));
	sprites[0].position = drawClouds;

	sprites[1].texture = SDL_CreateTextureFromSurface(
		wallpaper->renderer,
		SDL_LoadBMP_RW(SDL_RWFromMem(res_cloud1_bmp, res_cloud1_bmp_len), 1));
	sprites[1].position = drawClouds;

	sprites[2].texture = SDL_CreateTextureFromSurface(
		wallpaper->renderer,
		SDL_LoadBMP_RW(SDL_RWFromMem(res_cloud2_bmp, res_cloud2_bmp_len), 1));
	sprites[2].position = drawClouds;

	sprites[3].texture = SDL_CreateTextureFromSurface(
		wallpaper->renderer,
		SDL_LoadBMP_RW(SDL_RWFromMem(res_island_bmp, res_island_bmp_len), 1));
	sprites[3].position = drawIsland;

	sprites[4].texture = SDL_CreateTextureFromSurface(
		wallpaper->renderer,
		SDL_LoadBMP_RW(SDL_RWFromMem(res_mountains_bmp, res_mountains_bmp_len), 1));
	sprites[4].position = drawContinuous;

	sprites[5].texture = SDL_CreateTextureFromSurface(
		wallpaper->renderer,
		SDL_LoadBMP_RW(SDL_RWFromMem(res_trees_bmp, res_trees_bmp_len), 1));
	sprites[5].position = drawContinuous;

	for(int i = 0; i < 6; i++){
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

int run = 0;

int redraw(void){
	run++;
	for(unsigned int i = 0; i < 6; i++){
		sprites[i].position(&sprites[i], run);
		SDL_RenderCopy(
			wallpaper->renderer,
			sprites[i].texture,
			NULL,
			&sprites[i].destRect);
	}

	return 0;
}

void drawClouds(Sprite *sprite, time_t time){
	sprite->destRect.x =
		(0 * (wallpaper->width / 5) + time)
		% (wallpaper->width + sprite->destRect.w)
		- sprite->destRect.w;
	sprite->destRect.y =
		(0 * (wallpaper->height / 50))
		% (int)(wallpaper->height * .25)
		+ (wallpaper->height - (wallpaper->height * .5));
}

void drawIsland(Sprite *sprite, time_t time){
		/*((float)(wallpaper->width - sprite->destRect.w) * 0.6)*/
			/** (float)((sin(time) / 2.0f) + 0.5);*/
			/*+ ((float)(wallpaper->width - sprite->destRect.w) * 0.15)*/

		/*((wallpaper->height  - sprite->destRect.h) * 0.2)*/
			/** ((sin(time) / 2) + 0.5)*/
			/*+ ((wallpaper->height - sprite->destRect.h) * 0.5);*/

	sprite->destRect.x = (int)(sin(time / 10.0f) * (sprite->destRect.w)) + sprite->destRect.w;
	sprite->destRect.y = (int)(cos(time / 10.0f) * (sprite->destRect.h)) + sprite->destRect.h;
}

void drawContinuous(Sprite *sprite, time_t time){
	sprite->destRect.x = -(time / 10) % sprite->destRect.w;
	sprite->destRect.y = wallpaper->height - sprite->destRect.h;
}
