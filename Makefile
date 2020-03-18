CXX = g++
ROOT_CFLAGS := $(shell root-config --cflags)
ROOT_GLIBS := $(shell root-config --glibs)
CXXFLAGS := -g $(ROOT_CFLAGS) -Wall -Wextra -Werror -pedantic-errors -fPIC -O3

vpath %.a lib
vpath %.so lib
vpath %.o obj
vpath %.cpp src
vpath %.h include
vpath % bin

all: run_monitor

run_monitor: filesystem_interface.o cacheFile_interface.o run_monitor.o
	$(CXX) -pthread -o bin/run_monitor obj/filesystem_interface.o obj/cacheFile_interface.o obj/run_monitor.o $(ROOT_GLIBS)

filesystem_interface.o: filesystem_interface.h filesystem_interface.cpp
	$(CXX) $(CXXFLAGS) -c -o obj/filesystem_interface.o src/filesystem_interface.cpp

cacheFile_interface.o: filesystem_interface.h cacheFile_interface.h cacheFile_interface.cpp
	$(CXX) $(CXXFLAGS) -c -o obj/cacheFile_interface.o src/cacheFile_interface.cpp $(ROOT_GLIBS)

run_monitor.o: filesystem_interface.h cacheFile_interface.h run_monitor.h run_monitor.cpp
	$(CXX) $(CXXFLAGS) -c -o obj/run_monitor.o src/run_monitor.cpp

clean:
	rm -rf bin/*
	rm -rf obj/*
