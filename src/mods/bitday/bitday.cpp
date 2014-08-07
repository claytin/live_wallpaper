#include <iostream>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <string>

#include "../../Wallpaper.h"
#include "images.h"

//textures all saved with <type><num>.png names
//ex: grass0.png would be the first grass texture
//the max number of each is listed here
#define NUM_GRASS_TEXTURES 2
#define NUM_CLOUD_TEXTURES 5
#define NUM_WATER_TEXTURES 1

//time stuff
#define HOURS_IN_A_DAY 24
#define MINUTES_IN_AN_HOUR 60
#define SECONDS_IN_A_MINUTE 60

//test stuff idk
long test;

//all information about the wallpaper needed while drawing
Wallpaper *wallp;


//use the same seed for the entire session so redrawing
//with random values will be consistent
long seed;

//distance from bottom of screen that grass will be draw at
int grassOffset;

//scale of each sprite. 1 means each texture pixel is one pixel on screen
int grassScale, waterScale, cloudScale;
int waterOffset;

//how many clouds to generate depending on time of day
//day and night have their own cloud buffers
//this is the number of clouds that will be rendered to each buffer
int cloudCountDay;
int cloudCountNight;

//size of sun obviously
int sunRadius;

//sun y position is calculation with a parabola using a b c
float sunA, sunB, sunC;

//sun rise/set times in format <sunsetHour>:<sunsetMinute>
int sunsetHour;
int sunsetMinute;
int sunriseHour;
int sunriseMinute;

//holds the sprites that have been directly loaded from the texture files
sf::Sprite *grassSegmentSprites;
sf::Sprite *cloudSprites;
sf::Sprite *waterSegmentSprites;

//the segment sprites are rendered to these sprites
//each "rendered" sprite will likely have multiple "segment"
//sprites rendered to it
sf::Sprite renderedGrassSprite;
sf::RenderTexture *renderedGrassTexture;
sf::Sprite renderedWaterSprite;
sf::RenderTexture *renderedWaterTexture;

//rendered sprites for clouds
//there is a separate one for day and night
//allowing different number of clouds to be rendered
//depending on time of day
sf::Sprite renderedCloudSpriteDay;
sf::RenderTexture *renderedCloudTextureDay;
sf::Sprite renderedCloudSpriteNight;
sf::RenderTexture *renderedCloudTextureNight;

//these are all the functions that are called by the wallpaper
extern "C" int init(Wallpaper *);
extern "C" int redraw(void);
extern "C" int deinit(void);

//renders sprites to a given texture
sf::RenderTexture* drawGrass(sf::RenderTexture *);
sf::RenderTexture* drawWater(sf::RenderTexture *);

//same as other draw functions except it takes bool isDay and int seed
//isDay: whether to draw using "cloudCoundDay" or "cloudCoundNight"
//seed: random seed to use when generating clouds, this is here to allow
//the clouds to be redraw while off-screen in a different configuration
sf::RenderTexture* drawClouds(sf::RenderTexture *, bool isDay,
	int seed);

//loads textures and creates sprites/rendertextures needed for each object
int initGrass(void);
int initWater(void);
int initClouds(void);

extern "C" int init(Wallpaper * set){
	set->refresh = 0.01; //once every second
	wallp = set;

	//default scene settings
	grassScale = 3;
	waterScale = 3;
	cloudScale = 3;
	sunRadius = 25;
	grassOffset = 140;
	waterOffset = 195;
	cloudCountNight = 20;
	cloudCountDay = 1;
	sunA = 400;
	sunB = 0;
	sunC = sunRadius; //one radius from the top

	//default sun times
	//rise = 6:18, set = 20:08
	sunriseHour = 6;
	sunriseMinute = 18;
	sunsetHour = 20;
	sunsetMinute = 8;

	//setup stuff
	seed = time(NULL);
	test = seed;

	//load/create all the sprites and stuff
	//if one of them has an error then everything
	//is aborted
	if(initGrass() || initClouds() || initWater()){
		return 1;
	}

	return 0;
}

