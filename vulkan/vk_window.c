#include "vk_window.h"
#include "window.h"

int
vk_init_window(struct vk_program *program, struct glfw_callback_data *data)
{
	struct window *game_window = &program->game_window;

	// try to initialize glfw, abort on failure
	if (!glfwInit())
		goto return_error;

	// Tell to glfw to not create a OpenGL/OpenGL ES context
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	if (!create_window(&game_window->window, data, "Main Craft (Vulkan)"))
		return 0;

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


