all: build

build:
	gcc -Wall -Wextra -g main.c list.c tree.c helper_functions.c -o sd_fs

clean:
	rm *.o sd_fs

run:
	./sd_fs
