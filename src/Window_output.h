#ifndef WINDOW_OUTPUT_H
#define WINDOW_OUTPUT_H

#include <SDL.h>
#include "Wallpaper.h"

int window_output_init(Wallpaper *_wallpaper);
int window_output_update(void);
int window_output_destroy(void);

#endif
