#include "input.h"
#include "constants.h"
#include "vk_window.h"

GLFWwindow *
vk_init_window()
{
	// define window
	GLFWwindow* window;

	// try to initialize glfw, abort on failure
	if (!glfwInit())
		goto return_null;

	// Tell to glfw to not create a OpenGL/OpenGL ES context
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	// set error callback
	glfwSetErrorCallback(error_callback);

	// create window
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Main Craft", NULL, NULL);
	if (!window)
		goto terminate_glfw;

	// set key callback
	if(!glfwSetKeyCallback(window, key_callback))
		return window;

	glfwDestroyWindow(window);
terminate_glfw:
	glfwTerminate();
return_null:
	return NULL;
}

void
vk_destroy_window(GLFWwindow *window)
{
	glfwDestroyWindow(window);
	glfwTerminate();
}


