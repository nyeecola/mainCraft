#ifndef TYPES_H
#define TYPES_H

#include <cglm/cglm.h>
#include <stdbool.h>

#include "FastNoise/FastNoiseLite.h"

enum key { SPACE = 0, A, W, S, D, key_count };
enum coodinates { X = 0, Y, coordinate_count };

struct input {
	bool key_pressed[key_count];
	double mouse_position[coordinate_count];
};

struct glfw_callback_data {
	bool *window_resized;
	struct input *input;
};

struct game_configs {
	float mouse_speed;
	float FoV;
};

struct player_info {
	float horizontal_angle;
	float vertical_angle;
	vec3 position;
	vec3 looking_at;
};

struct game_terrain {
	fnl_state noise;
};

struct game_data {
	struct game_configs configs;
	struct player_info player;
	struct game_terrain terrain;
	double last_frame_time;
};


#endif //TYPES_H
