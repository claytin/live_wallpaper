#include "Bmp_output.h"

static SDL_Surface *output_surface = NULL;
static char *output_file_path = NULL;

int bmp_output_init(Wallpaper *_wallpaper, char *_output_file_path){
	output_file_path = _output_file_path;

	output_surface = SDL_CreateRGBSurface(
		0,
		_wallpaper->width,
		_wallpaper->height,
		8 * 4,
		0, 0, 0, 0);

	_wallpaper->renderer = SDL_CreateSoftwareRenderer(output_surface);
	_wallpaper->output.update = bmp_output_update;
	_wallpaper->output.destroy = bmp_output_destroy;

	return 0;
}

int bmp_output_update(void){
	SDL_SaveBMP(output_surface, output_file_path);

	return 0;
}

int bmp_output_destroy(void){
	return 0;
}
