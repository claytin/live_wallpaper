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

long test;

Wallpaper *wallp;	//holds all the info it needs

long seed;	//keep same seed
bool redrawGrass;

//settings
int sunRadius, grassScale, grassOffset;
float sunA, sunB, sunC;	//sun parabola stuff

//sprite for stuff
sf::Sprite *grassSegmentSprites;	//sprite for every different ground type
sf::Sprite *cloudSprites;	//sprite for every diffferent cloud

//holds the entire rendered grass, about as big as the screen width
sf::Sprite renderedGrassSprite;
sf::RenderTexture *renderedGrassTexture;

//function defs
extern "C" int init(Wallpaper *);
extern "C" int redraw(void);
extern "C" int deinit(void);
sf::RenderTexture* drawGrass(sf::RenderTexture *);
int initGrass(void);
int initClouds(void);

extern "C" int init(Wallpaper * set){
	set->refresh = 0.01;	//once every second
	wallp = set;

	//scene settings
	grassScale = 3;
	sunRadius = 25;
	grassOffset = 140;
	sunA = 400;
	sunB = 0;
	sunC = sunRadius;	//one radius from the top

	//setup stuff
	seed = time(NULL);
	test = seed;
	redrawGrass = true;

	if(initGrass() || initClouds()){
		return 1;
	}

	return 0;
}

extern "C" int deinit(void){
	delete [] grassSegmentSprites;
	return 0;
}

int initGrass(void){
	//create grass texture for each fragment
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

	//crate sprite for each fragment
	grassSegmentSprites = new sf::Sprite[NUM_GRASS_TEXTURES];
	for(int i = 0; i < NUM_GRASS_TEXTURES; i++){
		grassSegmentSprites[i].setTexture(grassTextures[i]);
		grassSegmentSprites[i].setScale(grassScale, grassScale);
	}

	//create texture and sprite to hold rendered fragments
	renderedGrassTexture = new sf::RenderTexture();
	renderedGrassTexture->create(wallp->width, wallp->height);
	renderedGrassSprite.setTexture(renderedGrassTexture->getTexture());

	renderedGrassTexture =  drawGrass(renderedGrassTexture);

	return 0;
}

int initClouds(void){
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

sf::RenderTexture* drawGrass(sf::RenderTexture * texture){
	//draw a rectangle behind the grass from bottom of screen to grass middle
	sf::RectangleShape grassRect(sf::Vector2f(wallp->width, grassOffset - (5 * grassScale)));
	grassRect.setPosition(0, wallp->height - grassRect.getSize().y);
	grassRect.setFillColor(sf::Color(131, 203, 83));
	texture->draw(grassRect);

	//always use the same random otherwise the grass will change between redraws
	srand(seed);
	
	//grass y never changes so...
	float grassY = wallp->height - grassOffset;

	//while there is screen space left keep drawing a random piece of grass
	int i = 0;
	while((grassSegmentSprites[0].getTexture()->getSize().x
				* grassSegmentSprites[0].getScale().x) * i < wallp->width){
		int spriteChoice = rand() % 2;

		float grassX =
			(grassSegmentSprites[spriteChoice].getTexture()->getSize().x
			* grassSegmentSprites[spriteChoice].getScale().x) * i;
		
		grassSegmentSprites[spriteChoice].setPosition(grassX, grassY);
		texture->draw(grassSegmentSprites[spriteChoice]);
		i++;
	}
	texture->display();

	return texture;
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

	float sunYPos = ((sunA * pow(x, 2)) + (sunB * x) + sunC) + 1;

	//correct for sun radius
	sunXPos -= sunRadius;

	shape.setFillColor(sf::Color((tm_struct->tm_yday * 50) % 256,
				(tm_struct->tm_yday * 100) % 256,
				(tm_struct->tm_yday * 15) % 256));
	shape.setPosition(sunXPos, sunYPos);

	wallp->renderBuff->clear();
	wallp->renderBuff->draw(shape);

	renderedGrassTexture = drawGrass(renderedGrassTexture);
	wallp->renderBuff->draw(renderedGrassSprite);
	wallp->renderBuff->display();

	std::cout << "500" << std::endl;
	if(sunYPos < 500){
		redrawGrass = true;
	}else{
		redrawGrass = false;
	}

	return 0;
}
