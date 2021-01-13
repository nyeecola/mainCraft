#include "input.h"
#include "constants.h"
#include "vk_window.h"

static void
framebuffer_resize_callback(GLFWwindow* window, int width, int height) {
	struct vk_swapchain *swapchain = glfwGetWindowUserPointer(window);
	swapchain->framebuffer_resized = true;
}

int
vk_init_window(struct vk_program *program)
{
	struct window *game_window = &program->game_window;

	// try to initialize glfw, abort on failure
	if (!glfwInit())
		goto return_error;

	// Tell to glfw to not create a OpenGL/OpenGL ES context
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	// set error callback
	glfwSetErrorCallback(error_callback);

	// create window
	program->window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Main Craft (Vulkan)", NULL, NULL);
	if (!program->window)
		goto terminate_glfw;

	glfwSetWindowUserPointer(program->window, &program->device.swapchain);
	glfwSetFramebufferSizeCallback(program->window, framebuffer_resize_callback);

	// set key callback
	if(!glfwSetKeyCallback(program->window, key_callback))
		return 0;

	glfwDestroyWindow(program->window);
terminate_glfw:
	glfwTerminate();
return_error:
	return -1;
}

void
vk_destroy_window(struct window *game_window)
{
	glfwDestroyWindow(game_window->window);
	glfwTerminate();
}


