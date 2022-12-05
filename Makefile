FLAGS = -std=c11 -lpthread -lm $(shell pkg-config --cflags --libs check)
all: main.c sbuffer.c
	gcc -g -Wall -Werror main.c sbuffer.c -o build/sbuffer $(FLAGS)
	./build/sbuffer

file_creator: file_creator.c
	mkdir -p build
	gcc -Wall -Werror -DDEBUG file_creator.c -o build/file_creator
	./build/file_creator

zip:
	zip milestone3.zip *.c *.h
