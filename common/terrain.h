#ifndef TERRAIN_H
#define TERRAIN_H

#include "types.h"

int
get_seed();

void
init_noise_generator(fnl_state *noise, int seed);

void
generate_terrain(vec3 *data, fnl_state *noise, int xi, int yi, int xf, int yf);

#endif //TERRAIN_H
