compile:
	gcc -c main.c

link:
	gcc main.o -o main -lglfw -lGL

run:
	./main

clean:
	rm main.o
	rm main

all: compile link
