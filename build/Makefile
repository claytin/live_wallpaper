#compilers to use
CC = clang

#dirs
SRCDIR=../src/
MODDIR=../src/mods/

#comp flags
TARGET = walp
CFLAGS = -g -Wall -Weverything -D_REENTRANT -I/usr/include/SDL2 --system-header-prefix=SDL
LIBS = -ldl -lSDL2 -lpthread

all: $(TARGET)

$(TARGET): Wallpaper.o
	$(CC) $(LIBS) $(CFLAGS) -o $(TARGET) $^

%.o: $(SRCDIR)%.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o
	rm -rf $(TARGET)
