#include <stdlib.h>
#include <stdio.h>

#include "vk_command_buffer.h"
#include "constants.h"
#include "utils.h"


VkCommandPool
create_command_pool(VkDevice logical_device, uint32_t family_index, VkCommandPoolCreateFlags flags)
{
	VkCommandPool command_pool;
	VkResult result;

	VkCommandPoolCreateInfo pool_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.queueFamilyIndex = family_index,
		.flags = flags // Optional
	};

	result = vkCreateCommandPool(logical_device, &pool_info, NULL, &command_pool);
	if (result != VK_SUCCESS) {
		print_error("Failed to create command pool!");
		return VK_NULL_HANDLE;
	}

	return command_pool;
}

VkCommandBuffer *
create_command_buffers(VkDevice logical_device, VkCommandPool pool, VkCommandBufferLevel level, uint32_t count)
{
	VkCommandBuffer *command_buffers;
	VkResult result;

	command_buffers = malloc(sizeof(VkCommandBuffer) * count);
	if (!command_buffers) {
		print_error("Failed to allocate command buffer handle vector!");
		return NULL;
	}

	VkCommandBufferAllocateInfo alloc_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.commandPool = pool,
		.level = level,
		.commandBufferCount = count,
	};

	result = vkAllocateCommandBuffers(logical_device, &alloc_info, command_buffers);
	if (result != VK_SUCCESS) {
		print_error("Failed to allocate command buffer handles!");
		free(command_buffers);
		return NULL;
	}

	return command_buffers;
}

void
cleanup_command_pools(VkDevice logical_device, VkCommandPool command_pools[])
{
	int i;

	for (i = 0; i < queues_count; i++)
		if (command_pools[i] != VK_NULL_HANDLE)
			vkDestroyCommandPool(logical_device, command_pools[i], NULL);
}

void
free_command_buffer_vector(VkCommandBuffer *cmd_buffers[])
{
	int i;

	for (i = 0; i < queues_count; i++)
		if (!cmd_buffers[i])
			free(cmd_buffers[i]);
}

