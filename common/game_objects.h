#ifndef GAME_OBJECTS_H
#define GAME_OBJECTS_H

#include <stdalign.h>
#include <cglm/cglm.h>

#include "vk_types.h"

struct vertex {
	vec3 pos;
	vec2 texCoord;
};

extern struct vertex cube_vertices[20];
extern uint16_t cube_vertex_indices[36];

#endif //GAME_OBJECTS_H
