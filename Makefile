asm:
	gcc -S main.c

compile:
	gcc -c main.c -I/usr/include/freetype2

link:
	gcc main.o -o mainCraft.run -lglfw -lGL -lftgl

run:
	./mainCraft.run

clean:
	rm *.o
	rm mainCraft.run

all: compile link
