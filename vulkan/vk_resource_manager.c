#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vk_resource_manager.h"
#include "vk_logical_device.h"
#include "vk_command_buffer.h"
#include "vk_descriptors.h"
#include "vk_swapchain.h"
#include "game_objects.h"
#include "vk_instance.h"
#include "vk_backend.h"
#include "vk_texture.h"
#include "vk_buffer.h"
#include "vk_render.h"
#include "vk_image.h"
#include "vk_draw.h"
#include "utils.h"

int
create_render_and_presentation_infra(struct vk_program *program)
{
	struct window *game_window = &program->game_window;
	struct vk_device *dev = &program->device;
	struct vk_render *render = &dev->render;
	struct vk_swapchain *swapchain = &dev->swapchain;
	struct view_projection *camera = &dev->game_objs.camera;

	if (create_swapchain(dev, game_window->surface, game_window->window))
		goto exit_error;

	if (create_swapchain_image_views(dev->logical_device, swapchain))
		goto destroy_swapchain;

	render->render_pass = create_render_pass(dev->logical_device, swapchain->state);
	if (render->render_pass == VK_NULL_HANDLE)
		goto destroy_image_views;

	if (create_graphics_pipeline(dev->logical_device, &swapchain->state, render))
		goto destroy_render_pass;

	if (create_framebuffers(dev->logical_device, swapchain, &dev->render))
		goto destroy_graphics_pipeline;

	if (create_mvp_buffers(dev, camera))
		goto destroy_framebuffers;

	/* Update the projection matrix to handle a possible windows resize */
	calculate_projection(camera->proj, swapchain->state.extent);

	dev->cmd_submission.descriptor_pool = create_descriptor_pool(dev->logical_device, swapchain);
	if (dev->cmd_submission.descriptor_pool == VK_NULL_HANDLE)
		goto destroy_mvp_buffers;

	if (create_descriptor_sets(dev, &dev->cmd_submission, render->descriptor_set_layout))
		goto destroy_descriptor_pool;

	return 0;

	/* Descriptor sets are destroyed *here* */
destroy_descriptor_pool:
	vkDestroyDescriptorPool(dev->logical_device, dev->cmd_submission.descriptor_pool, NULL);
destroy_mvp_buffers:
	destroy_uniform_buffers(dev, camera->buffers, camera->buffers_memory, camera->buffer_count);
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
	vkDestroySwapchainKHR(dev->logical_device, swapchain->handle, NULL);
exit_error:
	return -1;
}

void
destroy_render_and_presentation_infra(struct vk_device *dev)
{
	struct vk_render *render = &dev->render;
	struct vk_swapchain *swapchain = &dev->swapchain;
	struct view_projection *camera = &dev->game_objs.camera;

	/* Descriptor sets are destroyed *here* */
	destroy_uniform_buffers(dev, camera->buffers, camera->buffers_memory, camera->buffer_count);
	vkDestroyDescriptorPool(dev->logical_device, dev->cmd_submission.descriptor_pool, NULL);
	free(dev->cmd_submission.descriptor_sets);
	free(camera->buffers_memory);
	free(camera->buffers);

	/* Cleanup pipeline resources */
	framebuffers_cleanup(dev->logical_device, render->swapChain_framebuffers, render->framebuffer_count);
	vkDestroyPipeline(dev->logical_device, dev->render.graphics_pipeline, NULL);
	vkDestroyPipelineLayout(dev->logical_device, render->pipeline_layout, NULL);
	vkDestroyRenderPass(dev->logical_device, dev->render.render_pass, NULL);

	/* Cleanup swapchain resources*/
	image_views_cleanup(dev->logical_device, swapchain->image_views, swapchain->images_count);
	vkDestroySwapchainKHR(dev->logical_device, swapchain->handle, NULL);
}

