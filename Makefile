FLAGS = -std=c11 -lpthread -lm $(shell pkg-config --cflags --libs check)
CPATH = g++ -I /home/jeffee/chiehfei.hsiung/ex1/sensor_db.h

test_server: test_server.c lib/tcpsock.c
	gcc test_server.c lib/tcpsock.c -o test_server

sensor_node: sensor_node.c lib/tcpsock.c
	gcc sensor_node.c lib/tcpsock.c -o sensor_node

runserver: test_server
	./test_server

runclient1: sensor_node
	./sensor_node 1 2 127.0.0.1 5678

runclient2: sensor_node
	./sensor_node 2 5 127.0.0.1 5678

all: main.c sensor_node.c test_server.c lib/tcpsock.c
	gcc -g -Wall -Werror _GNU_SOURCE main.c sensor_node.c test_server.c lib/tcpsock.c -o connmgr $(FLAGS)
	./connmgr

file_creator: file_creator.c
	gcc -Wall -Werror file_creator.c -o build/file_creator
	./build/file_creator

clean:
	rm -f *.o
	rm -r build/*

zip:
	zip milestone2.zip *.c *.h *.log *.csv 