int
create_cmd_submission_infra(struct vk_device *device)
{
	uint32_t *cmd_buffers_count = device->cmd_submission.cmd_buffers_count;
	const uint32_t *family_index = device->cmd_submission.family_indices;
	const uint32_t images_count = device->swapchain.images_count;
	VkCommandBuffer **cmd_buffer = device->cmd_submission.cmd_buffers;
	VkCommandPool *cmd_pool = device->cmd_submission.command_pools;

	/* If we don't have a transfer queue we need to use the graphics queue to
	 * transfer the buffers and images, then we need to set the flag
	 * VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT to reuse graphics
	 * command buffers to transfer things and render things by reseting command
	 * buffers.
	 *  */
	if (device->cmd_submission.queue_count[transfer])
		cmd_pool[graphics] = create_command_pool(device->logical_device, family_index[graphics], 0);
	else
		cmd_pool[graphics] = create_command_pool(device->logical_device, family_index[graphics],
												 VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
	if (cmd_pool[graphics] == VK_NULL_HANDLE)
		goto return_error;

	cmd_buffer[graphics] = create_command_buffers(device->logical_device, cmd_pool[graphics],
												  VK_COMMAND_BUFFER_LEVEL_PRIMARY, images_count);
	if (!cmd_buffer[graphics])
		goto cleanup_command_pools;

	cmd_buffers_count[graphics] = images_count;

	if (device->cmd_submission.queue_count[transfer]) {
		cmd_pool[transfer] = create_command_pool(device->logical_device, family_index[transfer],
												 VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
		if (cmd_pool[transfer] == VK_NULL_HANDLE)
			goto cleanup_command_pools;

		cmd_buffer[transfer] = create_command_buffers(device->logical_device, cmd_pool[transfer],
													  VK_COMMAND_BUFFER_LEVEL_PRIMARY, images_count);
		if (!cmd_buffer[transfer])
			goto cleanup_command_pools;

		cmd_buffers_count[transfer] = images_count;
	}

	return 0;

cleanup_command_pools:
	cleanup_command_pools(device->logical_device, cmd_pool);
	free_command_buffer_vector(cmd_buffer);
return_error:
	return -1;
}

int
record_draw_cmd(struct vk_cmd_submission *cmd_sub, struct vk_swapchain *swapchain,
				struct vk_render *render, struct vk_game_objects *game_objects)
{
	struct vk_vertex_object *obj = &game_objects->dummy_triangle;
	VkCommandBuffer **cmd_buffers = cmd_sub->cmd_buffers;
	VkBuffer vertex_buffers[] = { obj->vertex_buffer };
	VkBuffer index_buffer = obj->index_buffer;
	uint32_t index_count = obj->indices_count;
	VkDeviceSize offsets[] = { 0 };
	VkResult result;
	int i;

	for (i = 0; i < swapchain->images_count; i++) {
		VkCommandBufferBeginInfo begin_info = {
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
			.flags = 0,
		};

		result = vkBeginCommandBuffer(cmd_buffers[graphics][i], &begin_info);
		if (result != VK_SUCCESS) {
			print_error("Failed to begin recording command buffer!");
			return -1;
		}

		VkClearValue clear_values = {
			/* workarround: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=80454 */
			.color = { { SKY_COLOR_RED, SKY_COLOR_GREEN , SKY_COLOR_BLUE , SKY_COLOR_ALPHA } }
		};

		VkRenderPassBeginInfo render_pass_info = {
			.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
			.renderPass = render->render_pass,
			.framebuffer = render->swapChain_framebuffers[i],
			.renderArea.offset = { 0, 0 },
			.renderArea.extent = swapchain->state.extent,
			.clearValueCount = 1,
			.pClearValues = &clear_values
		};

		vkCmdBeginRenderPass(cmd_buffers[graphics][i], &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(cmd_buffers[graphics][i], VK_PIPELINE_BIND_POINT_GRAPHICS, render->graphics_pipeline);

		vkCmdBindVertexBuffers(cmd_buffers[graphics][i], 0, array_size(vertex_buffers), vertex_buffers, offsets);

		vkCmdBindIndexBuffer(cmd_buffers[graphics][i], index_buffer, 0, VK_INDEX_TYPE_UINT16);

		vkCmdBindDescriptorSets(cmd_buffers[graphics][i], VK_PIPELINE_BIND_POINT_GRAPHICS,
								render->pipeline_layout, 0, 1, &cmd_sub->descriptor_sets[i], 0, NULL);

		vkCmdDrawIndexed(cmd_buffers[graphics][i], index_count, 1, 0, 0, 0);

		vkCmdEndRenderPass(cmd_buffers[graphics][i]);

		result = vkEndCommandBuffer(cmd_buffers[graphics][i]);
		if (result != VK_SUCCESS) {
			print_error("Failed to record command buffer!");
			return -1;
		}
	}
	return 0;
}

// Boiler plate to start a One Time submit buffer
VkResult
begin_single_time_commands(VkCommandBuffer cmd_buffer)
{
	VkResult result;

	VkCommandBufferBeginInfo begin_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT
	};

	result = vkBeginCommandBuffer(cmd_buffer, &begin_info);
	if (result != VK_SUCCESS)
		print_error("Failed while beggining command buffer record!");

	return result;
}

// Boiler plate to finish a One Time submit buffer
VkResult
end_single_time_commands(VkCommandBuffer cmd_buffer, VkQueue queue)
{
	VkResult result;

	result = vkEndCommandBuffer(cmd_buffer);
	if (result != VK_SUCCESS) {
		print_error("Failed while finishing command buffer record!");
		return result;
	}

	VkSubmitInfo submit_info = {
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
		.commandBufferCount = 1,
		.pCommandBuffers = &cmd_buffer
	};

	/* TODO: Maybe we could be interesting use fences to do that if we have a more
	 * complex scenario with a lot of transfer and other stuffs going on
	 * */
	result = vkQueueSubmit(queue, 1, &submit_info, VK_NULL_HANDLE);
	if (result != VK_SUCCESS) {
		print_error("Failed to submit command buffer!");
		return result;
	}

	vkQueueWaitIdle(queue);

	return result;
}

