CC = g++
CFLAG = -I../../ -std=c++11
MOD_DIR = ../../../build/mods/
TARGET=bitday.so

all: $(TARGET)
	rm $(MOD_DIR)$(TARGET)
	cp $^ $(MOD_DIR)

$(TARGET): bitday.o
	$(CC) -shared -o $@ $^

bitday.o: bitday.cpp
	$(CC) -c -fpic $< $(CFLAG)
