#include "input.h"

void
error_callback(int error, const char *description)
{
	// log error
	fputs(description, stderr);
}

void
key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	// if key is escape: close window
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}
