HEADERS = opengl/gl_backend.h opengl/extras.h common/input.h vulkan/vk_backend.h vulkan/vk_window.h common/constants.h
SOURCES = opengl/gl_backend.c opengl/extras.c common/input.c vulkan/vk_backend.c vulkan/vk_window.c main.c
LIBS = -lftgl -lGL -lglfw -lvulkan
INCLUDES = -I opengl -I vulkan -I common -I /usr/include/freetype2

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
