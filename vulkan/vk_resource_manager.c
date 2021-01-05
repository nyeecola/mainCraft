#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vk_resource_manager.h"
#include "vk_logical_device.h"
#include "vk_command_buffer.h"
#include "vk_swapchain.h"
#include "game_objects.h"
#include "vk_instance.h"
#include "vk_backend.h"
#include "vk_buffer.h"
#include "vk_render.h"
#include "vk_draw.h"
#include "utils.h"

int
create_render_and_presentation_infra(struct vk_program *program)
{
	struct vk_device *dev = &program->device;
	struct vk_render *render = &dev->render;
	struct vk_swapchain *swapchain = &dev->swapchain;

	if (create_swapchain(dev, program->surface, program->window))
		goto exit_error;

	if (create_image_views(dev->logical_device, swapchain))
		goto destroy_swapchain;

	render->render_pass = create_render_pass(dev->logical_device, swapchain->state);
	if (render->render_pass == VK_NULL_HANDLE)
		goto destroy_image_views;

	if (create_graphics_pipeline(dev->logical_device, &swapchain->state, render))
		goto destroy_render_pass;

	if (create_framebuffers(dev->logical_device, swapchain, &dev->render))
		goto destroy_graphics_pipeline;

	if (create_cmd_submission_infra(dev))
		goto destroy_framebuffers;

	return 0;

destroy_framebuffers:
	framebuffers_cleanup(dev->logical_device, render->swapChain_framebuffers, render->framebuffer_count);
destroy_graphics_pipeline:
	vkDestroyPipeline(dev->logical_device, render->graphics_pipeline, NULL);
	vkDestroyPipelineLayout(dev->logical_device, render->pipeline_layout, NULL);
destroy_render_pass:
	vkDestroyRenderPass(dev->logical_device, render->render_pass, NULL);
destroy_image_views:
	image_views_cleanup(dev->logical_device, swapchain->image_views, swapchain->images_count);
destroy_swapchain:
	vkDestroySwapchainKHR(dev->logical_device, dev->swapchain.handle, NULL);
exit_error:
	return -1;
}

void
destroy_render_and_presentation_infra(struct vk_device *dev)
{
	struct vk_render *render = &dev->render;
	struct vk_swapchain *swapchain = &dev->swapchain;

	cleanup_command_pools(dev->logical_device, dev->command_pools);
	free_command_buffer_vector(dev->cmd_buffers);
	framebuffers_cleanup(dev->logical_device, render->swapChain_framebuffers, render->framebuffer_count);
	vkDestroyPipeline(dev->logical_device, dev->render.graphics_pipeline, NULL);
	vkDestroyPipelineLayout(dev->logical_device, render->pipeline_layout, NULL);
	vkDestroyRenderPass(dev->logical_device, dev->render.render_pass, NULL);
	image_views_cleanup(dev->logical_device, swapchain->image_views, swapchain->images_count);
	vkDestroySwapchainKHR(dev->logical_device, swapchain->handle, NULL);
}

int
init_vk(struct vk_program *program)
{
	struct vk_device *dev = &program->device;
	struct vk_vertex_object *triangle = &dev->game_objs.dummy_triangle;

	program->app_info = create_app_info();
	program->instance = create_instance(&program->app_info);
	if (program->instance == VK_NULL_HANDLE) {
		print_error("Failed to create a vulkan instance!");
		goto exit_error;
	}

	if (glfwCreateWindowSurface(program->instance, program->window, NULL, &program->surface) != VK_SUCCESS) {
		print_error("Failed to create a Window surface!");
		goto destroy_instance;
	}

	if (pick_physical_device(program->instance, dev, program->surface))
		goto destroy_surface;

	if (create_logical_device(dev))
		goto destroy_surface_support;

	if (create_render_and_presentation_infra(program))
		goto destroy_device;

	/* TODO: remove this, and either
	 * - Add a function to from these informationsload a file or
	 * - Create a function to do this
	 */
	triangle->vertices = triangle_vertices;
	triangle->vertices_count = array_size(triangle_vertices);
	triangle->indices = triangle_indices;
	triangle->indices_count = array_size(triangle_indices);

	if (create_vertex_buffer(dev, triangle))
		goto destroy_render_and_presentation_infra;

	if (create_index_buffer(dev, triangle))
		goto destroy_vertex_shader;

	if (record_draw_cmd(dev->cmd_buffers, &dev->swapchain, &dev->render, &dev->game_objs))
		goto destroy_index_shader;

	if (create_sync_objects(dev->logical_device, &dev->draw_sync, dev->swapchain.images_count))
		goto destroy_render_and_presentation_infra;

	return 0;

destroy_render_and_presentation_infra:
	destroy_render_and_presentation_infra(dev);
destroy_index_shader:
	vkDestroyBuffer(dev->logical_device, triangle->index_buffer, NULL);
	vkFreeMemory(dev->logical_device, triangle->index_buffer_memory, NULL);
destroy_vertex_shader:
	vkDestroyBuffer(dev->logical_device, triangle->vertex_buffer, NULL);
	vkFreeMemory(dev->logical_device, triangle->vertex_buffer_memory, NULL);
destroy_device:
	vkDestroyDevice(dev->logical_device, NULL);
destroy_surface_support:
	surface_support_cleanup(&dev->swapchain.support);
destroy_surface:
	vkDestroySurfaceKHR(program->instance, program->surface, NULL);
destroy_instance:
	vkDestroyInstance(program->instance, NULL);
exit_error:
	return -1;
}

void
vk_cleanup(struct vk_program program)
{
	struct vk_device *dev = &program.device;
	struct vk_vertex_object *triangle = &dev->game_objs.dummy_triangle;

	sync_objects_cleanup(dev->logical_device, &dev->draw_sync);
	vkDestroyBuffer(dev->logical_device, triangle->index_buffer, NULL);
	vkFreeMemory(dev->logical_device, triangle->index_buffer_memory, NULL);
	vkDestroyBuffer(dev->logical_device, triangle->vertex_buffer, NULL);
	vkFreeMemory(dev->logical_device, triangle->vertex_buffer_memory, NULL);
	destroy_render_and_presentation_infra(dev);
	surface_support_cleanup(&dev->swapchain.support);
	vkDestroyDevice(dev->logical_device, NULL);
	vkDestroySurfaceKHR(program.instance, program.surface, NULL);
	vkDestroyInstance(program.instance, NULL);
}

int
recreate_render_and_presentation_infra(struct vk_program *program)
{
	struct vk_device *dev = &program->device;
	int width = 0, height = 0;

	glfwGetFramebufferSize(program->window, &width, &height);
	// Deal with the "special" case of windows being minimized
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(program->window, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(dev->logical_device);

	// It is needed because we had a windows resize
	query_surface_support(dev->physical_device, program->surface, &dev->swapchain.support);

	destroy_render_and_presentation_infra(dev);
	if (create_render_and_presentation_infra(program)) {
		print_error("Failed to recreate the render and presentation infrastructure!");
		return -1;
	}

	if (record_draw_cmd(dev->cmd_buffers, &dev->swapchain, &dev->render, &dev->game_objs))
		return -1;

	return 0;
}
