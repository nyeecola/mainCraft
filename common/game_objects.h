#ifndef GAME_OBJECTS_H
#define GAME_OBJECTS_H

#include "vk_types.h"

struct vertex {
	vec2 pos;
	vec3 color;
};

extern struct vertex triangle_vertices[4];
extern uint16_t triangle_indices[6];

#endif //GAME_OBJECTS_H
