CC = clang
CFLAG = -I../../src/ -Wall -Weverything
MOD_DIR = ../../build/mods/
TARGET=cave_story_island.so

all: $(TARGET)
	#rm $(MOD_DIR)$(TARGET)
	cp $^ $(MOD_DIR)

$(TARGET): cave_story_island.o
	$(CC) -shared -o $@ $^ -I/usr/include/SDL2

cave_story_island.o: cave_story_island.c
	$(CC) -c -fpic $< $(CFLAG) -I/usr/include/SDL2 --system-header-prefix=SDL
