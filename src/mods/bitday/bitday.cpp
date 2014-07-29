#include <iostream>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <string>

#include "../../Wallpaper.h"
#include "images.h"

#define NUM_GRASS_TEXTURES 2
#define NUM_CLOUD_TEXTURES 5

//time stuff
#define HOURS_IN_A_DAY 24
#define MINUTES_IN_AN_HOUR 60
#define SECONDS_IN_A_MINUTE 60

Wallpaper * wallp;
long seed, test;
bool redrawGrass;

//settings
int sunRadius, grassScale, grassOffset;
float sunA, sunB, sunC;

//sprite for stuff
sf::Sprite *grassSprites;
sf::Sprite *cloudSprites;

extern "C" int init(Wallpaper * set){
	set->refresh = 0.01;	//once every second
	wallp = set;

	//scene settings
	grassScale = 3;
	sunRadius = 25;
	grassOffset = 100;
	sunA = 400;
	sunB = 0;
	sunC = -(wallp->height - (sunRadius * 3));

	//setup stuff
	seed = time(NULL);
	test = seed;
	redrawGrass = true;

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
		grassSprites[i].setScale(grassScale, -grassScale);
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
	sf::CircleShape shape(sunRadius);

	long curtime = (test += 60 * 5);
	//long curtime = time(NULL);

	struct tm *tm_struct = localtime(&curtime);
	long dayInSec =
		(tm_struct->tm_hour * MINUTES_IN_AN_HOUR * SECONDS_IN_A_MINUTE)
				+ (tm_struct->tm_min * SECONDS_IN_A_MINUTE)
				+ tm_struct->tm_sec;

	const long dayTotalSec = HOURS_IN_A_DAY
		* MINUTES_IN_AN_HOUR
		* SECONDS_IN_A_MINUTE;

	double DayProgress = ((float)dayInSec / (float)dayTotalSec);

	//print status message
	std::cout << "time: " <<  tm_struct->tm_hour << ':' << tm_struct->tm_min
		<< ':' << tm_struct->tm_sec << " prog: " << DayProgress * 100 << '%'
		<< std::endl;

	//x pos will correspond to dayprogress with min being -diameter(radius*2)
	//and max being the screen width
	float sunXPos = (DayProgress * (wallp->width + (sunRadius * 2)))
		- (sunRadius * 2);

	//some fancy parabola stuff, thanks shoemaker
	float x = (sunXPos - (wallp->width / 2)) / (wallp->width / 2);

	float sunYPos = (-((sunA * pow(x, 2)) + (sunB * x) + sunC) + 1);

	//correct for sun radius
	sunXPos -= sunRadius;

	shape.setFillColor(sf::Color((tm_struct->tm_yday * 50) % 256,
				(tm_struct->tm_yday * 100) % 256,
				(tm_struct->tm_yday * 15) % 256));
	shape.setPosition(sunXPos, sunYPos);

	//wallp->renderBuff->clear();
	wallp->renderBuff->draw(shape);

	if(redrawGrass){
		srand(seed);
		int i = 0;
		sf::RectangleShape grassRect(sf::Vector2f(wallp->width, 50
					+ grassOffset));
		grassRect.setFillColor(sf::Color(131, 203, 83));
		wallp->renderBuff->draw(grassRect);
		while((grassSprites[0].getTexture()->getSize().x
					* grassSprites[0].getScale().x) * i < wallp->width){
			int spriteChoice = rand() % 2;
			grassSprites[spriteChoice].setPosition(
					(grassSprites[spriteChoice].getTexture()->getSize().x
					* grassSprites[spriteChoice].getScale().x) * i,
					grassSprites[spriteChoice].getTexture()->getSize().x
					+ grassOffset);
			wallp->renderBuff->draw(grassSprites[spriteChoice]);
			i++;
		}
	}

	if(sunYPos < (3 * 16) + grassOffset){
		redrawGrass = true;
	}else{
		redrawGrass = false;
	}

	return 0;
}

extern "C" int deinit(void){
	delete [] grassSprites;
	return 0;
}
