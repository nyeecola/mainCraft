compile:
	gcc -c main.c

link:
	gcc *.o -o mainCraft.run -lglfw -lGL

run:
	./mainCraft.run

clean:
	rm *.o
	rm mainCraft.run

all: compile link
