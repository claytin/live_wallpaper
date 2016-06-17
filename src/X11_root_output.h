#ifndef X11_ROOT_OUTPUT_H
#define X11_ROOT_OUTPUT_H

#include <SDL.h>
#include "Wallpaper.h"

int x11_root_output_init(Wallpaper*);
int x11_root_output_update(void);
int x11_root_output_destroy(void);


#endif
