HEADERS=sbuffer.h valgrind/*.h
SOURCE=sbuffer.c

FLAGS = -std=c11 -lpthread -lm $(shell pkg-config --cflags --libs check)

all: main.c sbuffer.c
	gcc -g -Wall -Werror -D _GNU_DOURCE main.c sbuffer.c -o build/sbuffer $(FLAGS)
	gcc ./build/sbuffer 

sbuffer: sbuffer.c 
	mkdir -p build
	gcc -g -Wall -Werror -D _GNU_DOURCE sbuffer.c -o ./build/sbuffer $(FLAGS)
	./build/sbuffer

check : $(SOURCE) $(HEADERS)
	@echo "Running cppcheck :"
	cppcheck --enable=all --suppress=missingIncludeSystem --error-exitcode=1 $(SOURCE) $(HEADERS)

zip:
	zip milestone3.zip *.c *.h