#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>

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

#endif //TYPES_H