extern "C" int deinit(void){
	//clean everything up
	delete [] grassSegmentSprites;
	delete [] cloudSprites;
	delete [] waterSegmentSprites;
	delete renderedGrassTexture;
	delete renderedWaterTexture;
	delete renderedCloudTextureDay;
	delete renderedCloudTextureNight;
	return 0;
}

extern "C" int redraw(void){
	sf::CircleShape sunShape(sunRadius);
	sf::CircleShape moonShape(sunRadius);

	sunShape.setFillColor(sf::Color::Red);
	moonShape.setFillColor(sf::Color::Yellow);

	long curtime = (test += 60);
	//long curtime = time(NULL);

	struct tm *tm_struct = localtime(&curtime);
	long timeInSec = (tm_struct->tm_hour * MINUTES_IN_AN_HOUR *
		SECONDS_IN_A_MINUTE) + (tm_struct->tm_min * SECONDS_IN_A_MINUTE) +
		tm_struct->tm_sec;

	const long dayTotalSec = HOURS_IN_A_DAY *
		MINUTES_IN_AN_HOUR *
		SECONDS_IN_A_MINUTE;

	//how far though the day it is
	//0 is midnight (morning)
	//0.5 is noon
	//1 is midnight (evening)
	double DayProgress = ((float)timeInSec / (float)dayTotalSec);

	//how far the sun is in its path (similar to "DayProgress")
	//0 sunrise
	//0.5 noon
	//1 sunset
	//its calculated using the sunset and sunrise times
	double SunProgress = 0;

	//convert sun<rise/set><min/hour> to total seconds
	const long sunsetInSec = (sunsetHour * MINUTES_IN_AN_HOUR *
			SECONDS_IN_A_MINUTE) + (sunsetMinute * SECONDS_IN_A_MINUTE);
	const long sunriseInSec = (sunriseHour * MINUTES_IN_AN_HOUR *
			SECONDS_IN_A_MINUTE) + (sunriseMinute * SECONDS_IN_A_MINUTE);

	//get amount of time that sun will be out (in seconds)
	const long sunTotalUpSec = sunsetInSec - sunriseInSec;

	//if sun is currently up and should be rendered/calculated further
	bool sunUp = timeInSec > sunriseInSec && timeInSec < sunsetInSec;

	if(sunUp){
		SunProgress = (float)(timeInSec - sunriseInSec) / (float)sunTotalUpSec;
	}

	//print status message
	if(tm_struct->tm_min == 0){
		std::cout << "time: " <<  tm_struct->tm_hour << ':'
			<< tm_struct->tm_min << ':' << tm_struct->tm_sec
			<< " prog: " << DayProgress * 100
			<< '%' << std::endl;
	}

	//x pos will correspond to dayprogress with min being -diameter(radius*2)
	//and max being the screen width
	float sunXPos = (SunProgress * (wallp->width + (sunRadius * 2))) -
		(sunRadius * 2);

	//some fancy parabola stuff, thanks shoemaker
	float x = (sunXPos - (wallp->width / 2)) / (wallp->width / 2);
	float sunYPos = ((sunA * pow(x, 2)) + (sunB * x) + sunC) + 1;

	//correct for sun radius
	sunXPos -= sunRadius;

	sunShape.setPosition(sunXPos, sunYPos);

	wallp->renderBuff->clear(sf::Color(115, 224, 255));
	wallp->renderBuff->draw(sunShape);

	//TODO: some render stuff not sure why its here
	//renderedWaterTexture = drawWater(renderedWaterTexture);
	//renderedGrassTexture = drawGrass(renderedGrassTexture);
	//renderedCloudTextureDay = drawClouds(renderedCloudTextureDay, true, cloudProg);
	//renderedCloudTextureNight = drawClouds(renderedCloudTextureNight, false, cloudProg);

	wallp->renderBuff->draw(renderedWaterSprite);
	wallp->renderBuff->draw(renderedGrassSprite);

	//re-gen night clouds at midnight and dayclouds at noon
	if(tm_struct->tm_min == 0 && tm_struct->tm_hour == 0){
		renderedCloudTextureNight = drawClouds(renderedCloudTextureNight,
			false, time(NULL));
	}else if(tm_struct->tm_min == 0 && tm_struct->tm_hour == 12){
		renderedCloudTextureDay = drawClouds(renderedCloudTextureDay,
			true, time(NULL));
	}

	//cloud stuff
	renderedCloudSpriteDay.setPosition((((int)(wallp->width * DayProgress * 2)
		+ wallp->width) % (wallp->width * 2)) - wallp->width, 0);
	renderedCloudSpriteNight.setPosition(((int)(wallp->width * DayProgress * 2)
		% (wallp->width * 2)) - wallp->width, 0);

	//draw clouds and stuff
	wallp->renderBuff->draw(renderedCloudSpriteDay);
	wallp->renderBuff->draw(renderedCloudSpriteNight);

	wallp->renderBuff->display();

	return 0;
}

