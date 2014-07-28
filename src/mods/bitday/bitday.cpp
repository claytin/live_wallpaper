#include <iostream>
#include <time.h>
#include <math.h>

#include "../../Wallpaper.h"
#include "images.h"
//#include "Wallpaper.h"

Wallpaper * wallp;
int sunRadius;

extern "C" int init(Wallpaper * set){
	std::cout << "inited" << std::endl;

	set->refresh = 1;	//once every second
	wallp = set;
	sunRadius = 25;

	return 0;
}

extern "C" int redraw(void){
	std::cout << "redrawing" << std::endl;

	sf::CircleShape shape(sunRadius);
	shape.setFillColor(sf::Color::White);

	long curtime = time(NULL);
	struct tm *tm_struct = localtime(&curtime);
	long dayInSec = (tm_struct->tm_hour * 60 * 60)
				+ (tm_struct->tm_min * 60)
				+ tm_struct->tm_sec;
	const long dayTotalSec = 24 * 60 * 60;
	double DayProgress = ((float)dayInSec / (float)dayTotalSec);

	float xpos = DayProgress * (wallp->width + (sunRadius * 2)) - (sunRadius * 2);
	float ypos = sin(DayProgress * 3) * (wallp->height - (sunRadius * 2.5));
	std::cout << "percent: " << DayProgress << "\tx/y: " << xpos << "/" << ypos << std::endl;

	shape.setPosition(xpos, ypos);
	wallp->renderBuff->clear();
	wallp->renderBuff->draw(shape);
	
	sf::Texture image;
	image.update(pixels);
	
	sf::Sprite sprite;
	sprite.setTexture(image);
	wallp->renderBuff->draw(sprite);

	return 0;
}
