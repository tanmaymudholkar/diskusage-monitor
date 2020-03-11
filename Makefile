vpath %.a lib
vpath %.so lib
vpath %.o obj
vpath %.cpp src
vpath %.h include
vpath % bin

all: run_monitor

run_monitor: filesystem_interface.o run_monitor.o
	g++ -o bin/run_monitor obj/run_monitor.o obj/filesystem_interface.o `root-config --cflags --glibs`

filesystem_interface.o: filesystem_interface.h filesystem_interface.cpp
	g++ -g -c -std=c++17 -Wall -Wextra -Werror -pedantic-errors -fPIC -O3 -o obj/filesystem_interface.o src/filesystem_interface.cpp

run_monitor.o: run_monitor.cpp run_monitor.h
	g++ -g -c -std=c++17 -Wall -Wextra -Werror -pedantic-errors -fPIC -O3 -o obj/run_monitor.o src/run_monitor.cpp `root-config --cflags --glibs`

clean:
	rm -rf bin/*
	rm -rf obj/*
