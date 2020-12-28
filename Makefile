TARGET_EXEC ?= mainCraft.run

PROJ_DIRS ?= opengl vulkan common
SOURCES := $(wildcard $(addsuffix /*.c,$(PROJ_DIRS))) main.c

HEADER_DIRS += $(PROJ_DIRS) /usr/include/freetype2/
INCLUDES += $(addprefix -I,$(HEADER_DIRS))

LIB_NAMES ?= ftgl GL glfw vulkan m
LD_LIBS += $(addprefix -l,$(LIB_NAMES))

CFLAGS += -Wall

ifeq (${XDG_SESSION_TYPE}, wayland)
	MACROS += -D GLFW_USE_WAYLAND=ON
endif

all: compile shaders

asm: $(SOURCES)
	$(CC) -S $^ $(INCLUDES) $(LD_LIBS)

compile: $(SOURCES)
	$(CC) $(CFLAGS) $^ $(INCLUDES) $(LD_LIBS) -o $(TARGET_EXEC) $(MACROS)

debug: CFLAGS += -ggdb3 -Og -fstack-protector-all -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer
debug: compile

debug_vk: MACROS += -D ENABLE_VALIDATION_LAYERS
debug_vk: debug

.PHONY: shaders
shaders: shaders/dummy_shader.vert shaders/dummy_shader.frag
	glslangValidator -V shaders/dummy_shader.vert -o shaders/vert.spv
	glslangValidator -V shaders/dummy_shader.frag -o shaders/frag.spv

run: $(TARGET_EXEC)
	./$(TARGET_EXEC)

clean:
	$(RM) *.o *.s $(TARGET_EXEC)
