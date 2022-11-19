FLAGS = -std=c11 -lm $(shell pkg-config --cflags --libs check)
all: sensor_db.c sensor_db.h config.h main.c
	mkdir -p build
	gcc -g -Wall -Werror main.c sensor_db.c  -o build/sensor_db $(FLAGS)
	./build/sensor_db

file_creator: file_creator.c
	mkdir -p build
	gcc -Wall -Werror -DDEBUG file_creator.c -o build/file_creator
	./build/file_creator

clean:
	rm -f *.o
	rm -fr build/
