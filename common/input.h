#ifndef INPUT_H
#define INPUT_H

#include <GLFW/glfw3.h>
#include <stdio.h>

/* callback for error events */
void
error_callback(int error, const char *description);

/* callback for key events */
void
key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

#endif //INPUT_H
