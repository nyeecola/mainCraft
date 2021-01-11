#ifndef GAME_OBJECTS_H
#define GAME_OBJECTS_H

#include <stdalign.h>
#include <cglm/cglm.h>

#include "vk_types.h"

struct MVP {
	alignas(16) mat4 model;
	alignas(16) mat4 view_proj;
};

struct vertex {
	vec3 pos;
	vec2 texCoord;
};

extern struct vertex cube_vertices[20];
extern uint16_t cube_vertex_indices[36];

#endif //GAME_OBJECTS_H
