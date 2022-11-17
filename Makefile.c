all: main file_creator

main: main.c datamgr.c lib/dplist.c
	mkdir -p build
	gcc -Wall -Werror -DSET_MAX_TEMP=25 -DSET_MIN_TEMP=5 -o build/datamgr main.c datamgr.c lib/dplist.c
	./build/datamgr

file_creator: file_creator.c
	mkdir -p build
	gcc -Wall -Werror -DDEBUG -o build/file_creator file_creator.c
	./build/file_creator

clean:
	rm -r build
