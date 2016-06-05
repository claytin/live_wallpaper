#ifndef WALLPAPER_H
#define WALLPAPER_H
#include <SDL.h>

struct Wallpaper{
	unsigned int width, height;	//wallpaper size
	unsigned int refresh;	//time between redraws in milliseconds
	SDL_Renderer *renderer;

	void *program;
	int (*redraw)();
	int (*init)(struct Wallpaper*);
	int (*destroy)(void);
};

enum outputs{
	NONE = 0,
	WINDOW,
	BACKGROUND,
	IMAGE
};

typedef struct Wallpaper Wallpaper;

static const unsigned int DEFAULT_WIDTH = 640;
static const unsigned int DEFAULT_HEIGHT = 480;

//by default refresh every second
static const unsigned int DEFAULT_REFRESH_RATE = 1000;

#endif
