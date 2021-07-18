CC = clang
CFLAG = -I../../src/ -Wall -Weverything
MOD_DIR = ../../build/mods/
TARGET=cave_story_island.so

IMAGES=$(wildcard res/*.bmp)
IMAGES_AS_HEADER=$(IMAGES:.bmp=.h)

CFLAGS = -g -Wall -Weverything `pkg-config sdl2 SDL2_gfx --cflags` --system-header-prefix=SDL

all: $(TARGET)

install: $(TARGET)
	cp $^ $(MOD_DIR)

$(TARGET): cave_story_island.o
	$(CC) -shared -o $@ $^ -lSDL2_gfx

cave_story_island.o: cave_story_island.c cave_story_island.h
	$(CC) -c -fpic $< $(CFLAG) $(CFLAGS)

cave_story_island.h: $(IMAGES_AS_HEADER)

res/%.h: res/%.bmp
	xxd -i $^ > $@

clean:
	rm -f *.o
	rm -f *.so
	rm -f res/*.h
