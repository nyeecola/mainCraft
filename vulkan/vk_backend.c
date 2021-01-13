#include <stdlib.h>
#include <stdio.h>

#include "vk_resource_manager.h"
#include "vk_window.h"
#include "vk_draw.h"

int
vk_main_loop(struct vk_program *program)
{
	uint8_t current_frame = 0;
	int ret = 0;
	struct vk_device *dev = &program->device;

	while (!glfwWindowShouldClose(program->window)) {
		glfwPollEvents();
		if (draw_frame(program, &current_frame)) {
			ret = -1;
			break;
		}
	}

	vkDeviceWaitIdle(dev->logical_device);

	return ret;
}

int
run_vk(const int argc, char *const *argv)
{
	int exit_status = EXIT_FAILURE;
	struct vk_program program = { };

	if (vk_init_window(&program))
		goto exit_program;

	if (init_vk(&program))
		goto destroy_window;

	if (vk_main_loop(&program))
		goto vk_cleanup;

	exit_status = EXIT_SUCCESS;

vk_cleanup:
	vk_cleanup(&program);
destroy_window:
	vk_destroy_window(program.window);
exit_program:
	return exit_status;
}

