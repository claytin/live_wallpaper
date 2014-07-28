#compilers to use
CC = g++

#dirs
OBJDIR=./obj/
SRCDIR=../src/
BINDIR=./bin/
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

#%.o: %.cpp
#	$(CC) $(CPFLAGS) -c $< -o $(patsubst $(SRCDIR)%,$(OBJDIR)%,$@)

%.o: $(SRCDIR)%.cpp
	    $(CC) $(CFLAGS) -c $<

clean:
	rm -rf $(OBJDIR)/*.o
	rm -rf $(BINDIR)$(TARGET)
