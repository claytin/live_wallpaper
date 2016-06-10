CC = g++
CFLAG = -I../../ -std=c++11 -Wall
MOD_DIR = ../../build/mods/
TARGET=bitday.so

all: $(TARGET)

$(TARGET): bitday.o
	$(CC) -shared -o $@ $^

bitday.o: bitday.cpp
	$(CC) -c -fpic $< $(CFLAG)

install: $(TARGET)
	cp $^ $(MOD_DIR)
