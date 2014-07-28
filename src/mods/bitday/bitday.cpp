#include <iostream>
#include "../../Wallpaper.h"
//#include "Wallpaper.h"

Wallpaper * wallp;

extern "C" int init(Wallpaper * set){
	std::cout << "inited" << std::endl;

	set->refresh = 1;	//once every second
	wallp = set;

	return 0;
}

extern "C" int redraw(void){
	std::cout << "redrawing" << std::endl;

	sf::CircleShape shape(100.f);
	shape.setFillColor(sf::Color::Green);
	wallp->renderBuff->draw(shape);

	return 0;
}
