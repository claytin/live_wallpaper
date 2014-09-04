CC = g++
CFLAG = -I../../ -std=c++11 -Wall
MOD_DIR = ../../../build/mods/
TARGET=cave_story_island.so

all: $(TARGET)
	rm $(MOD_DIR)$(TARGET)
	cp $^ $(MOD_DIR)

$(TARGET): cave_story_island.o
	$(CC) -shared -o $@ $^

cave_story_island.o: cave_story_island.cpp
	$(CC) -c -fpic $< $(CFLAG)
