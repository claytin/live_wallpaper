#include <iostream>
#include <time.h>
#include <math.h>
#include <stdlib.h>

#include "../../Wallpaper.h"
#include "images.h"
//#include "Wallpaper.h"

Wallpaper * wallp;
int sunRadius;
long seed;

//sprite for stuff
sf::Sprite grassSprite0, grassSprite1;

extern "C" int init(Wallpaper * set){
	std::cout << "inited" << std::endl;

	set->refresh = 0.01;	//once every second
	wallp = set;

	//scene settings
	sunRadius = 25;
	seed = time(NULL);

	//create grass
	sf::Texture * grassTexture0 = new sf::Texture;
	sf::Texture * grassTexture1 = new sf::Texture;
	if (!grassTexture0->loadFromFile("grass0.png")
			|| !grassTexture1->loadFromFile("grass1.png")){
		std::cout << "couldn't load texture" << std::endl;
		return 1;
	}
	
	grassSprite0.setTexture(*grassTexture0);
	grassSprite1.setTexture(*grassTexture1);

	//grassSprite0.setScale(sf::Vector2f(3.0f, 3.0f));
	//grassSprite1.setScale(sf::Vector2f(3.0f, 3.0f));
	grassSprite0.setScale(3, -3);
	grassSprite1.setScale(3, -3);

	return 0;
}

extern "C" int redraw(void){
	const bool redrawGrass = true;

	sf::CircleShape shape(sunRadius);
	shape.setFillColor(sf::Color::White);

	long curtime = (seed += 60);
	//long curtime = time(NULL);
	struct tm *tm_struct = localtime(&curtime);
	long dayInSec = (tm_struct->tm_hour * 60 * 60)
				+ (tm_struct->tm_min * 60)
				+ tm_struct->tm_sec;
	const long dayTotalSec = 24 * 60 * 60;
	double DayProgress = ((float)dayInSec / (float)dayTotalSec);

	std::cout << "time: " <<  tm_struct->tm_hour << ':' << tm_struct->tm_min 
		<< ':' << tm_struct->tm_sec<< std::endl;

	float xpos = DayProgress * (wallp->width + (sunRadius * 2)) - (sunRadius * 2);
	float ypos = sin(DayProgress * 3) * (wallp->height - (sunRadius * 2.5));

	shape.setPosition(xpos, ypos);
	wallp->renderBuff->clear();
	wallp->renderBuff->draw(shape);

	if(redrawGrass){
		srand(seed);
		int i = 0;
		sf::RectangleShape grassRect(sf::Vector2f(wallp->width, 50));
		grassRect.setFillColor(sf::Color(131, 203, 83));
		wallp->renderBuff->draw(grassRect);
		while((grassSprite0.getTexture()->getSize().x
					* grassSprite0.getScale().x) * i < wallp->width){
			if(rand() % 2){
				grassSprite0.setPosition(
						(grassSprite0.getTexture()->getSize().x
						* grassSprite0.getScale().x) * i,
						grassSprite0.getTexture()->getSize().x);
				wallp->renderBuff->draw(grassSprite0);
			}else{
				grassSprite1.setPosition(
						(grassSprite1.getTexture()->getSize().x
						* grassSprite1.getScale().x) * i,
						grassSprite1.getTexture()->getSize().x);
				wallp->renderBuff->draw(grassSprite1);
			}
			i++;
		}

	}

	return 0;
}

extern "C" int deinit(void){
	delete grassSprite0.getTexture();
	delete grassSprite1.getTexture();
	return 0;
}
