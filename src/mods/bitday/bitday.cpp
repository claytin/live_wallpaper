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
//isDay: wether to draw using "cloudCoundDay" or "cloudCoundNight"
//seed: random seed to use when generating clouds, this is here to allow
//the clouds to be redraw while offscreen in a different configuration
sf::RenderTexture* drawClouds(sf::RenderTexture *, bool isDay,
	int seed);

//loads textures and creates sprites/rendertextures needed for each object
int initGrass(void);
int initWater(void);
int initClouds(void);

extern "C" int init(Wallpaper * set){
	set->refresh = 0.01; //once every second
	wallp = set;

	//scene settings
	grassScale = 3;
	waterScale = 3;
	cloudScale = 3;
	sunRadius = 25;
	grassOffset = 140;
	waterOffset = 195;
	cloudCountNight = 1;
	cloudCountDay = 20;
	sunA = 400;
	sunB = 0;
	sunC = sunRadius; //one radius from the top

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

//TODO: temp variable fucko
int cloudProg = 0;

extern "C" int redraw(void){
	sf::CircleShape shape(sunRadius);

	long curtime = (test += 60 * 5);
	//long curtime = time(NULL);

	struct tm *tm_struct = localtime(&curtime);
	long dayInSec = (tm_struct->tm_hour * MINUTES_IN_AN_HOUR *
		SECONDS_IN_A_MINUTE) + (tm_struct->tm_min * SECONDS_IN_A_MINUTE) +
		tm_struct->tm_sec;

	const long dayTotalSec = HOURS_IN_A_DAY *
		MINUTES_IN_AN_HOUR *
		SECONDS_IN_A_MINUTE;

	double DayProgress = ((float)dayInSec / (float)dayTotalSec);

	//print status message
	std::cout << "time: " <<  tm_struct->tm_hour << ':' << tm_struct->tm_min
		<< ':' << tm_struct->tm_sec << " prog: " << DayProgress * 100
		<< '%' << std::endl;

	//x pos will correspond to dayprogress with min being -diameter(radius*2)
	//and max being the screen width
	float sunXPos = (DayProgress * (wallp->width + (sunRadius * 2))) -
		(sunRadius * 2);

	//some fancy parabola stuff, thanks shoemaker
	float x = (sunXPos - (wallp->width / 2)) / (wallp->width / 2);
	float sunYPos = ((sunA * pow(x, 2)) + (sunB * x) + sunC) + 1;

	//correct for sun radius
	sunXPos -= sunRadius;

	shape.setFillColor(sf::Color((tm_struct->tm_yday * 50) % 256,
		(tm_struct->tm_yday * 100) % 256,
		(tm_struct->tm_yday * 15) % 256));

	shape.setPosition(sunXPos, sunYPos);

	wallp->renderBuff->clear(sf::Color(115, 224, 255));
	wallp->renderBuff->draw(shape);

	//renderedWaterTexture = drawWater(renderedWaterTexture);
	wallp->renderBuff->draw(renderedWaterSprite);

	//renderedGrassTexture = drawGrass(renderedGrassTexture);
	wallp->renderBuff->draw(renderedGrassSprite);

	//cloud stuff
	cloudProg++;
	//renderedCloudTextureDay = drawClouds(renderedCloudTextureDay, true, cloudProg);
	renderedCloudSpriteDay.setPosition(((cloudProg + wallp->width) %
		(wallp->width * 2)) - wallp->width, 0);

	renderedCloudTextureNight = drawClouds(renderedCloudTextureNight, false, cloudProg);
	renderedCloudSpriteNight.setPosition((cloudProg % (wallp->width * 2)) -
		wallp->width, 0);

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
	texture->clear(sf::Color(0, 0, 0, 0));

	int cloudCount;
	if(isDay){
		cloudCount = cloudCountDay;
	}else{
		cloudCount = cloudCountNight;
	}

	for(int i = 0; i < cloudCount; i++){
		cloudSprites[rand() % NUM_CLOUD_TEXTURES].setPosition(rand() %
			(wallp->width - (cloudSprites[rand() %
			NUM_CLOUD_TEXTURES].getTexture()->getSize().x *
			cloudScale)), (rand() % ((wallp->height - waterOffset) / 5)) +
			100);

		texture->draw(cloudSprites[rand() % NUM_CLOUD_TEXTURES]);
		texture->display();
	}
	return texture;
}
