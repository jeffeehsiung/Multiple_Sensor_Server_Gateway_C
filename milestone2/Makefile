FLAGS = -std=c11 -lpthread -lm $(shell pkg-config --cflags --libs check)
CPATH = g++ -I /home/jeffee/chiehfei.hsiung/ex1/sensor_db.h

all: main.c logger.c logger.h sensor_db.c sensor_db.h config.h
	mkdir -p build
	gcc -g -Wall -Werror -D _GNU_SOURCE main.c sensor_db.c logger.c -o build/logger $(FLAGS)
	./build/logger

file_creator: file_creator.c
	mkdir -p build
	gcc -Wall -Werror file_creator.c -o build/file_creator
	./build/file_creator

clean:
	rm -f *.o
	rm -r build/*

zip:
	zip milestone2.zip *.c *.h *.log *.csv

