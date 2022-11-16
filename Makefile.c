all: 
	mkdir -p build
	gcc -g -Wall -Werror -D SET_MIN_TEMP=15 -D SET_MAX_TEMP=25 main.c datamgr.c datamgr.h lib/dplist.c lib/dplist.h -o build/datamgr $(FLAGS)

file_creator: 
	gcc -g -Wall -Werror -DDEBUG file_creator.c -o file_creator

clean:
	rm build/*