int initGrass(void){
	//create grass texture for each fragment
	sf::Texture * grassTextures = new sf::Texture[NUM_GRASS_TEXTURES];

	//load textures
	for(int i = 0; i < NUM_GRASS_TEXTURES; i++){
		std::string textureName = "grass" + std::to_string(i) + ".png";
		if (!grassTextures[i].loadFromFile(textureName)){
			std::cout << "couldn't load texture: \"" << textureName
				<< '"' << std::endl;
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
	renderedGrassTexture->create(wallp->width,
		(grassSegmentSprites[0].getTexture()->getSize().y *
		 grassScale) + grassOffset);

	renderedGrassSprite.setTexture(renderedGrassTexture->getTexture());
	renderedGrassSprite.setPosition(0, wallp->height - grassOffset);

	renderedGrassTexture =  drawGrass(renderedGrassTexture);

	return 0;
}

int initWater(void){
	sf::Texture * waterTextures = new sf::Texture[NUM_WATER_TEXTURES];
	
	//load textures
	for(int i = 0; i < NUM_WATER_TEXTURES; i++){
		std::string textureName = "water" + std::to_string(i) + ".png";
		if (!waterTextures[i].loadFromFile(textureName)){
			std::cout << "couldn't load texture: \""
				<< textureName << '"' << std::endl;
			return 1;
		}
	}

	//crate sprite for each fragment
	waterSegmentSprites = new sf::Sprite[NUM_WATER_TEXTURES];
	for(int i = 0; i < NUM_WATER_TEXTURES; i++){
		waterSegmentSprites[i].setTexture(waterTextures[i]);
		waterSegmentSprites[i].setScale(waterScale, waterScale);
	}

	//create texture and sprite to hold rendered fragments
	renderedWaterTexture = new sf::RenderTexture();
	renderedWaterTexture->create(wallp->width,
			(waterSegmentSprites[0].getTexture()->getSize().y * waterScale) +
			waterOffset);

	renderedWaterSprite.setTexture(renderedWaterTexture->getTexture());
	renderedWaterSprite.setPosition(0, wallp->height - waterOffset);

	renderedWaterTexture =  drawWater(renderedWaterTexture);

	return 0;
}

int initClouds(void){
	//cloud stuff
	sf::Texture * cloudTextures = new sf::Texture[NUM_CLOUD_TEXTURES];
	for(int i = 0; i < NUM_CLOUD_TEXTURES; i++){
		std::string textureName = "cloud" + std::to_string(i) + ".png";
		if (!cloudTextures[i].loadFromFile(textureName)){
			std::cout << "couldn't load texture: \""
				<< textureName << '"' << std::endl;
			return 1;
		}
	}

	cloudSprites = new sf::Sprite[NUM_CLOUD_TEXTURES];
	for(int i = 0; i < NUM_CLOUD_TEXTURES; i++){
		cloudSprites[i].setTexture(cloudTextures[i]);
		cloudSprites[i].setScale(cloudScale, cloudScale);
	}

	//create both textures
	renderedCloudTextureDay = new sf::RenderTexture();
	renderedCloudTextureDay->create(wallp->width, wallp->height);
	renderedCloudTextureNight = new sf::RenderTexture();
	renderedCloudTextureNight->create(wallp->width, wallp->height);

	renderedCloudSpriteDay.setTexture(renderedCloudTextureDay->getTexture());
	renderedCloudSpriteDay.setPosition(0, 0);
	renderedCloudSpriteNight.setTexture(renderedCloudTextureNight->
		getTexture());
	renderedCloudSpriteNight.setPosition(0, 0);

	renderedCloudTextureDay = drawClouds(renderedCloudTextureDay, true, seed);
	renderedCloudTextureNight = drawClouds(renderedCloudTextureNight, false,
		seed);

	return 0;
}

sf::RenderTexture* drawGrass(sf::RenderTexture *texture){
	//draw a rectangle behind the grass from bottom of screen to grass middle
	sf::RectangleShape grassRect(sf::Vector2f(wallp->width,
		grassOffset - (5 * grassScale)));

	grassRect.setPosition(0, 5 * grassScale);
	grassRect.setFillColor(sf::Color(131, 203, 83));
	texture->draw(grassRect);

	//always use the same random otherwise the grass will change and stuff
	srand(seed);
	
	//grass y never changes so...
	float grassY = 0;

	//while there is screen space left keep drawing a random piece of grass
	int i = 0;
	while((grassSegmentSprites[0].getTexture()->getSize().x *
			grassSegmentSprites[0].getScale().x) * i < wallp->width){
		int spriteChoice = rand() % 2;

		float grassX =
			(grassSegmentSprites[spriteChoice].getTexture()->getSize().x *
				grassSegmentSprites[spriteChoice].getScale().x) * i;

		grassSegmentSprites[spriteChoice].setPosition(grassX, grassY);
		texture->draw(grassSegmentSprites[spriteChoice]);
		i++;
	}
	texture->display();

	return texture;
}

sf::RenderTexture* drawWater(sf::RenderTexture *texture){
	sf::RectangleShape waterRect(sf::Vector2f(wallp->width,
		wallp->height - grassOffset));

	waterRect.setPosition(0, 5 * grassScale);
	waterRect.setFillColor(sf::Color(92, 166, 255));
	texture->draw(waterRect);

	float waterY = 0;

	for(int i = 0; (int)(i * waterScale * waterSegmentSprites->getTexture()->
			getSize().x) < wallp->width; i++){
		float waterX =
			(waterSegmentSprites[0].getTexture()->getSize().x *
				waterSegmentSprites[0].getScale().x) * i;

		waterSegmentSprites[0].setPosition(waterX, waterY);
		texture->draw(waterSegmentSprites[0]);
	}

	texture->display();

	return texture;
}

sf::RenderTexture* drawClouds(sf::RenderTexture *texture, bool isDay,
		int cloudSeed){
	srand(cloudSeed);
	//texture->clear(sf::Color(0, 0, 0, 0));

	int cloudCount;
	if(isDay){
		texture->clear(sf::Color(255, 255, 255, 10));
		cloudCount = cloudCountDay;
	}else{
		texture->clear(sf::Color(0, 0, 0, 10));
		cloudCount = cloudCountNight;
	}

	for(int i = 0; i < cloudCount; i++){
		int randSprite = rand() % NUM_CLOUD_TEXTURES;
		cloudSprites[randSprite].setPosition(rand() %
			(wallp->width - (cloudSprites[rand() %
			NUM_CLOUD_TEXTURES].getTexture()->getSize().x *
			cloudScale)), (rand() % ((wallp->height - waterOffset) / 5)) +
			100);

		texture->draw(cloudSprites[randSprite]);
	}

	texture->display();
	return texture;
}
//TODO: fuck
