HEADERS = opengl/gl_backend.h opengl/extras.h common/input.h
SOURCES = main.c opengl/gl_backend.c opengl/extras.c common/input.c
LIBS = -lftgl -lGL -lglfw
INCLUDES = -I opengl -I common -I /usr/include/freetype2

all: compile

asm: $(HEADERS) $(SOURCES)
	$(CC) -S $(SOURCES) $(INCLUDES) $(LIBS)

compile: $(HEADERS) $(SOURCES)
	$(CC) $(SOURCES) $(INCLUDES) $(LIBS) -o mainCraft.run

run: mainCraft.run
	./mainCraft.run

clean:
	rm *.o
	rm *.s
	rm mainCraft.run
