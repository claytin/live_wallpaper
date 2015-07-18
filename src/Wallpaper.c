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
		{0, 0, 0, 0}	//thanks c/gnu
	};

	char *path = NULL;

	int nextOption;
	while((nextOption = getopt_long(argc, argv, "hp:", longOptions, 0)) != -1){
		switch(nextOption){
			case 'h':
				printUsage(argv[0]);
				return 0;
			case 'p':
				path = optarg;
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

	settings.wallpaper->width = 100;
	settings.wallpaper->height = 100;
	settings.wallpaper->refresh = 60;

	if(loadWallpaper(path, settings.wallpaper)){
		printf("unable to load wallpaper %s\n", path);
		return 1;
	}

	if(start()){
		return 1;
	}

	/*sf::RenderTexture buff;*/

	/*//defaults and stuff*/
	/*Wallpaper wallset = {*/
		/*640 * 2,*/
		/*480,*/
		/*60,*/
		/*&buff*/
	/*};*/

	/*if((*init)(&wallset)){*/
		/*return 1;*/
	/*}*/

	/*//now that all the settings have been resolved. itt is time*/
	/*buff.create(wallset.width, wallset.height);*/

	/*sf::RenderWindow window(sf::VideoMode(wallset.width, wallset.height),*/
		/*"SFML works!");*/

	/*double workspace = 0;*/
	/*while(true){*/
		/*(*redraw)();*/

		/*workspace -= 1;*/
		/*(*signal)(workspace);*/

		/*//draw it to the window*/
		/*sf::Sprite sprite(wallset.renderBuff->getTexture());*/

		/*window.clear();*/
		/*window.draw(sprite);*/
		/*window.display();*/

		/*sf::sleep(sf::milliseconds(1000 * wallset.refresh));*/
	/*}*/

	/*dlclose(wallpaper_program);*/

	return 0;
}

int start(void){
	if (SDL_Init(SDL_INIT_VIDEO) != 0){
		printf("SDL_Init Error: %s\n", SDL_GetError());
		return 1;
	}

	SDL_Window *window = SDL_CreateWindow(
		"wallpaper test window",
		100, 100, 640, 480,
		SDL_WINDOW_SHOWN);

	if (window == NULL){
		printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,
		SDL_RENDERER_ACCELERATED);

	if (renderer == NULL){
		SDL_DestroyWindow(window);
		printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	SDL_Surface *bitmap = SDL_LoadBMP("./test.bmp");
	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, bitmap);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);

	SDL_RenderPresent(renderer);
	SDL_Delay(10000);

	SDL_Quit();

	settings.wallpaper->texture = SDL_CreateTexture(
		renderer,
		SDL_PIXELFORMAT_RGB24,
		SDL_TEXTUREACCESS_STREAMING,
		settings.wallpaper->width,
		settings.wallpaper->height);

	return 0;
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
