#include <getopt.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

#include "Wallpaper.h"

int loadWallpaper(const char *path, Wallpaper *wallpaper);
void printUsage(const char *command);
int start(void);

static struct {
	Wallpaper *wallpaper;
}settings;

int main(int argc, char **argv){

	const struct option longOptions[] = {
		{"help",	no_argument,		0,	'h'},
		{"path",	required_argument,	0,	'p'},
		{"width",	required_argument,	0,	'x'},
		{"height",	required_argument,	0,	'y'},
		{"window",	no_argument,		0,	'w'},
		{"root",	no_argument,		0,	'r'},
		{"file",	required_argument,	0,	'f'},
		{0, 0, 0, 0}	//thanks c/gnu
	};

	char *path = NULL;
	int width = 0, height = 0, output = 0;
	char *outfile = NULL;

	int nextOption;
	while((nextOption = getopt_long(argc, argv, "whp:x:y:", longOptions, 0)) != -1){
		switch(nextOption){
			case 'h':
				printUsage(argv[0]);
				return 0;
			case 'p':
				path = optarg;
				break;
			case 'x':
				width = atoi(optarg);
				break;
			case 'y':
				height = atoi(optarg);
				break;
			case 'w':
				output = 0;
				break;
			case 'r':
				output = 1;
				break;
			case 'f':
				output = 2;
				outfile = optarg;
				break;
			default:
				printUsage(argv[0]);
				return 1;
		}
	}

	if(path == NULL){
		printf("you must include a wallpaper\n");
		printUsage(argv[0]);
		return 1;
	}

	settings.wallpaper = (Wallpaper*)malloc(sizeof(Wallpaper));

	settings.wallpaper->width = (width) ? width : 640;
	settings.wallpaper->height = (height) ? height : 480;

	settings.wallpaper->refresh = 1000;

	if(loadWallpaper(path, settings.wallpaper)){
		printf("unable to load wallpaper %s\n", path);
		return 1;
	}

	if(start()){
		return 1;
	}

	return 0;
}

int start(void){
	if (SDL_Init(SDL_INIT_VIDEO) != 0){
		printf("SDL_Init Error: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Window *window = SDL_CreateWindow(
		"wallpaper test window",
		100,
		100,
		settings.wallpaper->width,
		settings.wallpaper->height,
		SDL_WINDOW_SHOWN);

	if (window == NULL){
		printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(
		window,
		-1,
		SDL_RENDERER_ACCELERATED);

	if (renderer == NULL){
		SDL_DestroyWindow(window);
		printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	/*settings.wallpaper->texture = SDL_CreateTexture(*/
		/*renderer,*/
		/*SDL_PIXELFORMAT_RGBA8888,*/
		/*SDL_TEXTUREACCESS_TARGET,*/
		/*settings.wallpaper->width,*/
		/*settings.wallpaper->height);*/

	/*SDL_SetRenderTarget(*/
		/*renderer,*/
		/*settings.wallpaper->texture);*/

	settings.wallpaper->renderer = renderer;

	if((*settings.wallpaper->init)(settings.wallpaper)){
		return 1;
	}

	SDL_Event event;
	while(1){

		SDL_RenderClear(renderer);
		(*settings.wallpaper->redraw)();
		/*SDL_RenderCopy(*/
			/*settings.wallpaper->renderer,*/
			/*settings.wallpaper->texture, NULL, NULL);*/
		SDL_RenderPresent(renderer);

		while(SDL_PollEvent(&event) != 0){
			if(event.type == SDL_QUIT){
				SDL_Quit();
				return 0;
			}
		}

		SDL_Delay(settings.wallpaper->refresh);
	}
}

int loadWallpaper(const char *path, Wallpaper *wallpaper){
	void * wallpaper_program = dlopen(path, RTLD_NOW);

	char * error;
	if((error = dlerror()) != 0){
		printf("error: %s\n", error);
		return 1;
	}

	wallpaper->redraw = (int (*)())dlsym(wallpaper_program, "redraw");
	wallpaper->init = (int (*)(Wallpaper*))dlsym(wallpaper_program, "init");
	wallpaper->destroy = (int (*)(void))dlsym(wallpaper_program, "destroy");
	wallpaper->signal = (int (*)(int, char*))dlsym(wallpaper_program, "signal");

	if((error = dlerror()) != 0){
		printf("error: %s\n", error);
		return 1;
	}

	return 0;
}

void printUsage(const char *command){
	printf("usage: %s\n", command);
}
