#include <iostream>
#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <string>

#include "../../Wallpaper.h"

//TODO: textures in header file

//time stuff
#define HOURS_IN_A_DAY 24
#define MINUTES_IN_AN_HOUR 60
#define SECONDS_IN_A_MINUTE 60

//all information about the wallpaper needed while drawing
Wallpaper *wallp;

//use the same seed for the entire session so redrawing
//with random values will be consistent
long seed;


//holds the sprites that have been directly loaded from the texture files
sf::Sprite *mountainSegmentSprite;
sf::Sprite *cloudSprites;
sf::Sprite *treeSegmentSprites;

//the segment sprites are rendered to these sprites
//each "rendered" sprite will likely have multiple "segment"
//sprites rendered to it
sf::Sprite renderedMountainSprite;
sf::RenderTexture *renderedMountainTexture;
sf::Sprite renderedTreeSprite;
sf::RenderTexture *renderedTreeTexture;

//these are all the functions that are called by the wallpaper
extern "C" int init(Wallpaper *);
extern "C" int redraw(void);
extern "C" int deinit(void);

//renders sprites to a given texture
sf::RenderTexture* drawMountains(sf::RenderTexture *);
sf::RenderTexture* drawTrees(sf::RenderTexture *);
sf::RenderTexture* drawClouds(sf::RenderTexture *);

//loads textures and creates sprites/rendertextures needed for each object
int initMountains(void);
int initTrees(void);
int initClouds(void);

extern "C" int init(Wallpaper * set){
	set->refresh = 0.01; //once every second
	wallp = set;

	//setup stuff
	seed = time(NULL);

	//load/create all the sprites and stuff
	//if one of them has an error then everything
	//is aborted
	if(initTrees() || initMountains()){
		return 1;
	}

	return 0;
}

extern "C" int deinit(void){
	//clean everything up
	///hheh
	return 0;
}

extern "C" int redraw(void){
	wallp->renderBuff->clear(sf::Color(16, 65, 132));

	wallp->renderBuff->draw(renderedMountainSprite);
	wallp->renderBuff->draw(renderedTreeSprite);

	wallp->renderBuff->display();

	return 0;
}

int initMountains(void){
	//create grass texture for each fragment
	sf::Texture * MountainTexture = new sf::Texture;
	MountainTexture->loadFromFile("mountains.png");

	//crate sprite for each fragment
	mountainSegmentSprite = new sf::Sprite;
	mountainSegmentSprite->setTexture(*MountainTexture);
	mountainSegmentSprite->setScale(2, 2);

	//create texture and sprite to hold rendered fragments
	renderedMountainTexture = new sf::RenderTexture();
	renderedMountainTexture->create(wallp->width, wallp->height);

	renderedMountainSprite.setTexture(renderedMountainTexture->getTexture());
	renderedMountainSprite.setPosition(0, 0);

	renderedMountainTexture =  drawMountains(renderedMountainTexture);

	return 0;
}

sf::RenderTexture* drawMountains(sf::RenderTexture *texture){
	//while there is screen space left keep drawing a random piece of grass
	int i = 0;
	while((mountainSegmentSprite->getTexture()->getSize().x *
			mountainSegmentSprite->getScale().x) * i < wallp->width){

		float x = mountainSegmentSprite->getTexture()->getSize().x * 2 * i;

		mountainSegmentSprite->setPosition(x, wallp->height -
			(mountainSegmentSprite->getTexture()->getSize().y * 2));
		texture->draw(*mountainSegmentSprite);
		i++;
	}

	texture->display();

	return texture;
}

int initTrees(void){
	//create grass texture for each fragment
	sf::Texture * TreeTexture = new sf::Texture;
	TreeTexture->loadFromFile("trees.png");

	//crate sprite for each fragment
	mountainSegmentSprite = new sf::Sprite;
	mountainSegmentSprite->setTexture(*TreeTexture);
	mountainSegmentSprite->setScale(2, 2);

	//create texture and sprite to hold rendered fragments
	renderedTreeTexture = new sf::RenderTexture();
	renderedTreeTexture->create(wallp->width, wallp->height);

	renderedTreeSprite.setTexture(renderedTreeTexture->getTexture());
	renderedTreeSprite.setPosition(0, 0);

	renderedTreeTexture =  drawTrees(renderedTreeTexture);

	return 0;
}

sf::RenderTexture* drawTrees(sf::RenderTexture *texture){
	//while there is screen space left keep drawing a random piece of grass
	int i = 0;
	while((mountainSegmentSprite->getTexture()->getSize().x *
			mountainSegmentSprite->getScale().x) * i < wallp->width){

		float x = mountainSegmentSprite->getTexture()->getSize().x * 2 * i;

		mountainSegmentSprite->setPosition(x, wallp->height -
			(mountainSegmentSprite->getTexture()->getSize().y * 2));
		texture->draw(*mountainSegmentSprite);
		i++;
	}

	texture->display();

	return texture;
}
