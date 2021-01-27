#include <cglm/cglm.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#include <stdio.h>

#define FNL_IMPL
#include "terrain.h"

int
get_seed()
{
	srand(time(NULL));
	return rand();
}

void
init_noise_generator(fnl_state *noise, int seed)
{
	*noise = fnlCreateState();
	noise->seed = seed;
}

void
generate_terrain(vec3 *data, fnl_state *noise, int xi, int zi, int xf, int zf)
{
	int x, z, index = 0;

	for (x = xi; x < xf; x++) {
		for (z = zi; z < zf; z++) {
			data[index][0] = (float) x;
			data[index][1] = -roundf(16 * fnlGetNoise2D(noise, x, z));
			data[index][2] = (float) z;
			index++;
		}
	}
}