int
_create_render_and_presentation_infra(struct vk_program *program)
{
	struct vk_device *dev = &program->device;
	VkCommandBuffer **cmd_buffer = dev->cmd_submission.cmd_buffers;
	VkCommandPool *cmd_pool = dev->cmd_submission.command_pools;
	uint32_t buffer_count = dev->swapchain.images_count;

	if (create_render_and_presentation_infra(program))
		goto return_error;

	cmd_buffer[graphics] = alloc_command_buffers(dev->logical_device, cmd_pool[graphics],
												 VK_COMMAND_BUFFER_LEVEL_PRIMARY, buffer_count);
	if (!cmd_buffer[graphics])
		goto destroy_render_and_presentation_infra;

	return 0;

destroy_render_and_presentation_infra:
	destroy_render_and_presentation_infra(dev);
return_error:
	return -1;
}

void
_destroy_render_and_presentation_infra(struct vk_device *dev)
{
	uint32_t *cmd_buffers_count = dev->cmd_submission.cmd_buffers_count;
	VkCommandBuffer **cmd_buffer = dev->cmd_submission.cmd_buffers;
	VkCommandPool *cmd_pool = dev->cmd_submission.command_pools;

	destroy_render_and_presentation_infra(dev);

	/* Free grahics command buffers */
	vkFreeCommandBuffers(dev->logical_device, cmd_pool[graphics], cmd_buffers_count[graphics], cmd_buffer[graphics]);
	free_command_buffer_vector(dev->cmd_submission.cmd_buffers);
}

int
init_vk(struct vk_program *program)
{
	struct vk_device *dev = &program->device;
	struct vk_vertex_object *cube = &dev->game_objs.cube;
	struct vk_cmd_submission *cmd_sub = &dev->cmd_submission;
	struct window *game_window = &program->game_window;
	struct vk_render *render = &dev->render;
	VkResult result;

	program->app_info = create_app_info();
	program->instance = create_instance(&program->app_info);
	if (program->instance == VK_NULL_HANDLE) {
		print_error("Failed to create a vulkan instance!");
		goto exit_error;
	}

	result = glfwCreateWindowSurface(program->instance, game_window->window, NULL, &game_window->surface);
	if (result != VK_SUCCESS) {
		print_error("Failed to create a Window surface!");
		goto destroy_instance;
	}

	if (pick_physical_device(program->instance, dev, game_window->surface))
		goto destroy_surface;

	if (create_logical_device(dev))
		goto destroy_surface_support;

	if (create_command_pools(dev))
		goto destroy_device;

	if (create_command_buffers(dev, dev->swapchain.support.capabilities.minImageCount + 1))
		goto destroy_command_pools;

	if (create_texture_image(dev, "assets/textures/stone_bricks.png", &cube->texture_image, &cube->texture_image_memory))
		goto destroy_command_pools;

	cube->texture_image_view = create_texture_image_view(dev->logical_device, cube->texture_image);
	if (cube->texture_image_view == VK_NULL_HANDLE)
		goto destroy_texture_image;

	render->texture_sampler = create_texture_sampler(dev->logical_device, &dev->device_properties.device_properties);
	if (render->texture_sampler == VK_NULL_HANDLE)
		goto destroy_texture_image_view;

	dev->render.descriptor_set_layout = create_descriptor_set_layout_binding(dev->logical_device);
	if (dev->render.descriptor_set_layout == VK_NULL_HANDLE)
		goto destroy_texture_sampler;

	if (create_render_and_presentation_infra(program))
		goto destroy_descriptor_set_layout;

	/* TODO: remove this, and either
	 * - Add a function to from these informationsload a file or
	 * - Create a function to do this
	 */
	cube->vertices = cube_vertices;
	cube->vertices_count = array_size(cube_vertices);
	cube->indices = cube_vertex_indices;
	cube->indices_count = array_size(cube_vertex_indices);

	if (create_vertex_buffer(dev, cube))
		goto destroy_render_and_presentation_infra;

	if (create_index_buffer(dev, cube))
		goto destroy_vertex_shader;

	if (record_draw_cmd(cmd_sub, &dev->swapchain, &dev->render, &dev->game_objs))
		goto destroy_index_shader;

	if (create_sync_objects(dev->logical_device, &dev->draw_sync, dev->swapchain.images_count))
		goto destroy_index_shader;

	return 0;

destroy_index_shader:
	vkDestroyBuffer(dev->logical_device, cube->index_buffer, NULL);
	vkFreeMemory(dev->logical_device, cube->index_buffer_memory, NULL);
destroy_vertex_shader:
	vkDestroyBuffer(dev->logical_device, cube->vertex_buffer, NULL);
	vkFreeMemory(dev->logical_device, cube->vertex_buffer_memory, NULL);
destroy_render_and_presentation_infra:
	destroy_render_and_presentation_infra(dev);
destroy_descriptor_set_layout:
	vkDestroyDescriptorSetLayout(dev->logical_device, dev->render.descriptor_set_layout, NULL);
destroy_texture_sampler:
	vkDestroySampler(dev->logical_device, render->texture_sampler, NULL);
destroy_texture_image_view:
	vkDestroyImageView(dev->logical_device, cube->texture_image_view, NULL);
destroy_texture_image:
	vkDestroyImage(dev->logical_device, cube->texture_image, NULL);
	vkFreeMemory(dev->logical_device, cube->texture_image_memory, NULL);
destroy_command_pools:
	cleanup_command_pools(dev->logical_device, dev->cmd_submission.command_pools);
	free_command_buffer_vector(dev->cmd_submission.cmd_buffers);
destroy_device:
	vkDestroyDevice(dev->logical_device, NULL);
destroy_surface_support:
	surface_support_cleanup(&dev->swapchain.support);
destroy_surface:
	vkDestroySurfaceKHR(program->instance, game_window->surface, NULL);
destroy_instance:
	vkDestroyInstance(program->instance, NULL);
exit_error:
	return -1;
}

