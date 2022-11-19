FLAGS = -std=c11 -lm $(shell pkg-config --cflags --libs check)
all: main.c
	mkdir -p build
	gcc -g -Wall -Werror main.c -o build/main $(FLAGS)
	./build/main
clean:
	rm -f *.o
	rm -fr build/
