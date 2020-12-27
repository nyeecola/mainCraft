#include <stdlib.h>
#include <stdio.h>

#include "vk_command_buffer.h"
#include "utils.h"


VkCommandPool
create_command_pool(VkDevice logical_device, uint32_t family_index, VkCommandPoolCreateFlags flags)
{
	VkCommandPool command_pool;

	VkCommandPoolCreateInfo pool_info = {
		.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
		.queueFamilyIndex = family_index,
		.flags = flags // Optional
	};

	if (vkCreateCommandPool(logical_device, &pool_info, NULL, &command_pool) != VK_SUCCESS) {
		print_error("Failed to create command pool!");
		return VK_NULL_HANDLE;
	}

	return command_pool;
}

VkCommandBuffer *
create_command_buffers(VkDevice logical_device, VkCommandPool pool, VkCommandBufferLevel level, uint32_t count)
{
	VkCommandBuffer *command_buffers = malloc(sizeof(VkCommandBuffer) * count);
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

	if (vkAllocateCommandBuffers(logical_device, &alloc_info, command_buffers) != VK_SUCCESS) {
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
	const uint32_t *family_index = device->queues.family_indices;
	const uint32_t images_count = device->swapchain.images_count;
	VkCommandBuffer **cmd_buffer = device->cmd_buffers;
	VkCommandPool *cmd_pool = device->command_pools;

	cmd_pool[graphics] = create_command_pool(device->logical_device, family_index[graphics], 0);
	if (cmd_pool[graphics] == VK_NULL_HANDLE)
		goto return_error;

	cmd_buffer[graphics] = create_command_buffers(device->logical_device, cmd_pool[graphics],
												  VK_COMMAND_BUFFER_LEVEL_PRIMARY, images_count);
	if (!cmd_buffer[graphics])
		goto cleanup_command_pools;

	device->cmd_buffers_count[graphics] = images_count;

	if (device->queues.queue_count[transfer]) {
		cmd_pool[transfer] = create_command_pool(device->logical_device, family_index[transfer],
												 VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
		if (cmd_pool[transfer] == VK_NULL_HANDLE)
			goto cleanup_command_pools;

		cmd_buffer[transfer] = create_command_buffers(device->logical_device, cmd_pool[transfer],
													  VK_COMMAND_BUFFER_LEVEL_PRIMARY, images_count);
		if (!cmd_buffer[transfer])
			goto cleanup_command_pools;

		device->cmd_buffers_count[transfer] = images_count;
	}

	return 0;

cleanup_command_pools:
	cleanup_command_pools(device->logical_device, cmd_pool);
	free_command_buffer_vector(cmd_buffer);
return_error:
	return -1;
}
