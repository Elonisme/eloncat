all: eloncat

eloncat: main.c
	gcc -o eloncat -O3 main.c

clean:
	rm -f eloncat
