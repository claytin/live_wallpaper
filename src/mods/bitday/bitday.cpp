#include <iostream>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <string>

#include "../../Wallpaper.h"
#include "images.h"

//TODO: textures in header file

//textures all saved with <type><num>.png names
//ex: grass0.png would be the first grass texture
//the max number of each is listed here
#define NUM_GRASS_TEXTURES 2
#define NUM_CLOUD_TEXTURES 5
#define NUM_WATER_TEXTURES 1
#define NUM_DITHER_TEXTURES 4
#define NUM_GRADIENT_TEXTURES 1

//time stuff
#define HOURS_IN_A_DAY 24
#define MINUTES_IN_AN_HOUR 60
#define SECONDS_IN_A_MINUTE 60

//level of verbosity
//0 = no output not even errors (quiet)
//1 = normal verbosity (default)
//2 = debug output
int verbosity;

//test stuff idk
long test;

//all information about the wallpaper needed while drawing
Wallpaper *wallp;

//use the same seed for the entire session so redrawing
//with random values will be consistent
long seed;

//distance from bottom of screen that grass will be draw at
int grassOffset;

//scale of each sprite from 1:<num> res
//with 1 being each texture pixel is one on screen pixel
int grassScale, waterScale, cloudScale, gradientDitherScale;
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

//moon rise/set times (same as sun rise/set...)
int moonsetHour;
int moonsetMinute;
int moonriseHour;
int moonriseMinute;

//holds the sprites that have been directly loaded from the texture files
sf::Sprite *grassSegmentSprites;
sf::Sprite *cloudSprites;
sf::Sprite *waterSegmentSprites;
sf::Sprite *gradientDitherSprites;

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

//for skype gradient and stuff
sf::Sprite renderedBackgroundGradientSprite;
sf::RenderTexture *renderedBackgroundGradientTexture;

//temp sun/moon stuff
//TODO: get rid of this
sf::CircleShape sunShape(sunRadius);
sf::CircleShape moonShape(sunRadius);

//these are all the functions that are called by the wallpaper
extern "C" int init(Wallpaper *);
extern "C" int redraw(void);
extern "C" int deinit(void);

//renders sprites to a given texture
sf::RenderTexture* drawGrass(sf::RenderTexture *);
sf::RenderTexture* drawWater(sf::RenderTexture *);
sf::RenderTexture* drawBackgroundGradient(sf::RenderTexture *);

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
int initGradient(void);

extern "C" int init(Wallpaper * set){
	set->refresh = 0.01; //once every second
	wallp = set;

	//default scene settings
	grassScale = 3;
	waterScale = 3;
	cloudScale = 3;
	gradientDitherScale = 3;
	sunRadius = 25;
	grassOffset = 140;
	waterOffset = 195;
	cloudCountNight = 20;
	cloudCountDay = 5;
	sunA = 400;
	sunB = 0;
	sunC = sunRadius; //one radius from the top

	//default sun times
	//rise = 6:20, set = 20:00
	sunriseHour = 6;
	sunriseMinute = 20;
	sunsetHour = 20;
	sunsetMinute = 0;

	//default moon times
	//rise = 5:30, set = 6:00
	moonriseHour = 19;
	moonriseMinute = 30;
	moonsetHour = 6;
	moonsetMinute = 0;

	//setup stuff
	seed = time(NULL);
	test = seed;

	//set verbosity to debug for now
	verbosity = 2;

	//load/create all the sprites and stuff
	//if one of them has an error then everything
	//is aborted
	if(initGrass() ||
			initClouds() ||
			initWater() ||
			initGradient()){
		return 1;
	}

	return 0;
}

