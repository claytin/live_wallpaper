#ifndef WALLPAPER_H
#define WALLPAPER_H

#include <SDL.h>

enum output_names{
	NONE = 0,
	WINDOW,
	BACKGROUND,
	BMP
};

struct Output{
	unsigned short name;
	int (*update)(void);
	int (*destroy)(void);
};

struct Wallpaper{
	char *path;	//path to the shared object file
	int width, height;	//wallpaper size
	int refresh;	//time between redraws in milliseconds
	SDL_Renderer *renderer;

	void *program;
	int (*redraw)();
	int (*init)(struct Wallpaper*);
	int (*destroy)(void);

	struct Output output;
};

typedef struct Wallpaper Wallpaper;

static const unsigned int DEFAULT_WIDTH = 640;
static const unsigned int DEFAULT_HEIGHT = 480;

//by default refresh every second
static const unsigned int DEFAULT_REFRESH_RATE = 1000;

#endif
