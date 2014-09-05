#include <iostream>
#include <SFML/Graphics.hpp>
#include <dlfcn.h>

#include "Wallpaper.h"

int main(int argc, char **argv){
	if(argc < 2){
		std::cout << "gotta include a wallpaper" << std::endl;
		return 1;
	}
	
	void * wallpaper_program = dlopen(argv[1], RTLD_NOW);

	char * error;
	if((error = dlerror()) != NULL){
		std::cout << "DL Error: " << error << std::endl;
		return 1;
	}

	int (*redraw)();
	int (*init)(Wallpaper*);
	int (*pan)(int);

	redraw = (int (*)())dlsym(wallpaper_program, "redraw");
	init = (int (*)(Wallpaper*))dlsym(wallpaper_program, "init");
	pan = (int (*)(int))dlsym(wallpaper_program, "pan");

	if((error = dlerror()) != NULL){
		std::cout << "DL Error: " << error << std::endl;
		return 1;
	}

	//wallpaper will render to this
	sf::RenderTexture buff;

	//defaults and stuff
	Wallpaper wallset = {
		640 * 2,
		480,
		60,
		&buff
	};

	if((*init)(&wallset)){
		return 1;
	}
	
	//now that all the settings have been resolved. itt is time
	buff.create(wallset.width, wallset.height);

	sf::RenderWindow window(sf::VideoMode(wallset.width, wallset.height),
		"SFML works!");

	while(true){
		(*redraw)();

		(*pan)(1);
		//draw it to the window
		sf::Sprite sprite(wallset.renderBuff->getTexture());
		
		window.clear();
		window.draw(sprite);
		window.display();
		
		sf::sleep(sf::milliseconds(1000 * wallset.refresh));
	}

	dlclose(wallpaper_program);

	return 0;
}
