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
	unsigned short output;
	char *path;
	char *output_file;
	unsigned int width;
	unsigned int height;
} settings;

int main(int argc, char **argv){

	const struct option longOptions[] = {
		{"help",		no_argument,		0,	'h'},
		{"path",		required_argument,	0,	'p'},
		{"width",		required_argument,	0,	'x'},
		{"height",		required_argument,	0,	'y'},
		{"window",		no_argument,		0,	'w'},
		{"background",	no_argument,		0,	'b'},
		{"file",		required_argument,	0,	'f'},
		{0, 0, 0, 0}	//thanks c/gnu
	};

	settings.path = NULL;
	settings.output_file = NULL;
	settings.output = NONE;
	settings.wallpaper = NULL;

	int nextOption;
	while((nextOption = getopt_long(argc, argv, "whp:x:y:", longOptions, 0)) != -1){
		switch(nextOption){
			case 'h':
				printUsage(argv[0]);
				return 0;
			case 'p':
				settings.path = optarg;
				break;
			case 'x':
				settings.width = (unsigned int)atoi(optarg);
				break;
			case 'y':
				settings.height = (unsigned int)atoi(optarg);
				break;
			case 'w':
				settings.output = WINDOW;
				break;
			case 'b':
				settings.output = BACKGROUND;
				break;
			case 'f':
				settings.output = IMAGE;
				settings.output_file = optarg;
				break;
			default:
				printUsage(argv[0]);
				return 1;
		}
	}

	if(settings.path == NULL){
		printf("you must at least specify a wallpaper path\n");
		printUsage(argv[0]);
		return 1;
	}

	settings.wallpaper = (Wallpaper*)malloc(sizeof(Wallpaper));

	settings.wallpaper->width = (settings.width) ?  settings.width : DEFAULT_WIDTH;
	settings.wallpaper->height = (settings.height) ?  settings.height : DEFAULT_HEIGHT;

	settings.wallpaper->refresh = DEFAULT_REFRESH_RATE;

	if(loadWallpaper(settings.path, settings.wallpaper)){
		printf("unable to load wallpaper %s\n", settings.path);
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
		(int)settings.wallpaper->width,
		(int)settings.wallpaper->height,
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
		return 2;
	}

	wallpaper->redraw = (int (*)())dlsym(wallpaper_program, "redraw");
	wallpaper->init = (int (*)(Wallpaper*))dlsym(wallpaper_program, "init");
	wallpaper->destroy = (int (*)(void))dlsym(wallpaper_program, "destroy");

	if((error = dlerror()) != 0){
		printf("error: %s\n", error);
		return 2;
	}

	return 0;
}

void printUsage(const char *command){
	printf("usage: %s [options] -p wallpaper\n", command);
	printf(
		"-h --help          show this help message\n"
		"-p --path   path   path to the wallaper (required)\n"
		"-x --width  width  width of output image\n"
		"-y --height height height of output image\n"
		"-w --window        output to a window\n"
		"-b --background    output to X11 root window (desktop background)\n"
		"-f --file   path   output to a file\n"
	);
}
