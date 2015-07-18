CC = clang
CFLAG = -I../../src/ -Wall -Weverything
MOD_DIR = ../../build/mods/
TARGET=cave_story_island.so

all: $(TARGET)
	#rm $(MOD_DIR)$(TARGET)
	cp $^ $(MOD_DIR)

$(TARGET): cave_story_island.o
	$(CC) -shared -o $@ $^

cave_story_island.o: cave_story_island.c
	$(CC) -c -fpic $< $(CFLAG)
