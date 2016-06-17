#compilers to use
CC = clang

#dirs
ROOT_DIR=..
SRC_DIR=$(ROOT_DIR)/src

AVAILABLE_MOD_DIR=$(ROOT_DIR)/mods
INSTALL_MOD_DIR=$(ROOT_DIR)/build/mods

MODS=$(notdir $(addsuffix .so,$(wildcard $(AVAILABLE_MOD_DIR)/*)))

#comp flags
TARGET = walp
CFLAGS = -g -Wall -Weverything `pkg-config sdl2 SDL2_gfx --cflags` --system-header-prefix=SDL
LIBS = -ldl -lpthread `pkg-config sdl2 SDL2_gfx --libs`

all: $(TARGET) mods

$(TARGET): Wallpaper.o X11_root_output.o Bmp_output.o Window_output.o
	$(CC) $(LIBS) $(CFLAGS) -o $(TARGET) $^

%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $<

mods: $(addprefix $(INSTALL_MOD_DIR)/, $(MODS))

$(INSTALL_MOD_DIR)/%.so: $(AVAILABLE_MOD_DIR)/%
	cd $< && $(MAKE) && $(MAKE) install

list_mods:
	@echo "mods:"
	@echo $(join $(MODS), \n)

clean:
	rm -rf *.o
	rm -rf ./mods/*
	rm -rf $(TARGET)
