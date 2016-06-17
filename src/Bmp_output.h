#ifndef BMP_OUTPUT_H
#define BMP_OUTPUT_H

#include <SDL.h>
#include "Wallpaper.h"

int bmp_output_init(Wallpaper *_wallpaper, char *output_file_path);
int bmp_output_update(void);
int bmp_output_destroy(void);


#endif
