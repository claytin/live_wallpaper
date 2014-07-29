#include <iostream>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <string>

#include "../../Wallpaper.h"
#include "images.h"

#define NUM_GRASS_TEXTURES 2
#define NUM_CLOUD_TEXTURES 5

Wallpaper * wallp;
int sunRadius;
long seed, test;

//sprite for stuff
sf::Sprite *grassSprites;
sf::Sprite *cloudSprites;

extern "C" int init(Wallpaper * set){
	set->refresh = 0.01;	//once every second
	wallp = set;

	//scene settings
	sunRadius = 25;
	seed = time(NULL);
	test = seed;

	//create grass
	sf::Texture * grassTextures = new sf::Texture[NUM_GRASS_TEXTURES];
	
	//load textures
	for(int i = 0; i < NUM_GRASS_TEXTURES; i++){
		std::string textureName = "grass" + std::to_string(i) + ".png";
		if (!grassTextures[i].loadFromFile(textureName)){
			std::cout << "couldn't load texture: \"" << textureName << '"'
				<< std::endl;
			return 1;
		}
	}
	grassSprites = new sf::Sprite[NUM_GRASS_TEXTURES];
	for(int i = 0; i < NUM_GRASS_TEXTURES; i++){
		grassSprites[i].setTexture(grassTextures[i]);
		grassSprites[i].setScale(3, -3);
	}

	//cloud stuff
	sf::Texture * cloudTextures = new sf::Texture[NUM_CLOUD_TEXTURES];
	for(int i = 0; i < NUM_CLOUD_TEXTURES; i++){
		std::string textureName = "cloud" + std::to_string(i) + ".png";
		if (!cloudTextures[i].loadFromFile(textureName)){
			std::cout << "couldn't load texture: \"" << textureName << '"'
				<< std::endl;
			return 1;
		}
	}
	cloudSprites = new sf::Sprite[NUM_CLOUD_TEXTURES];
	for(int i = 0; i < NUM_CLOUD_TEXTURES; i++){
		cloudSprites[i].setTexture(cloudTextures[i]);
	}

	return 0;
}

extern "C" int redraw(void){
	const bool redrawGrass = true;

	sf::CircleShape shape(sunRadius);

	long curtime = (test += 60 * 5);
	//long curtime = time(NULL);
	struct tm *tm_struct = localtime(&curtime);
	long dayInSec = (tm_struct->tm_hour * 60 * 60)
				+ (tm_struct->tm_min * 60)
				+ tm_struct->tm_sec;
	const long dayTotalSec = 24 * 60 * 60;
	double DayProgress = ((float)dayInSec / (float)dayTotalSec);

	std::cout << "time: " <<  tm_struct->tm_hour << ':' << tm_struct->tm_min
		<< ':' << tm_struct->tm_sec << " prog: " << DayProgress * 100 << '%'
		<< std::endl;

	float xpos = DayProgress * (wallp->width + (sunRadius * 2)) - (sunRadius * 2);
	float ypos = sin(DayProgress * 3) * (wallp->height - (sunRadius * 2.5));

	shape.setFillColor(sf::Color(DayProgress * 255, (1 - DayProgress) * 255, (1 - DayProgress) * 100));
	shape.setPosition(xpos, ypos);
	wallp->renderBuff->draw(shape);

	if(redrawGrass){
		srand(seed);
		int i = 0;
		sf::RectangleShape grassRect(sf::Vector2f(wallp->width, 50));
		grassRect.setFillColor(sf::Color(131, 203, 83));
		wallp->renderBuff->draw(grassRect);
		while((grassSprites[0].getTexture()->getSize().x
					* grassSprites[0].getScale().x) * i < wallp->width){
			if(rand() % 2){
				grassSprites[0].setPosition(
						(grassSprites[0].getTexture()->getSize().x
						* grassSprites[0].getScale().x) * i,
						grassSprites[0].getTexture()->getSize().x);
				wallp->renderBuff->draw(grassSprites[0]);
			}else{
				grassSprites[1].setPosition(
						(grassSprites[1].getTexture()->getSize().x
						* grassSprites[1].getScale().x) * i,
						grassSprites[1].getTexture()->getSize().x);
				wallp->renderBuff->draw(grassSprites[1]);
			}
			i++;
		}
	}

	return 0;
}

extern "C" int deinit(void){
	delete [] grassSprites;
	return 0;
}