void
vk_cleanup(struct vk_program *program)
{
	struct window *game_window = &program->game_window;
	struct vk_device *dev = &program->device;
	struct vk_vertex_object *cube = &dev->game_objs.cube;
	struct vk_render *render = &dev->render;

	/* Destroy the draw synchronization primitives */
	sync_objects_cleanup(dev->logical_device, &dev->draw_sync);

	/* Destroy vertex and index buffer and buffer memory */
	vkDestroyBuffer(dev->logical_device, cube->index_buffer, NULL);
	vkFreeMemory(dev->logical_device, cube->index_buffer_memory, NULL);
	vkDestroyBuffer(dev->logical_device, cube->vertex_buffer, NULL);
	vkFreeMemory(dev->logical_device, cube->vertex_buffer_memory, NULL);

	/* clean texture resources */
	vkDestroySampler(dev->logical_device, render->texture_sampler, NULL);
	vkDestroyImageView(dev->logical_device, cube->texture_image_view, NULL);
	vkDestroyImage(dev->logical_device, cube->texture_image, NULL);
	vkFreeMemory(dev->logical_device, cube->texture_image_memory, NULL);

	destroy_render_and_presentation_infra(dev);

	/* Free command submission resources */
	cleanup_command_pools(dev->logical_device, dev->cmd_submission.command_pools);
	free_command_buffer_vector(dev->cmd_submission.cmd_buffers);

	/* Destroy the uniform buffer MVP descriptor set layout, used in the graphics pipeline */
	vkDestroyDescriptorSetLayout(dev->logical_device, dev->render.descriptor_set_layout, NULL);

	surface_support_cleanup(&dev->swapchain.support);

	vkDestroyDevice(dev->logical_device, NULL);

	vkDestroySurfaceKHR(program->instance, game_window->surface, NULL);

	vkDestroyInstance(program->instance, NULL);
}

int
recreate_render_and_presentation_infra(struct vk_program *program)
{
	struct window *game_window = &program->game_window;
	struct vk_device *dev = &program->device;
	struct vk_cmd_submission *cmd_sub = &dev->cmd_submission;
	int width = 0, height = 0;

	glfwGetFramebufferSize(game_window->window, &width, &height);
	// Deal with the "special" case of windows being minimized
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(game_window->window, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(dev->logical_device);

	// It is needed because we had a windows resize
	query_surface_support(dev->physical_device, game_window->surface, &dev->swapchain.support);

	_destroy_render_and_presentation_infra(dev);
	if (_create_render_and_presentation_infra(program)) {
		print_error("Failed to recreate the render and presentation infrastructure!");
		return -1;
	}

	if (record_draw_cmd(cmd_sub, &dev->swapchain, &dev->render, &dev->game_objs))
		return -1;

	return 0;
}
