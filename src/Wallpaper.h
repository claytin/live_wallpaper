#ifndef WALLPAPER_H
#define WALLPAPER_H

#include <SFML/Graphics.hpp>

struct Wallpaper{
	int width, height;	//wallpaper size
	double refresh;		//time between redraws in seconds
	sf::RenderTexture * renderBuff;
};

#endif