extern "C" int deinit(void){
	if(verbosity){
		std::cout << "cleaning up and exiting" << std::endl;
	}
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
	sunShape.setFillColor(sf::Color::Yellow);
	moonShape.setFillColor(sf::Color::Blue);

	const long curtime = (test += 60);
	//const long curtime = time(NULL);

	struct tm *tm_struct = localtime(&curtime);
	const long timeInSec = (tm_struct->tm_hour * MINUTES_IN_AN_HOUR *
		SECONDS_IN_A_MINUTE) + (tm_struct->tm_min * SECONDS_IN_A_MINUTE) +
		tm_struct->tm_sec;

	const long dayTotalSec = HOURS_IN_A_DAY *
		MINUTES_IN_AN_HOUR *
		SECONDS_IN_A_MINUTE;

	//how far though the day it is
	//0 is midnight (morning)
	//0.5 is noon
	//1 is midnight (evening)
	const double DayProgress = ((float)timeInSec / (float)dayTotalSec);


	//convert sun<rise/set><min/hour> to total seconds
	const long sunsetTimeInSec = (sunsetHour * MINUTES_IN_AN_HOUR *
			SECONDS_IN_A_MINUTE) + (sunsetMinute * SECONDS_IN_A_MINUTE);
	const long sunriseTimeInSec = (sunriseHour * MINUTES_IN_AN_HOUR *
			SECONDS_IN_A_MINUTE) + (sunriseMinute * SECONDS_IN_A_MINUTE);

	//if sun is currently up and should be rendered/calculated further
	const bool sunUp = timeInSec > sunriseTimeInSec && timeInSec < sunsetTimeInSec;

	wallp->renderBuff->clear(sf::Color(115, 224, 255));

	wallp->renderBuff->draw(renderedBackgroundGradientSprite);

	//only draw/calculate sun if it's up
	if(sunUp){
		//get amount of time that sun will be out (in seconds)
		const long sunTotalUpSec = sunsetTimeInSec - sunriseTimeInSec;

		//how far the sun is in its path (similar to "DayProgress")
		//0 sunrise
		//0.5 noon
		//1 sunset
		//its calculated using the sunset and sunrise times
		double SunProgress = (float)(timeInSec - sunriseTimeInSec) /
			(float)sunTotalUpSec;

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

		wallp->renderBuff->draw(sunShape);
	}

	//pretty much do the same exact thing for the moon
	//convert sun<rise/set><min/hour> to total seconds
	const long moonsetTimeInSec = (moonsetHour * MINUTES_IN_AN_HOUR *
			SECONDS_IN_A_MINUTE) + (moonsetMinute * SECONDS_IN_A_MINUTE);
	const long moonriseTimeInSec = (moonriseHour * MINUTES_IN_AN_HOUR *
			SECONDS_IN_A_MINUTE) + (moonriseMinute * SECONDS_IN_A_MINUTE);

	//get amount of time that moon will be out (in seconds)
	const long moonTotalUpSec = moonsetTimeInSec + (dayTotalSec -
		moonriseTimeInSec);

	//if moon is currently up and should be rendered/calculated further
	//must account for midnight (00:00)
	const bool moonUpBeforeMidnight = (timeInSec > moonriseTimeInSec &&
		timeInSec < HOURS_IN_A_DAY * MINUTES_IN_AN_HOUR * SECONDS_IN_A_MINUTE);
	const bool moonUpAfterMidnight = (timeInSec < moonsetTimeInSec &&
			timeInSec > 0);
	
	if(moonUpBeforeMidnight || moonUpAfterMidnight){
		//how far the moon is in its path (similar to "DayProgress")
		//0 moonrise
		//0.5 noon
		//1 moonset
		//its calculated using the moon set/rise time and before after midnight
		double moonProgress;

		if(moonUpBeforeMidnight){
			//std::cout << "yep" << std::endl;
			//moon progress before midnight.
			//0 = center of skype
			//<moonrisetime> = about to rise
			moonProgress = (float)(timeInSec - moonriseTimeInSec) /
				(float)moonTotalUpSec;
		}else{
			//moon progress before midnight.
			//0 = center of sky
			//<moonsettime> = moon should have set by now
			moonProgress = ((float)timeInSec / (float)moonTotalUpSec) +
				((float)(dayTotalSec - moonriseTimeInSec) /
				(float)moonTotalUpSec);
		}

		//same thing as sun
		float moonXPos = (moonProgress * (wallp->width + (sunRadius * 2))) -
			(sunRadius * 2);

		float x = (moonXPos - (wallp->width / 2)) / (wallp->width / 2);
		float moonYPos = ((sunA * pow(x, 2)) + (sunB * x) + sunC) + 1;

		//yep
		moonXPos -= sunRadius;

		moonShape.setPosition(moonXPos, moonYPos);

		wallp->renderBuff->draw(moonShape);
	}

	//print status message
	if(tm_struct->tm_min == 0 && verbosity == 2){
		std::cout << "time: " <<  tm_struct->tm_hour << ':'
			<< tm_struct->tm_min << ':' << tm_struct->tm_sec
			<< " prog: " << DayProgress * 100
			<< '%' << std::endl;
	}

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
		if(verbosity){
			std::cout << "loading file: " << textureName << std::endl;
		}
		if(!grassTextures[i].loadFromFile(textureName) && verbosity){
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
		if(verbosity){
			std::cout << "loading file: " << textureName << std::endl;
		}
		if(!waterTextures[i].loadFromFile(textureName) && verbosity){
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
		if(verbosity){
			std::cout << "loading file: " << textureName << std::endl;
		}
		if(!cloudTextures[i].loadFromFile(textureName) && verbosity){
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

int initGradient(void){
	//yep this again
	sf::Texture * ditherTextures = new sf::Texture[NUM_DITHER_TEXTURES];
	gradientDitherSprites = new sf::Sprite[NUM_DITHER_TEXTURES];
	for(int i = 0; i < NUM_DITHER_TEXTURES; i++){
		std::string textureName = "dither" + std::to_string(i) + ".png";
		if(verbosity){
			std::cout << "loading file: " << textureName << std::endl;
		}
		if(!ditherTextures[i].loadFromFile(textureName) && verbosity){
			std::cout << "couldn't load texture: \""
				<< textureName << '"' << std::endl;
			return 1;
		}

		gradientDitherSprites[i].setTexture(ditherTextures[i]);
		gradientDitherSprites[i].scale(gradientDitherScale, gradientDitherScale);
	}

	//texture time?
	renderedBackgroundGradientTexture = new sf::RenderTexture();
	renderedBackgroundGradientTexture->create(wallp->width, wallp->height);

	renderedBackgroundGradientSprite.setTexture(
			renderedBackgroundGradientTexture->getTexture());
	renderedBackgroundGradientTexture = drawBackgroundGradient(
			renderedBackgroundGradientTexture);

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
		int randSprite = rand() % NUM_CLOUD_TEXTURES;
		//cloudSprites[randSprite].setPosition(rand() %
			//(wallp->width - (cloudSprites[rand() %
			//NUM_CLOUD_TEXTURES].getTexture()->getSize().x * cloudScale)),
			//(rand() % ((wallp->height - waterOffset) / 5)) + 100);

		cloudSprites[randSprite].setPosition(rand() % (wallp->width -
			cloudSprites[randSprite].getTexture()->getSize().x * cloudScale),
			(rand() % ((wallp->height - waterOffset) / 5)) + 100);

		texture->draw(cloudSprites[randSprite]);
	}

	texture->display();
	return texture;
}

sf::RenderTexture* drawBackgroundGradient(sf::RenderTexture *texture){
	for(int curDither = NUM_DITHER_TEXTURES - 1; curDither >= 0; curDither--){
		int distAcrossStage = 0;
		while(distAcrossStage < wallp->width){
			gradientDitherSprites[curDither].setPosition(distAcrossStage,
				wallp->height - ((curDither * 40) + waterOffset + 10));
			texture->draw(gradientDitherSprites[curDither]);
		
			distAcrossStage += gradientDitherScale *
				gradientDitherSprites[curDither].getTexture()->getSize().x;
		}
	}

	texture->display();

	return texture;
}
//TODO: fuck
