#ifndef WALLPAPER_H
#define WALLPAPER_H
#include <SDL.h>

struct Wallpaper{
	int width, height;	//wallpaper size
	double refresh;	//time between redraws in seconds
	SDL_Texture *texture;

	void *program;
	int (*redraw)();
	int (*init)(struct Wallpaper*);
	int (*signal)(int, char*);
};

typedef struct Wallpaper Wallpaper;

#endif
