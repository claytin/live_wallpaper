#include <getopt.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL.h>

#include "Wallpaper.h"

#include "X11_root_output.h"
#include "Bmp_output.h"
#include "Window_output.h"

int loadWallpaper(const char *path, Wallpaper *wallpaper);
void printUsage(const char *command);
int start(Wallpaper*);

static bool once = false;

int main(int argc, char **argv){

	const struct option longOptions[] = {
		{"help",		no_argument,		0,	'h'},
		{"path",		required_argument,	0,	'p'},
		{"width",		required_argument,	0,	'x'},
		{"height",		required_argument,	0,	'y'},
		{"window",		no_argument,		0,	'w'},
		{"background",	no_argument,		0,	'b'},
		{"bmp",			required_argument,	0,	'm'},
		{"once",		no_argument,		0,	'n'},
		{0, 0, 0, 0}
	};

	Wallpaper *wallpaper = (Wallpaper*)malloc(sizeof(Wallpaper));

	wallpaper->path = NULL;
	wallpaper->output.name = NONE;
	wallpaper->width = -1;
	wallpaper->height = -1;
	wallpaper->refresh = -1;

	char *output_file_path = NULL;

	int nextOption;
	while((nextOption = getopt_long(argc, argv, "hp:x:y:wbm:", longOptions, 0)) != -1){
		switch(nextOption){
			case 'n':
				once = true;
				break;
			case 'h':
				printUsage(argv[0]);
				return 0;
			case 'p':
				wallpaper->path = optarg;
				break;
			case 'x':
				wallpaper->width = atoi(optarg);
				break;
			case 'y':
				wallpaper->height = atoi(optarg);
				break;
			case 'w':
				wallpaper->output.name = WINDOW;
				break;
			case 'b':
				wallpaper->output.name = BACKGROUND;
				break;
			case 'm':
				wallpaper->output.name = BMP;
				output_file_path = optarg;
				break;
			default:
				printUsage(argv[0]);
				return 1;
		}
	}

	if(wallpaper->path == NULL){
		printf("you must at least specify a wallpaper path\n");
		printUsage(argv[0]);
		return 1;
	}

	wallpaper->width =
		(wallpaper->width == -1) ?
			DEFAULT_WIDTH : wallpaper->width;

	wallpaper->height =
		(wallpaper->height == -1) ?
			DEFAULT_HEIGHT : wallpaper->height;

	wallpaper->refresh =
		(wallpaper->refresh == -1) ?
		DEFAULT_REFRESH_RATE : wallpaper->refresh;

	if(loadWallpaper(wallpaper->path, wallpaper)){
		printf("unable to load wallpaper %s\n", wallpaper->path);
		return 1;
	}

	switch(wallpaper->output.name){
		case BACKGROUND:
			x11_root_output_init(wallpaper);
			break;
		case BMP:
			bmp_output_init(wallpaper, output_file_path);
			break;
		case WINDOW:
			window_output_init(wallpaper);
			break;
		default:
			printf("you must specify an output\n");
			printUsage(argv[0]);
			return 1;
	}

	return start(wallpaper);
}

int start(Wallpaper *_wallpaper){

	if((*_wallpaper->init)(_wallpaper)){
		return 1;
	}

	SDL_Event event;
	while(1){
		SDL_RenderClear(_wallpaper->renderer);
		(*_wallpaper->redraw)();
		SDL_RenderPresent(_wallpaper->renderer);

		(_wallpaper->output.update)();

		if (once) { 
			SDL_Quit();
			return 0;
		}

		while(SDL_PollEvent(&event) != 0){
			if(event.type == SDL_QUIT){
				SDL_Quit();
				return 0;
			}
		}

		SDL_Delay((Uint32)_wallpaper->refresh);
	}
}

int loadWallpaper(const char *_path, Wallpaper *_wallpaper){
	_wallpaper->program = dlopen(_path, RTLD_NOW);

	char * error;
	if((error = dlerror()) != 0){
		printf("error: %s\n", error);
		return 2;
	}

	_wallpaper->redraw = (int (*)())dlsym(_wallpaper->program, "redraw");
	_wallpaper->init = (int (*)(Wallpaper*))dlsym(_wallpaper->program, "init");
	_wallpaper->destroy = (int (*)(void))dlsym(_wallpaper->program, "destroy");

	if((error = dlerror()) != 0){
		printf("error: %s\n", error);
		return 2;
	}

	return 0;
}

void printUsage(const char *_command){
	printf("usage: %s [options] -p wallpaper\n", _command);
	printf(
		"-h --help            show this help message\n"
		"-p --path   <path>   path to the wallaper (required)\n"
		"-x --width  <width>  width of output image\n"
		"-y --height <height> height of output image\n"
		"-w --window          output to a window\n"
		"-b --background      output to X11 root window (desktop background)\n"
		"-m --bmp    <path>   output to a bitmap file\n"
	);
}
