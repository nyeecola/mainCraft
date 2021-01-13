#include "window.h"
#include "utils.h"

static void
framebuffer_resize_callback(GLFWwindow* window, int width, int height)
{
	struct glfw_callback_data *data = glfwGetWindowUserPointer(window);
	*data->window_resized = true;
}

void
error_callback(int error, const char *description)
{
	// log error
	fputs(description, stderr);
}

int
create_window(GLFWwindow **window, struct glfw_callback_data *data_pointer, char *window_title)
{
	// set error callback
	glfwSetErrorCallback(error_callback);

	// create window
	*window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, window_title, NULL, NULL);
	if (!*window) {
		print_error("Failed to create window!");
		goto return_error;
	}

	glfwSetWindowUserPointer(*window, data_pointer);

	glfwSetFramebufferSizeCallback(*window, framebuffer_resize_callback);

	glfwSetInputMode(*window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(*window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	// set key callback
	if(!glfwSetKeyCallback(*window, key_callback))
		return 0;

	print_error("Failed to setup key callback!");

	glfwDestroyWindow(*window);
return_error:
	return -1;
}

