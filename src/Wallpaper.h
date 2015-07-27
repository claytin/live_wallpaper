#ifndef WALLPAPER_H
#define WALLPAPER_H
#include <SDL.h>

struct Wallpaper{
	int width, height;	//wallpaper size
	unsigned int refresh;	//time between redraws in milliseconds
	//SDL_Texture *texture;
	SDL_Renderer *renderer;

	void *program;
	int (*redraw)();
	int (*init)(struct Wallpaper*);
	int (*destroy)(void);
	int (*signal)(int, char*);
};

enum signals{
	PAN
};

typedef struct Wallpaper Wallpaper;

#endif
