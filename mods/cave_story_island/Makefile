CC = clang
CFLAG = -I../../src/ -Wall -Weverything
MOD_DIR = ../../build/mods/
TARGET=cave_story_island.so

IMAGES=$(wildcard res/*.bmp)
IMAGES_AS_HEADER=$(IMAGES:.bmp=.h)

all: $(TARGET)

install: $(TARGET)
	cp $^ $(MOD_DIR)

$(TARGET): cave_story_island.o
	$(CC) -shared -o $@ $^ -I/usr/include/SDL2 -lSDL2_gfx

cave_story_island.o: cave_story_island.c cave_story_island.h
	$(CC) -c -fpic $< $(CFLAG) -I/usr/include/SDL2 --system-header-prefix=SDL -lSDL2_gfx -lSDL2_image

cave_story_island.h: $(IMAGES_AS_HEADER)

res/%.h: res/%.bmp
	xxd -i $^ > $@

clean:
	rm -f *.o
	rm -f *.so
	rm -f res/*.h
