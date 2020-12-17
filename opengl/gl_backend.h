#ifndef GL_BACKEND_H
#define GL_BACKEND_H

#include <GL/glx.h>
#include <GLFW/glfw3.h>

int
setup();

GLFWwindow *
init_window();

void
destroy_window(GLFWwindow *window);

int
main_loop(GLFWwindow *window);

#endif //GL_BACKEND_H
