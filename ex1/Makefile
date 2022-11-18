FLAGS = -std=c11 -lm $(shell pkg-config --cflags --libs check)
all: main.c datamgr.c lib/dplist.c
	mkdir -p build
	gcc -g -Wall -Werror -DSET_MAX_TEMP=25 -DSET_MIN_TEMP=5 main.c datamgr.c lib/dplist.c -o build/datamgr $(FLAGS)
	./build/datamgr

file_creator: file_creator.c
	mkdir -p build
	gcc -Wall -Werror -DDEBUG file_creator.c -o build/file_creator
	./build/file_creator

clean:
	rm -f *.o
	rm -fr build/
