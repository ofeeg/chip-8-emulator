CXX=g++
CPPFLAGS=-Wall -Wextra
CXXFLAGS=-std=c++17
#INCLUDES=-I "/home/projects/chip-8-emulator"

#CXXFLAGS += -Ofast
CXXFLAGS += -Og -g #-fsanitize=address
#CXXFLAGS += -fopenmp
#CPPFLAGS += -I
CXXFLAGS += $(shell pkg-config sdl2 --cflags)
LDLIBS += $(shell pkg-config sdl2 --libs)

OBJS=low_level.o main.o platform.o 

all: $(OBJS)
	$(CXX) -o $@ $(OBJS) $(CXXFLAGS) $(LDLIBS)
.cpp.o:
	$(CXX) $(CPPFLAGS) -c $<

clean:
	-rm -f all *.o
