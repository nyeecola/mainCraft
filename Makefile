HEADERS = opengl/gl_backend.h opengl/extras.h \
		  vulkan/vk_backend.h vulkan/vk_window.h vulkan/vk_instance.h vulkan/vk_logical_device.h vulkan/vk_constants.c vulkan/vk_types.h vulkan/vk_constants.h \
		  common/input.h common/constants.h common/utils.h
SOURCES = opengl/gl_backend.c opengl/extras.c \
		  vulkan/vk_backend.c vulkan/vk_window.c vulkan/vk_instance.c vulkan/vk_logical_device.c vulkan/vk_constants.c \
		  common/input.c main.c
LIBS = -lftgl -lGL -lglfw -lvulkan
INCLUDES = -I opengl -I vulkan -I common -I /usr/include/freetype2
CFLAGS := -Wall

all: compile

asm: $(HEADERS) $(SOURCES)
	$(CC) -S $(SOURCES) $(INCLUDES) $(LIBS)

compile: $(HEADERS) $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) $(INCLUDES) $(LIBS) -o mainCraft.run

debug: $(HEADERS) $(SOURCES)
	$(CC) $(CFLAGS) -ggdb3 -Og $(SOURCES) $(INCLUDES) $(LIBS) -o mainCraft.run -D ENABLE_VALIDATION_LAYERS

run: mainCraft.run
	./mainCraft.run

clean:
	rm *.o
	rm *.s
	rm mainCraft.run
