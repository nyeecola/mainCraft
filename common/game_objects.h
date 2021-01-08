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
	vec2 pos;
	vec3 color;
};

extern struct vertex triangle_vertices[4];
extern uint16_t triangle_indices[6];

#endif //GAME_OBJECTS_H
