#compilers to use
CC = g++

#dirs
SRCDIR=../src/
MODDIR=../src/mods/

#find all objects... this is ass
SRCS = $(patsubst $(SRCDIR)%.cpp,%.cpp,$(wildcard $(SRCDIR)*.cpp))
OBJS = $(patsubst %.cpp,%.o,$(SRCS))

#comp flags
TARGET = walp
CFLAGS = -g -Wall
LIBS = -lsfml-graphics -lsfml-window -lsfml-system -ldl

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LIBS) $(CFLAGS) -o $(TARGET) $^

%.o: $(SRCDIR)%.cpp
	    $(CC) $(CFLAGS) -c $<

clean:
	rm -rf *.o
	rm -rf $(TARGET)
