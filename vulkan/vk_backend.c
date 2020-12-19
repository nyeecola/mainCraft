#include <stdlib.h>
#include <stdio.h>

#include "vk_logical_device.h"
#include "vk_instance.h"
#include "vk_backend.h"
#include "vk_window.h"
#include "utils.h"


int
init_vk(struct vk_program *program)
{
	program->app_info = create_app_info();
	if ((program->instance = create_instance(&program->app_info)) == NULL)
		goto exit_error;

	if (glfwCreateWindowSurface(program->instance, program->window, NULL, &program->surface) != VK_SUCCESS) {
		fprintf(stderr, "Error: Failed to create a Window surface!\n");
		goto destroy_instance;
	}

	if (pick_physical_device(program->instance, &program->device, program->surface))
		goto destroy_surface;

	return 0;

destroy_surface:
	vkDestroySurfaceKHR(program->instance, program->surface, NULL);
destroy_instance:
	vkDestroyInstance(program->instance, NULL);
exit_error:
	return 1;
}

void
vk_cleanup(struct vk_program program)
{
	vkDestroySurfaceKHR(program.instance, program.surface, NULL);
	vkDestroyInstance(program.instance, NULL);
}

int
run_vk(const int argc, char *const *argv)
{
	int exit_status = EXIT_FAILURE;
	struct vk_program program = { };

	program.window = vk_init_window();
	if (!program.window)
		goto exit_program;

	if (init_vk(&program))
		goto destroy_window;

	exit_status = EXIT_SUCCESS;

	vk_cleanup(program);
destroy_window:
	vk_destroy_window(program.window);
exit_program:
	return exit_status;
}

