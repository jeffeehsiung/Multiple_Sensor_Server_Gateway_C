FLAGS = -std=c11 -lpthread -lm $(shell pkg-config --cflags --libs check)

all: sensor_node connmgr
	@echo "done"

sensor_node: sensor_node.c 
	mkdir -p build
	gcc sensor_node.c ./lib/tcpsock.c -Wall -Werror -o ./build/sensor_node -DLOOPS=10

sensor_node_usingsharedlib: 
	gcc sensor_node.c -Wall -Werror -c -o ./build/sensor_node.o -DLOOPS=10
	gcc ./build/sensor_node.o -ltcpsock -L./build/lib/ -Wl,-rpath=./lib/ -o ./build/sensor_node


connmgr: connmgr.c tcpsock
	gcc connmgr.c -Wall -Werror -c -o ./build/connmgr.o $(FLAGS)
	gcc ./build/connmgr.o -ltcpsock -L./build/lib/ -Wl,-rpath=./lib/ -o ./build/connmgr

tcpsock: lib/tcpsock.c
	mkdir -p ./build/lib
	gcc -c -fPIC ./lib/tcpsock.c -o ./build/lib/tcpsock.o
	gcc -shared -o ./build/lib/libtcpsock.so ./build/lib/tcpsock.o

run: connmgr sensor_node
	cd build; ./connmgr 1234 &
	sleep 2
	cd build; ./sensor_node 101 1 127.0.0.1 1234 &
	cd build; ./sensor_node 102 3 127.0.0.1 1234 &
	cd build; ./sensor_node 103 2 127.0.0.1 1234 &
