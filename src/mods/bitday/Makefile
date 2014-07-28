CC = g++
CFLAG = -I../../
MOD_DIR = ../../../build/mods/

all: bitday.so
	cp $^ $(MOD_DIR)

bitday.so: bitday.o
	$(CC) -shared -o $@ $^

bitday.o: bitday.cpp
	$(CC) -c -fpic $< $(CFLAG)
