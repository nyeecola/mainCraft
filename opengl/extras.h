#ifndef EXTRAS_H
#define EXTRAS_H

#include <GLFW/glfw3.h>
#include <FTGL/ftgl.h>
#include <stdio.h>

/* FPS counter */
struct FPS {
	double timer;
	double last_time;
	double current_time;
	double total_elapsed_time;
	double num_frames;
	char value[40];
};

/* get FPS */
void
calculate_FPS(struct FPS *fps, double cooldown);

#endif //EXTRAS_H
