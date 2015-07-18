//#include <iostream>
//#include <time.h>
//#include <math.h>
//#include <stdlib.h>
//#include <string>


//#define PAN_FRAMES 60

////TODO: textures in header file

////all information about the wallpaper needed while drawing
//Wallpaper *wallp;

////use the same seed for the entire session so redrawing
////with random values will be consistent
//long seed;

////amount panned left or right
////0 - no pan
////> 0 - pan to right
////< 0 - pan to left
//double panDist;

////holds the sprites that have been directly loaded from the texture files
//sf::Sprite mountainSegmentSprite;
//sf::Sprite treeSegmentSprite;
//sf::Sprite islandSprite;
//sf::Sprite *cloudSprites;

////the segment sprites are rendered to these sprites
////each "rendered" sprite will likely have multiple "segment"
////sprites rendered to it
//sf::Sprite renderedMountainSprite;
//sf::RenderTexture *renderedMountainTexture;

//sf::Sprite renderedTreeSprite;
//sf::RenderTexture *renderedTreeTexture;

////these are all the functions that are called by the wallpaper
//extern "C" int init(Wallpaper *);
//extern "C" int redraw(void);
//extern "C" int deinit(void);
//extern "C" int pan(double dist);

////renders sprites to a given texture
//sf::RenderTexture* drawMountains(sf::RenderTexture *);
//sf::RenderTexture* drawTrees(sf::RenderTexture *);

////loads textures and creates sprites/rendertextures needed for each object
//int initMountains(void);
//int initTrees(void);
//int initIsland(void);
//int initClouds(void);

	//set->refresh = 0.1;
	//wallp = set;

	////start with no pan
	//panDist = 0;

	////setup stuff
	//seed = time(NULL);

	////load/create all the sprites and stuff
	////if one of them has an error then everything
	////is aborted
	//if(initTrees()
		//|| initMountains()
		//|| initClouds()
		//|| initIsland()){
		//return 1;
	//}

	//return 0;
//}

//extern "C" int deinit(void){
	////clean everything up
	/////hheh
	//return 0;
//}

	//wallp->renderBuff->clear(sf::Color(16, 65, 132));

	//renderedMountainSprite.setPosition(panDist * 2, 0);
	//wallp->renderBuff->draw(renderedMountainSprite);

	//renderedTreeSprite.setPosition(panDist * 5, 0);
	//wallp->renderBuff->draw(renderedTreeSprite);

	//islandSprite.setPosition(wallp->width / 4, wallp->height / 2);
	//wallp->renderBuff->draw(islandSprite);

	//cloudSprites[0].setPosition((wallp->width / 5) + (panDist * 10), wallp->height / 1.6);
	//wallp->renderBuff->draw(cloudSprites[0]);

	//cloudSprites[1].setPosition((wallp->width / 1.2) + (panDist * 11), wallp->height / 2);
	//wallp->renderBuff->draw(cloudSprites[1]);

	//cloudSprites[1].setPosition((wallp->width / 10) + (panDist * 12), wallp->height / 1.4);
	//wallp->renderBuff->draw(cloudSprites[1]);

	//cloudSprites[2].setPosition((wallp->width / 2) + (panDist * 7), wallp->height / 2);
	//wallp->renderBuff->draw(cloudSprites[2]);

	//cloudSprites[2].setPosition(((wallp->width / 20) - 300) + (panDist * 9), wallp->height / 2.5);
	//wallp->renderBuff->draw(cloudSprites[2]);

	//wallp->renderBuff->display();

	//return 0;
//}

//int initIsland(void){
	//sf::Texture *islandTexture = new sf::Texture;
	//islandTexture->loadFromFile("island.png");

	////crate sprite for each fragment
	////mountainSegmentSprite = new sf::Sprite;
	//islandSprite.setTexture(*islandTexture);
	//islandSprite.setScale(2, 2);

	//return 0;
//}

//int initMountains(void){
	////create grass texture for each fragment
	//sf::Texture * MountainTexture = new sf::Texture;
	//MountainTexture->loadFromFile("mountains.png");

	////crate sprite for each fragment
	////mountainSegmentSprite = new sf::Sprite;
	//mountainSegmentSprite.setTexture(*MountainTexture);
	//mountainSegmentSprite.setScale(2, 2);

	////create texture and sprite to hold rendered fragments
	//renderedMountainTexture = new sf::RenderTexture();
	//renderedMountainTexture->create(wallp->width, wallp->height);

	//renderedMountainSprite.setTexture(renderedMountainTexture->getTexture());
	//renderedMountainSprite.setPosition(0, 0);

	//renderedMountainTexture =  drawMountains(renderedMountainTexture);

	//return 0;
//}

//int initClouds(void){
	//sf::Texture *cloudTextures = new sf::Texture[3];
	//cloudTextures[0].loadFromFile("cloud0.png");
	//cloudTextures[1].loadFromFile("cloud1.png");
	//cloudTextures[2].loadFromFile("cloud2.png");

	////crate sprite for each fragment
	//cloudSprites = new sf::Sprite[3];

	//cloudSprites[0].setTexture(cloudTextures[0]);
	//cloudSprites[0].setScale(2, 2);

	//cloudSprites[1].setTexture(cloudTextures[1]);
	//cloudSprites[1].setScale(2, 2);

	//cloudSprites[2].setTexture(cloudTextures[2]);
	//cloudSprites[2].setScale(2, 2);

	//return 0;
//}

//sf::RenderTexture* drawMountains(sf::RenderTexture *texture){
	////while there is screen space left keep drawing a random piece of grass
	//int i = 0;
	//while((mountainSegmentSprite.getTexture()->getSize().x *
			//mountainSegmentSprite.getScale().x) * i < wallp->width){

		//float x = mountainSegmentSprite.getTexture()->getSize().x * 2 * i;

		//mountainSegmentSprite.setPosition(x, wallp->height -
			//(mountainSegmentSprite.getTexture()->getSize().y * 2));
		//texture->draw(mountainSegmentSprite);
		//i++;
	//}

	//texture->display();

	//return texture;
//}

//int initTrees(void){
	////create grass texture for each fragment
	//sf::Texture * TreeTexture = new sf::Texture;
	//TreeTexture->loadFromFile("trees.png");

	////crate sprite for each fragment
	//treeSegmentSprite.setTexture(*TreeTexture);
	//treeSegmentSprite.setScale(2, 2);

	////create texture and sprite to hold rendered fragments
	//renderedTreeTexture = new sf::RenderTexture();
	//renderedTreeTexture->create(wallp->width, wallp->height);

	//renderedTreeSprite.setTexture(renderedTreeTexture->getTexture());
	//renderedTreeSprite.setPosition(0, 0);

	//renderedTreeTexture =  drawTrees(renderedTreeTexture);

	//return 0;
//}

//sf::RenderTexture* drawTrees(sf::RenderTexture *texture){
	////while there is screen space left keep drawing a random piece of grass
	//int i = 0;
	//while((treeSegmentSprite.getTexture()->getSize().x *
			//treeSegmentSprite.getScale().x) * i < wallp->width){

		//float x = treeSegmentSprite.getTexture()->getSize().x * 2 * i;

		//treeSegmentSprite.setPosition(x, wallp->height -
			//(treeSegmentSprite.getTexture()->getSize().y * 2));
		//texture->draw(treeSegmentSprite);
		//i++;
	//}

	//texture->display();

	//return texture;
//}
//

#include "Wallpaper.h"

int init(Wallpaper* wallpaper){
	return 0;
}

int signal(int cmd, char* data){
	return 0;
}

int redraw(void){
	return 0;
}
