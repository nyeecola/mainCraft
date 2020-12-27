#include <stdlib.h>
#include <stdio.h>

#include "vk_logical_device.h"
#include "vk_command_buffer.h"
#include "vk_instance.h"
#include "vk_backend.h"
#include "vk_swapchain.h"
#include "vk_window.h"
#include "utils.h"


int
init_vk(struct vk_program *program)
{
	struct vk_device *dev = &program->device;

	program->app_info = create_app_info();
	if ((program->instance = create_instance(&program->app_info)) == NULL)
		goto exit_error;

	if (glfwCreateWindowSurface(program->instance, program->window, NULL, &program->surface) != VK_SUCCESS) {
		print_error("Failed to create a Window surface!");
		goto destroy_instance;
	}

	if (pick_physical_device(program->instance, dev, program->surface))
		goto destroy_surface;

	if (create_logical_device(dev))
		goto destroy_surface_support;

	if (create_swapchain(dev, program->surface, program->window))
		goto destroy_device;

	if (create_image_views(dev->logical_device, &dev->swapchain))
		goto destroy_swapchain;

	if (create_cmd_submission_infra(dev))
		goto destroy_image_views;

	return 0;

destroy_image_views:
	image_views_cleanup(dev->logical_device, dev->swapchain);
destroy_swapchain:
	vkDestroySwapchainKHR(dev->logical_device, dev->swapchain.handle, NULL);
destroy_device:
	vkDestroyDevice(dev->logical_device, NULL);
destroy_surface_support:
	surface_support_cleanup(&dev->swapchain.support);
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
	struct vk_device *dev = &program.device;

	cleanup_command_pools(dev->logical_device, dev->command_pools);
	free_command_buffer_vector(dev->cmd_buffers);
	image_views_cleanup(dev->logical_device, dev->swapchain);
	vkDestroySwapchainKHR(dev->logical_device, dev->swapchain.handle, NULL);
	surface_support_cleanup(&dev->swapchain.support);
	vkDestroyDevice(dev->logical_device, NULL);
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

